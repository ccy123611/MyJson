#include"Headerfiles.h"

//KJsonParser.cpp
static const int nestingLimit = 1024;

enum {
    Space = 0x20,           // �ո�
    Tab = 0x09,             // �Ʊ��
    LineFeed = 0x0a,        // ����
    Return = 0x0d,          // �س�
    BeginArray = 0x5b,      // ��ʼ���� [
    BeginObject = 0x7b,     // ��ʼ���� {
    EndArray = 0x5d,        // �������� ]
    EndObject = 0x7d,       // �������� }
    NameSeparator = 0x3a,   // ���Ʒָ��� :
    ValueSeparator = 0x2c,  // ֵ�ָ��� ,
    Quote = 0x22,           // ���� "
    ForwardSlash = 0x2f,     // ��б��/
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
        json += 3;  // ���� BOM
    }
}

bool KJsonParser::skipSpace() {
    while (json < end) {
        if (*json > Space)  // ���ַǿո��ַ�
            break;
        if (*json != Space && *json != Tab && *json != LineFeed && *json != Return)
            break;
        ++json;
    }
    return json < end;
}

char KJsonParser::nextToken() {
    while (json < end) {
        // �����հ��ַ�
        if (!skipSpace())
            return 0;

        char token = *json;

        // �������ע�ͷ���
        if (token == ForwardSlash) {
            // ���ע�����ͣ����л����
            if (*(json + 1) == ForwardSlash) {
                // ����ע�ͣ���������
                json += 2;
                while (json < end && *json != LineFeed) {
                    ++json;
                }
                // ������һ��ѭ�����ҵ�������token
                continue;
            }
            else if (*(json + 1) == Asterisk) {
                // ����ע�ͣ�����ֱ���������� */
                json += 2;
                while (json < end - 1 && !(*json == Asterisk && *(json + 1) == ForwardSlash)) {
                    ++json;
                }
                json += 2; // ������������ */
                continue;
            }
        }

        // �ҵ��˷ǿհ��ַ����Ҳ���ע��
        json++;
        return token;
    }
    return 0; // ���û�и�����ַ�������0
}

/*����������д���һ�����⣬�������ע����δ���ʵ����JSON��ʽ�ǲ�����ע�͵ģ�����ڲ�����ɾ��ע�ͽ��в��ԣ���������ܹ���������ע�ʹ�����*/
bool KJsonParser::parseObject(KJsonObject& jsonObject) {
    if (++nestingLevel > nestingLimit) {
        lastError = "Deep nesting error"; //����Ƕ������
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
            lastError = "Missing closing brace for object";//ȱ�ٶ��������
            return false;
        }
    }

    if (token != EndObject) {
        lastError = "Unterminated object";//�����϶����ʽ
        return false;
    }

    --nestingLevel;
    return true;
}
/* memberָ����һ������ļ�ֵ�ԡ�*/
bool KJsonParser::parseMember(KJsonObject& jsonObject) {
    std::string key;          // ����key
    if (!parseString(key))
        return false;

    char token = nextToken();
    skipSpace();
    if (token != NameSeparator) {
        lastError = "Missing name separator";//ð��ȱʧ
        return false;
    }

    JsonValue value;          // ����token
    if (!parseValue(value))
        return false;

    jsonObject[key] = value;  // ��̬��Ӽ�ֵ��
    return true;
}

bool KJsonParser::parseArray(KJsonArray& jsonArray) {
    if (++nestingLevel > nestingLimit) {
        lastError = "Deep nesting";//Ƕ�ײ�������
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
            lastError = "Missing value separator";//����ȱʧ
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
            lastError = "Illegal value";//ֵ�Ƿ�
            return false;
        }
        json += 3;
        outValue = nullptr;
        return true;

    case 't':
        if (end - json < 3 || strncmp(json, "rue", 3) != 0) {
            lastError = "Illegal value";//ֵ�Ƿ�
            return false;
        }
        json += 3;
        outValue = true;
        return true;

    case 'f':
        if (end - json < 4 || strncmp(json, "alse", 4) != 0) {
            lastError = "Illegal value";//ֵ�Ƿ�
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

// ��ʮ�������ַ�ת��Ϊ��Ӧ����ֵ�������½��
static inline bool addHexDigit(char digit, char32_t* result) {
    *result <<= 4;  // ����ǰ�������4λ��׼������µ�4λ��һ��16����λ��

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

    *result |= h;  // ��ת�������ֵ�� result ���л�����
    return true;
}

// ɨ�貢����ת�����У�������洢�� ch ��
static inline bool scanEscapeSequence(const char*& json, const char* end, char32_t* ch) {
    ++json;  // ������б��
    if (json >= end)  // ����Ƿ񳬳��ַ���ĩβ
        return false;

    char escaped = *json++;  // ��ȡ��һ���ַ�
    switch (escaped) {
    case '"':
        *ch = '"'; break;
    case '\\':
        *ch = '\\'; break;
    case '/':
        *ch = '/'; break;
    case 'b':
        *ch = 0x8; break;  // �˸��
    case 'f':
        *ch = 0xc; break;  // ��ҳ��
    case 'n':
        *ch = 0xa; break;  // ���з�
    case 'r':
        *ch = 0xd; break;  // �س���
    case 't':
        *ch = 0x9; break;  // �Ʊ��
    case 'u': {
        *ch = 0;  // ��ʼ��Ϊ 0
        if (json > end - 4)  // ����Ƿ����㹻���ַ�����ȡ4��16��������
            return false;
        // ����4λʮ���������֣����һ��UTF-16�ַ�
        for (int i = 0; i < 4; ++i) {
            if (!addHexDigit(*json, ch))  // ���ת��ʧ�ܣ����� false
                return false;
            ++json;
        }
        return true;  // �ɹ�����
    }
    default:
        // �Ǳ�׼ת���ַ���ֱ�ӽ��丳ֵ�����
        *ch = escaped;
        return true;
    }
    return true;
}

// ɨ�� UTF-8 �ַ�������ת��Ϊ Unicode ���
static inline bool scanUtf8Char(const char*& json, const char* end, char32_t* result) {
    if (json >= end)
        return false;

    unsigned char b = static_cast<unsigned char>(*json++);

    if (b < 0x80) {  // ���ֽ��ַ� (ASCII)
        *result = b;
    }
    else if (b < 0xE0) {  // ���ֽ��ַ�
        if (json >= end || (static_cast<unsigned char>(*json) & 0xC0) != 0x80)
            return false;
        *result = ((b & 0x1F) << 6) | (static_cast<unsigned char>(*json++) & 0x3F);
    }
    else if (b < 0xF0) {  // ���ֽ��ַ�
        if (json + 1 >= end ||
            (static_cast<unsigned char>(json[0]) & 0xC0) != 0x80 ||
            (static_cast<unsigned char>(json[1]) & 0xC0) != 0x80)
            return false;
        *result = ((b & 0x0F) << 12) |
            ((static_cast<unsigned char>(*json++) & 0x3F) << 6) |
            (static_cast<unsigned char>(*json++) & 0x3F);
    }
    else if (b < 0xF8) {  // ���ֽ��ַ�
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
        return false;  // �Ƿ��� UTF-8 ����
    }

    return true;
}

bool KJsonParser::parseString(std::string& outString)
{
    const char* start = json;

    bool isUtf8 = true;
    bool isAscii = true;
    std::string result;

    // ��������ת�����е� UTF-8 �ַ���
    while (json < end) {
        char32_t ch = 0;
        if (*json == '"') // ��⵽�ַ�������
            break;
        if (*json == '\\') { // ����ת������
            isAscii = false;
            isUtf8 = false;
            break;
        }

        result += *json++;
    }
    ++json; // ����������˫����

    if (json >= end) {
        lastError = "Unterminated string";//�ַ�����ʽ����ȷ
        return false;
    }

    // ���û��ת�����У�ֱ�ӷ��ؽ��
    if (isUtf8) {
        outString = result;
        return true;
    }

    // �������ת�����У����¿�ʼ����
    json = start;
    result.clear();

    while (json < end) {
        char32_t ch = 0;
        if (*json == '"') // ��⵽�ַ�������
            break;
        else if (*json == '\\') { // ����ת���ַ�
            if (!scanEscapeSequence(json, end, &ch)) {
                lastError = "Illegal escape sequence";//ת���ַ���ʽ����ȷ
                return false;
            }
        }
        else {
            if (!scanUtf8Char(json, end, &ch)) { // ������ͨ�ַ�
                lastError = "Illegal UTF-8 string";//UTF-8�ַ���ʽ����ȷ
                return false;
            }
        }

        result += static_cast<char>(ch); // �����������ַ���ӵ������
    }
    ++json; // ����������˫����

    if (json >= end) {
        lastError = "Unterminated string";//�ַ�����ʽ����ȷ
        return false;
    }

    outString = result;
    return true;
}

int KJsonParser::parseNumber(double& outNumber)
{
    const char* start = json;
    bool isInt = true;

    // ������
    if (json < end && *json == '-')
        ++json;

    // ������������
    if (json < end && *json == '0') {
        ++json;  // ������0
    }
    else {
        while (json < end && isAsciiDigit(*json))  // ����������
            ++json;
    }

    // ����С������
    if (json < end && *json == '.') {
        ++json;
        while (json < end && isAsciiDigit(*json)) {
            isInt = false;
            ++json;
        }
    }

    // ����ָ������
    if (json < end && (*json == 'e' || *json == 'E')) {
        isInt = false;
        ++json;
        if (json < end && (*json == '-' || *json == '+'))  // ����ָ������
            ++json;
        while (json < end && isAsciiDigit(*json))
            ++json;
    }

    // ת���ַ���Ϊ����
    std::string numberStr(start, json - start);
    try {
        if (isInt) {
            outNumber = std::stoll(numberStr);  // ��������
            return 1;
        }
        else {
            outNumber = std::stod(numberStr);  // ��������
            return 2;
        }
    }
    catch (const std::invalid_argument& e) {
        lastError = "Invalid number format";//��Ч���ָ�ʽ
        return false;
    }
    catch (const std::out_of_range& e) {
        lastError = "Number out of range";//���ֳ�����Χ
        return false;
    }

    return true;
}

std::shared_ptr<KJson> KJsonParser::parse() {
    // �����ֽ����ǣ�BOM��
    skipBOM();

    // ��ȡ��һ����Чtoken
    char token = nextToken();

    std::shared_ptr<KJson> data;

    // �ж��Ƿ�����������Ŀ�ʼ
    if (token == '[') {
        auto jsonArray = std::make_shared<KJsonArray>();
        if (!parseArray(*jsonArray)) {
            // �����������ʧ�ܣ���¼���󲢷��ؿն���
            return std::make_shared<KJsonArray>();
        }
        data = jsonArray; // �����������͵� KJson ����
    }
    else if (token == '{') {
        auto jsonObject = std::make_shared<KJsonObject>();
        if (!parseObject(*jsonObject)) {
            // �����������ʧ�ܣ���¼���󲢷��ؿն���
            return std::make_shared<KJsonObject>();
        }
        data = jsonObject; // ��ֵΪ������Ķ���
    }
    else {
        lastError = "Unexpected token at the beginning of JSON data";//Json������ʼ�ַ��Ƿ�
        return std::make_shared<KJsonObject>(); // ���ؿն���
    }

    // �������ܵĿհ��ַ�
    skipSpace();

    // ����Ƿ���������ļ���β��ȷ��û�ж�������
    if (json < end) {
        lastError = "Extra data found after valid JSON";//Json��Ч���ݺ��ȡ����������
        return std::make_shared<KJsonObject>(); // ���ؿն���
    }

    return data; // ���ؽ������ KJson ����
}
//KJsonParser.cpp