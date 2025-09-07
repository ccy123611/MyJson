#pragma once
#include"KJson.h"
#include"KJsonArray.h"
#include"KJsonObject.h"

class KJsonParser {
public:
    // 构造函数，接收 JSON 字符串和长度
    KJsonParser(const char* json, int length);

    // 解析主函数，返回解析后的 KJson 对象
    std::shared_ptr<KJson> parse();

private:
    // 跳过字节序标记
    void skipBOM();
    // 跳过空白字符
    bool skipSpace();
    // 获取下一个有效 token
    char nextToken();

    // 解析对象
    bool parseObject(KJsonObject& jsonObject);
    // 解析数组
    bool parseArray(KJsonArray& jsonArray);
    // 解析键值对成员
    bool parseMember(KJsonObject& jsonObject);
    // 解析字符串
    bool parseString(std::string& outString);
    // 解析值（支持各种数据类型）
    bool parseValue(JsonValue& outValue);
    // 解析数字
    int parseNumber(double& outNumber);

    const char* head;
    const char* json;
    const char* end;

    int nestingLevel;
    std::string lastError; // 用于存储错误信息（暂时不实现）

    friend class KJson;
    friend class KJsonArray;
    friend class JsonValue;
};
