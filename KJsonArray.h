#pragma once
#include"KJson.h"

class JsonValue;

class KJsonArray : public KJson {
public:

    // ���캯��
    KJsonArray() {};
    ~KJsonArray() {};


    // �������캯��
    KJsonArray(const KJsonArray& other) {
        std::lock_guard<std::mutex> lock(other.arrayMutex);
        elements = other.elements; // ��������
    }

    // �ƶ����캯��
    KJsonArray(KJsonArray&& other) noexcept : elements(std::move(other.elements)) {}

    // ������ֵ�����
    KJsonArray& operator=(const KJsonArray& other) {
        if (this != &other) {
            std::lock_guard<std::mutex> lockOther(other.arrayMutex);
            std::lock_guard<std::mutex> lockThis(arrayMutex);
            elements = other.elements; // ��������
        }
        return *this;
    }

    // �ƶ���ֵ�����
    KJsonArray& operator=(KJsonArray&& other) noexcept {
        if (this != &other) {
            std::lock_guard<std::mutex> lockThis(arrayMutex);
            elements = std::move(other.elements); // �ƶ�����
        }
        return *this;
    }

    // ���Ԫ��
    void add(std::shared_ptr<JsonValue> value) {
        std::lock_guard<std::mutex> lock(arrayMutex);
        elements.push_back(value);
    }

    // ���Ԫ��
    void clear() {
        std::lock_guard<std::mutex> lock(arrayMutex);
        elements.clear();
    }

    // �Ƴ�Ԫ��
    void remove(int index) {
        std::lock_guard<std::mutex> lock(arrayMutex);
        if (index < elements.size() && index >= 0) {
            elements.erase(elements.begin() + index);
        }
        else {
            std::cout << "�������Ϸ�" << std::endl;
        }
    }

    //��ѯ����
    bool query(std::vector<std::string>& keys, size_t startIndex, const KJsonArray& currentJson) const;
    //���ڴ���Ƕ���߼�
    void interactiveQuery();

    // ��ȡԪ������
    size_t size() const {
        return elements.size();
    }


    // ��KJsonArray�������л�ΪJSON�ַ�����д���ļ�
    bool serializeToJsonFile(const std::string& filePath) const;//����serialize����

    // ��KJsonArray�������л�ΪXML��д���ļ�
    bool serializeToXMLFile(const std::string& filePath) const;

    using iterator = std::vector<std::shared_ptr<JsonValue>>::iterator;
    using const_iterator = std::vector<std::shared_ptr<JsonValue>>::const_iterator;

    iterator begin() { return elements.begin(); }
    iterator end() { return elements.end(); }
    const_iterator begin() const { return elements.begin(); }
    const_iterator end() const { return elements.end(); }

    void lock() {
        arrayMutex.lock();
    }// �����������
    void unlock() {
        arrayMutex.unlock();
    }// ��ӽ�������

private:

    static bool writeFile(const std::string& filePath, const std::string& content);
    std::string serializeValueToJson(int indentLevel = 0) const;
    static void serializeValueToXML(std::ostringstream& oss, const KJsonArray& jsonArray, int indentLevel);

    std::vector<std::shared_ptr<JsonValue>> elements;; // �洢Ԫ�ص� vector
    mutable std::mutex arrayMutex; // ������

    friend class KJsonObject;
    friend class JsonValue;
};
