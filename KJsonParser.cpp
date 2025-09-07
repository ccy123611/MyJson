#include"Headerfiles.h"

//KJsonParser.cpp
static const int nestingLimit = 1024;

enum {
    Space = 0x20,           // 空格
    Tab = 0x09,             // 制表符
    LineFeed = 0x0a,        // 换行
    Return = 0x0d,          // 回车
    BeginArray = 0x5b,      // 开始数组 [
    BeginObject = 0x7b,     // 开始对象 {
    EndArray = 0x5d,        // 结束数组 ]
    EndObject = 0x7d,       // 结束对象 }
    NameSeparator = 0x3a,   // 名称分隔符 :
    ValueSeparator = 0x2c,  // 值分隔符 ,
    Quote = 0x22,           // 引号 "
    ForwardSlash = 0x2f,     // 正斜杠/
    Asterisk = 0x2a
};

KJsonParser::KJsonParser(const char* json, int length) : head(json), json(json), nestingLevel(0), lastError("") {
    end = json + length;
}

void KJsonParser::skipBOM() {
    unsigned char utf8bom[3] = { 0xef, 0xbb, 0xbf };
    if (end - json > 3 &&
        static_cast<unsigned char>(json[0]) == utf8bom[0] &&
        static_cast<unsigned char>(json[1]) == utf8bom[1] &&
        static_cast<unsigned char>(json[2]) == utf8bom[2]) {
        json += 3;  // 跳过 BOM
    }
}

bool KJsonParser::skipSpace() {
    while (json < end) {
        if (*json > Space)  // 发现非空格字符
            break;
        if (*json != Space && *json != Tab && *json != LineFeed && *json != Return)
            break;
        ++json;
    }
    return json < end;
}

char KJsonParser::nextToken() {
    while (json < end) {
        // 跳过空白字符
        if (!skipSpace())
            return 0;

        char token = *json;

        // 如果遇到注释符号
        if (token == ForwardSlash) {
            // 检查注释类型，单行或多行
            if (*(json + 1) == ForwardSlash) {
                // 单行注释，跳过该行
                json += 2;
                while (json < end && *json != LineFeed) {
                    ++json;
                }
                // 继续下一个循环以找到真正的token
                continue;
            }
            else if (*(json + 1) == Asterisk) {
                // 多行注释，跳过直到结束符号 */
                json += 2;
                while (json < end - 1 && !(*json == Asterisk && *(json + 1) == ForwardSlash)) {
                    ++json;
                }
                json += 2; // 跳过结束符号 */
                continue;
            }
        }

        // 找到了非空白字符并且不是注释
        json++;
        return token;
    }
    return 0; // 如果没有更多的字符，返回0
}

/*在这个函数中存在一个问题，如果遇到注释如何处理，实际上JSON格式是不允许注释的，因此在测试中删除注释进行测试，如果后续能够解决则添加注释处理功能*/
bool KJsonParser::parseObject(KJsonObject& jsonObject) {
    if (++nestingLevel > nestingLimit) {
        lastError = "Deep nesting error"; //超出嵌套限制
        return false;
    }

    char token = nextToken();
    while (token == Quote) {
        if (!parseMember(jsonObject)) {
            return false;
        }

        token = nextToken();
        if (token != ValueSeparator)
            break;

        token = nextToken();
        if (token == EndObject) {
            lastError = "Missing closing brace for object";//缺少对象结束符
            return false;
        }
    }

    if (token != EndObject) {
        lastError = "Unterminated object";//不符合对象格式
        return false;
    }

    --nestingLevel;
    return true;
}
/* member指的是一个对象的键值对。*/
bool KJsonParser::parseMember(KJsonObject& jsonObject) {
    std::string key;          // 处理key
    if (!parseString(key))
        return false;

    char token = nextToken();
    skipSpace();
    if (token != NameSeparator) {
        lastError = "Missing name separator";//冒号缺失
        return false;
    }

    JsonValue value;          // 处理token
    if (!parseValue(value))
        return false;

    jsonObject[key] = value;  // 动态添加键值对
    return true;
}

bool KJsonParser::parseArray(KJsonArray& jsonArray) {
    if (++nestingLevel > nestingLimit) {
        lastError = "Deep nesting";//嵌套层数过深
        return false;
    }

    char token = nextToken();
    if (token == EndArray) {
        return true;
    }
    json--;
    while (true) {
        std::shared_ptr<JsonValue> arrayElement = std::make_shared<JsonValue>();
        if (!parseValue(*arrayElement))
            return false;

        jsonArray.add(arrayElement);

        token = nextToken();
        if (token == EndArray) {
            break;
        }
        else if (token != ValueSeparator) {
            lastError = "Missing value separator";//逗号缺失
            return false;
        }
        skipSpace();
    }

    --nestingLevel;
    return true;
}

bool KJsonParser::parseValue(JsonValue& outValue) {
    switch (*json++) {
    case 'n':
        if (end - json < 3 || strncmp(json, "ull", 3) != 0) {
            lastError = "Illegal value";//值非法
            return false;
        }
        json += 3;
        outValue = nullptr;
        return true;

    case 't':
        if (end - json < 3 || strncmp(json, "rue", 3) != 0) {
            lastError = "Illegal value";//值非法
            return false;
        }
        json += 3;
        outValue = true;
        return true;

    case 'f':
        if (end - json < 4 || strncmp(json, "alse", 4) != 0) {
            lastError = "Illegal value";//值非法
            return false;
        }
        json += 4;
        outValue = false;
        return true;

    case Quote: {
        std::string str;
        if (!parseString(str))
            return false;
        outValue = str;
        return true;
    }

    case BeginArray: {
        KJsonArray jsonArray;
        if (!parseArray(jsonArray))
            return false;
        outValue = jsonArray;
        return true;
    }

    case BeginObject: {
        KJsonObject jsonObject;
        if (!parseObject(jsonObject))
            return false;
        outValue = jsonObject;
        return true;
    }

    default:
        --json;
        double number;
        int typePending = parseNumber(number);
        if (typePending == 0)
            return false;
        else if (typePending == 1)
            outValue = static_cast<int>(number);
        else if (typePending == 2)
            outValue = number;
        return true;
    }
}

inline bool isAsciiDigit(char ch) {
    return std::isdigit(static_cast<unsigned char>(ch));
}

// 将十六进制字符转换为对应的数值，并更新结果
static inline bool addHexDigit(char digit, char32_t* result) {
    *result <<= 4;  // 将当前结果左移4位，准备添加新的4位（一个16进制位）

    int h;
    if (digit >= '0' && digit <= '9') {
        h = digit - '0';
    }
    else if (digit >= 'a' && digit <= 'f') {
        h = digit - 'a' + 10;
    }
    else if (digit >= 'A' && digit <= 'F') {
        h = digit - 'A' + 10;
    }
    else {
        return false;
    }

    *result |= h;  // 将转换后的数值与 result 进行或运算
    return true;
}

// 扫描并解析转义序列，将结果存储在 ch 中
static inline bool scanEscapeSequence(const char*& json, const char* end, char32_t* ch) {
    ++json;  // 跳过反斜杠
    if (json >= end)  // 检查是否超出字符串末尾
        return false;

    char escaped = *json++;  // 获取下一个字符
    switch (escaped) {
    case '"':
        *ch = '"'; break;
    case '\\':
        *ch = '\\'; break;
    case '/':
        *ch = '/'; break;
    case 'b':
        *ch = 0x8; break;  // 退格符
    case 'f':
        *ch = 0xc; break;  // 换页符
    case 'n':
        *ch = 0xa; break;  // 换行符
    case 'r':
        *ch = 0xd; break;  // 回车符
    case 't':
        *ch = 0x9; break;  // 制表符
    case 'u': {
        *ch = 0;  // 初始化为 0
        if (json > end - 4)  // 检查是否有足够的字符来读取4个16进制数字
            return false;
        // 解析4位十六进制数字，组成一个UTF-16字符
        for (int i = 0; i < 4; ++i) {
            if (!addHexDigit(*json, ch))  // 如果转换失败，返回 false
                return false;
            ++json;
        }
        return true;  // 成功解析
    }
    default:
        // 非标准转义字符，直接将其赋值给结果
        *ch = escaped;
        return true;
    }
    return true;
}

// 扫描 UTF-8 字符并将其转换为 Unicode 码点
static inline bool scanUtf8Char(const char*& json, const char* end, char32_t* result) {
    if (json >= end)
        return false;

    unsigned char b = static_cast<unsigned char>(*json++);

    if (b < 0x80) {  // 单字节字符 (ASCII)
        *result = b;
    }
    else if (b < 0xE0) {  // 两字节字符
        if (json >= end || (static_cast<unsigned char>(*json) & 0xC0) != 0x80)
            return false;
        *result = ((b & 0x1F) << 6) | (static_cast<unsigned char>(*json++) & 0x3F);
    }
    else if (b < 0xF0) {  // 三字节字符
        if (json + 1 >= end ||
            (static_cast<unsigned char>(json[0]) & 0xC0) != 0x80 ||
            (static_cast<unsigned char>(json[1]) & 0xC0) != 0x80)
            return false;
        *result = ((b & 0x0F) << 12) |
            ((static_cast<unsigned char>(*json++) & 0x3F) << 6) |
            (static_cast<unsigned char>(*json++) & 0x3F);
    }
    else if (b < 0xF8) {  // 四字节字符
        if (json + 2 >= end ||
            (static_cast<unsigned char>(json[0]) & 0xC0) != 0x80 ||
            (static_cast<unsigned char>(json[1]) & 0xC0) != 0x80 ||
            (static_cast<unsigned char>(json[2]) & 0xC0) != 0x80)
            return false;
        *result = ((b & 0x07) << 18) |
            ((static_cast<unsigned char>(*json++) & 0x3F) << 12) |
            ((static_cast<unsigned char>(*json++) & 0x3F) << 6) |
            (static_cast<unsigned char>(*json++) & 0x3F);
    }
    else {
        return false;  // 非法的 UTF-8 序列
    }

    return true;
}

bool KJsonParser::parseString(std::string& outString)
{
    const char* start = json;

    bool isUtf8 = true;
    bool isAscii = true;
    std::string result;

    // 解析不带转义序列的 UTF-8 字符串
    while (json < end) {
        char32_t ch = 0;
        if (*json == '"') // 检测到字符串结束
            break;
        if (*json == '\\') { // 遇到转义序列
            isAscii = false;
            isUtf8 = false;
            break;
        }

        result += *json++;
    }
    ++json; // 跳过结束的双引号

    if (json >= end) {
        lastError = "Unterminated string";//字符串格式不正确
        return false;
    }

    // 如果没有转义序列，直接返回结果
    if (isUtf8) {
        outString = result;
        return true;
    }

    // 如果包含转义序列，重新开始解析
    json = start;
    result.clear();

    while (json < end) {
        char32_t ch = 0;
        if (*json == '"') // 检测到字符串结束
            break;
        else if (*json == '\\') { // 处理转义字符
            if (!scanEscapeSequence(json, end, &ch)) {
                lastError = "Illegal escape sequence";//转义字符格式不正确
                return false;
            }
        }
        else {
            if (!scanUtf8Char(json, end, &ch)) { // 处理普通字符
                lastError = "Illegal UTF-8 string";//UTF-8字符格式不正确
                return false;
            }
        }

        result += static_cast<char>(ch); // 将解析到的字符添加到结果中
    }
    ++json; // 跳过结束的双引号

    if (json >= end) {
        lastError = "Unterminated string";//字符串格式不正确
        return false;
    }

    outString = result;
    return true;
}

int KJsonParser::parseNumber(double& outNumber)
{
    const char* start = json;
    bool isInt = true;

    // 处理负号
    if (json < end && *json == '-')
        ++json;

    // 处理整数部分
    if (json < end && *json == '0') {
        ++json;  // 处理单个0
    }
    else {
        while (json < end && isAsciiDigit(*json))  // 处理多个数字
            ++json;
    }

    // 处理小数部分
    if (json < end && *json == '.') {
        ++json;
        while (json < end && isAsciiDigit(*json)) {
            isInt = false;
            ++json;
        }
    }

    // 处理指数部分
    if (json < end && (*json == 'e' || *json == 'E')) {
        isInt = false;
        ++json;
        if (json < end && (*json == '-' || *json == '+'))  // 处理指数符号
            ++json;
        while (json < end && isAsciiDigit(*json))
            ++json;
    }

    // 转换字符串为数字
    std::string numberStr(start, json - start);
    try {
        if (isInt) {
            outNumber = std::stoll(numberStr);  // 处理整数
            return 1;
        }
        else {
            outNumber = std::stod(numberStr);  // 处理浮点数
            return 2;
        }
    }
    catch (const std::invalid_argument& e) {
        lastError = "Invalid number format";//无效数字格式
        return false;
    }
    catch (const std::out_of_range& e) {
        lastError = "Number out of range";//数字超出范围
        return false;
    }

    return true;
}

std::shared_ptr<KJson> KJsonParser::parse() {
    // 跳过字节序标记（BOM）
    skipBOM();

    // 获取下一个有效token
    char token = nextToken();

    std::shared_ptr<KJson> data;

    // 判断是否是数组或对象的开始
    if (token == '[') {
        auto jsonArray = std::make_shared<KJsonArray>();
        if (!parseArray(*jsonArray)) {
            // 如果解析数组失败，记录错误并返回空对象
            return std::make_shared<KJsonArray>();
        }
        data = jsonArray; // 创建数组类型的 KJson 对象
    }
    else if (token == '{') {
        auto jsonObject = std::make_shared<KJsonObject>();
        if (!parseObject(*jsonObject)) {
            // 如果解析对象失败，记录错误并返回空对象
            return std::make_shared<KJsonObject>();
        }
        data = jsonObject; // 赋值为解析后的对象
    }
    else {
        lastError = "Unexpected token at the beginning of JSON data";//Json数据起始字符非法
        return std::make_shared<KJsonObject>(); // 返回空对象
    }

    // 跳过可能的空白字符
    skipSpace();

    // 检查是否解析到了文件结尾，确保没有多余内容
    if (json < end) {
        lastError = "Extra data found after valid JSON";//Json有效数据后获取到额外数据
        return std::make_shared<KJsonObject>(); // 返回空对象
    }

    return data; // 返回解析后的 KJson 对象
}
//KJsonParser.cpp