#pragma once
#include"KJson.h"

class JsonValue;

class KJsonObject : public KJson {
public:

    // 构造与析构函数
    KJsonObject() {};
    ~KJsonObject() {};

    // 拷贝构造函数
    KJsonObject(const KJsonObject& other) {
        std::lock_guard<std::mutex> lock(other.objectMutex);
        data = other.data; // 复制数据
    }
    //拷贝赋值函数
    KJsonObject& operator=(const KJsonObject& other) {
        if (this != &other) {
            std::lock_guard<std::mutex> lockOther(other.objectMutex);
            std::lock_guard<std::mutex> lockThis(objectMutex);
            data = other.data; // 复制数据
        }
        return *this;
    }

    // 移动构造函数
    KJsonObject(KJsonObject&& other) noexcept : data(std::move(other.data)) {}

    // 移动赋值运算符
    KJsonObject& operator=(KJsonObject&& other) noexcept {
        std::lock_guard<std::mutex> lockThis(objectMutex);
        if (this != &other) {
            data = std::move(other.data);
        }
        return *this;
    }

    // 将KJsonObject对象序列化为JSON字符串并写入文件
    bool serializeToJsonFile(const std::string& filePath) const;

    // 将KJsonObject对象序列化为XML并写入文件
    bool serializeToXMLFile(const std::string& filePath) const;

    // 动态添加键值对
    void add(const std::string& key, const std::string& value);

    //查询函数
    bool query(std::vector<std::string>& keys, size_t startIndex, const KJsonObject& currentJson) const;
    //用于处理嵌套逻辑
    void interactiveQuery();

    // 删除指定键
    void remove(const std::string& key);

    // 处理嵌套对象和数组
    JsonValue& operator[](const std::string& key);

    void lock() {
        objectMutex.lock();
    }// 添加锁定方法
    void unlock() {
        objectMutex.unlock();
    }// 添加解锁方法

private:

    std::unordered_map<std::string, JsonValue> data;
    mutable std::mutex objectMutex; // 互斥锁
    // 内部辅助函数
    static bool writeFile(const std::string& filePath, const std::string& content);
    std::string serializeValueToJson(int indentLevel = 0) const;
    // 辅助函数：递归序列化每个键值对为XML格式
    static void serializeValueToXML(const std::string& key, const JsonValue& value, std::ostringstream& oss, int indent);
    KJsonObject parse(const std::string& jsonString);

    friend class KJsonArray;
    friend class KJsonParser;
    friend class JsonValue;
};