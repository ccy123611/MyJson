#pragma once
#include"KJson.h"
#include"KJsonArray.h"
#include"KJsonObject.h"

class KJsonParser {
public:
    // ���캯�������� JSON �ַ����ͳ���
    KJsonParser(const char* json, int length);

    // ���������������ؽ������ KJson ����
    std::shared_ptr<KJson> parse();

private:
    // �����ֽ�����
    void skipBOM();
    // �����հ��ַ�
    bool skipSpace();
    // ��ȡ��һ����Ч token
    char nextToken();

    // ��������
    bool parseObject(KJsonObject& jsonObject);
    // ��������
    bool parseArray(KJsonArray& jsonArray);
    // ������ֵ�Գ�Ա
    bool parseMember(KJsonObject& jsonObject);
    // �����ַ���
    bool parseString(std::string& outString);
    // ����ֵ��֧�ָ����������ͣ�
    bool parseValue(JsonValue& outValue);
    // ��������
    int parseNumber(double& outNumber);

    const char* head;
    const char* json;
    const char* end;

    int nestingLevel;
    std::string lastError; // ���ڴ洢������Ϣ����ʱ��ʵ�֣�

    friend class KJson;
    friend class KJsonArray;
    friend class JsonValue;
};
