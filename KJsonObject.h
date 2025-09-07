#pragma once
#include"KJson.h"

class JsonValue;

class KJsonObject : public KJson {
public:

    // ��������������
    KJsonObject() {};
    ~KJsonObject() {};

    // �������캯��
    KJsonObject(const KJsonObject& other) {
        std::lock_guard<std::mutex> lock(other.objectMutex);
        data = other.data; // ��������
    }
    //������ֵ����
    KJsonObject& operator=(const KJsonObject& other) {
        if (this != &other) {
            std::lock_guard<std::mutex> lockOther(other.objectMutex);
            std::lock_guard<std::mutex> lockThis(objectMutex);
            data = other.data; // ��������
        }
        return *this;
    }

    // �ƶ����캯��
    KJsonObject(KJsonObject&& other) noexcept : data(std::move(other.data)) {}

    // �ƶ���ֵ�����
    KJsonObject& operator=(KJsonObject&& other) noexcept {
        std::lock_guard<std::mutex> lockThis(objectMutex);
        if (this != &other) {
            data = std::move(other.data);
        }
        return *this;
    }

    // ��KJsonObject�������л�ΪJSON�ַ�����д���ļ�
    bool serializeToJsonFile(const std::string& filePath) const;

    // ��KJsonObject�������л�ΪXML��д���ļ�
    bool serializeToXMLFile(const std::string& filePath) const;

    // ��̬��Ӽ�ֵ��
    void add(const std::string& key, const std::string& value);

    //��ѯ����
    bool query(std::vector<std::string>& keys, size_t startIndex, const KJsonObject& currentJson) const;
    //���ڴ���Ƕ���߼�
    void interactiveQuery();

    // ɾ��ָ����
    void remove(const std::string& key);

    // ����Ƕ�׶��������
    JsonValue& operator[](const std::string& key);

    void lock() {
        objectMutex.lock();
    }// �����������
    void unlock() {
        objectMutex.unlock();
    }// ��ӽ�������

private:

    std::unordered_map<std::string, JsonValue> data;
    mutable std::mutex objectMutex; // ������
    // �ڲ���������
    static bool writeFile(const std::string& filePath, const std::string& content);
    std::string serializeValueToJson(int indentLevel = 0) const;
    // �����������ݹ����л�ÿ����ֵ��ΪXML��ʽ
    static void serializeValueToXML(const std::string& key, const JsonValue& value, std::ostringstream& oss, int indent);
    KJsonObject parse(const std::string& jsonString);

    friend class KJsonArray;
    friend class KJsonParser;
    friend class JsonValue;
};