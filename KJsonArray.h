#pragma once
#include"KJson.h"

class JsonValue;

class KJsonArray : public KJson {
public:

    // 构造函数
    KJsonArray() {};
    ~KJsonArray() {};


    // 拷贝构造函数
    KJsonArray(const KJsonArray& other) {
        std::lock_guard<std::mutex> lock(other.arrayMutex);
        elements = other.elements; // 复制数据
    }

    // 移动构造函数
    KJsonArray(KJsonArray&& other) noexcept : elements(std::move(other.elements)) {}

    // 拷贝赋值运算符
    KJsonArray& operator=(const KJsonArray& other) {
        if (this != &other) {
            std::lock_guard<std::mutex> lockOther(other.arrayMutex);
            std::lock_guard<std::mutex> lockThis(arrayMutex);
            elements = other.elements; // 复制数据
        }
        return *this;
    }

    // 移动赋值运算符
    KJsonArray& operator=(KJsonArray&& other) noexcept {
        if (this != &other) {
            std::lock_guard<std::mutex> lockThis(arrayMutex);
            elements = std::move(other.elements); // 移动数据
        }
        return *this;
    }

    // 添加元素
    void add(std::shared_ptr<JsonValue> value) {
        std::lock_guard<std::mutex> lock(arrayMutex);
        elements.push_back(value);
    }

    // 清空元素
    void clear() {
        std::lock_guard<std::mutex> lock(arrayMutex);
        elements.clear();
    }

    // 移除元素
    void remove(int index) {
        std::lock_guard<std::mutex> lock(arrayMutex);
        if (index < elements.size() && index >= 0) {
            elements.erase(elements.begin() + index);
        }
        else {
            std::cout << "索引不合法" << std::endl;
        }
    }

    //查询函数
    bool query(std::vector<std::string>& keys, size_t startIndex, const KJsonArray& currentJson) const;
    //用于处理嵌套逻辑
    void interactiveQuery();

    // 获取元素数量
    size_t size() const {
        return elements.size();
    }


    // 将KJsonArray对象序列化为JSON字符串并写入文件
    bool serializeToJsonFile(const std::string& filePath) const;//调用serialize函数

    // 将KJsonArray对象序列化为XML并写入文件
    bool serializeToXMLFile(const std::string& filePath) const;

    using iterator = std::vector<std::shared_ptr<JsonValue>>::iterator;
    using const_iterator = std::vector<std::shared_ptr<JsonValue>>::const_iterator;

    iterator begin() { return elements.begin(); }
    iterator end() { return elements.end(); }
    const_iterator begin() const { return elements.begin(); }
    const_iterator end() const { return elements.end(); }

    void lock() {
        arrayMutex.lock();
    }// 添加锁定方法
    void unlock() {
        arrayMutex.unlock();
    }// 添加解锁方法

private:

    static bool writeFile(const std::string& filePath, const std::string& content);
    std::string serializeValueToJson(int indentLevel = 0) const;
    static void serializeValueToXML(std::ostringstream& oss, const KJsonArray& jsonArray, int indentLevel);

    std::vector<std::shared_ptr<JsonValue>> elements;; // 存储元素的 vector
    mutable std::mutex arrayMutex; // 互斥锁

    friend class KJsonObject;
    friend class JsonValue;
};
