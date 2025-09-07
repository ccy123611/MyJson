#include"Headerfiles.h"
#include"auxiliary.h"

//KJsonObject.cpp


JsonValue& KJsonObject::operator[](const std::string& key) {
    lock();
    // 查找键是否存在
    auto it = data.find(key);
    if (it != data.end()) {
        // 如果找到，返回对应的值
        return it->second;
    }

    // 如果没有找到，创建一个新的键值对并返回该值的引用
    data[key] = JsonValue(); // 默认构造一个 JsonValue
    unlock();
    return data[key];

}

KJsonObject KJsonObject::parse(const std::string& jsonString) {
    lock();
    KJsonParser parser(jsonString.c_str(), jsonString.length());
    std::shared_ptr<KJson> json = parser.parse();
    if (auto objectPtr = std::dynamic_pointer_cast<KJsonObject>(json)) {
        return *objectPtr;// 处理 KJsonObject
    }
    unlock();
    return KJsonObject();
}

void KJsonObject::add(const std::string& key, const std::string& value) {
    lock();
    std::string pending_data = "";
    pending_data += "{\"" + key + "\": " + value + "}"; // 如果 value 是字符串
    KJsonObject tmpjsonObject = parse(pending_data);
    data[key] = tmpjsonObject.data[key]; // 自动类型推导
    unlock();
}

void KJsonObject::remove(const std::string& key) {
    lock();
    data.erase(key);
    unlock();
}

// 辅助查询函数，处理逐级查询逻辑
bool KJsonObject::query(std::vector<std::string>& keys, size_t startIndex, const KJsonObject& currentJson) const {
    for (size_t i = startIndex; i < keys.size(); ++i) {
        const std::string& key = keys[i];
        auto it = currentJson.data.find(key);

        if (it == currentJson.data.end()) {
            std::cout << "未找到键: " << key << std::endl;
            return false;
        }

        // 检查类型并输出或继续
        if (auto intPtr = std::get_if<int>(&it->second.value_)) {
            std::cout << "结果: " << *intPtr << std::endl;
            return true;
        }
        else if (auto floatPtr = std::get_if<double>(&it->second.value_)) {
            std::cout << "结果: " << *floatPtr << std::endl;
            return true;
        }
        else if (auto stringPtr = std::get_if<std::string>(&it->second.value_)) {
            std::cout << "结果: " << *stringPtr << std::endl;
            return true;
        }
        else if (auto boolPtr = std::get_if<bool>(&it->second.value_)) {
            std::cout << "结果: " << (*boolPtr ? "true" : "false") << std::endl;
            return true;
        }
        else if (auto nullPtr = std::get_if<std::nullptr_t>(&it->second.value_)) {
            std::cout << "结果: null" << std::endl;
            return true;
        }
        else if (auto jsonPtr = std::get_if<KJsonObject>(&it->second.value_)) {
            // 递归调用
            if (query(keys, i + 1, *jsonPtr)) {
                return true;
            }
            else if (i == keys.size() - 1) {
                std::cout << "未完全指定键路径。可用键: ";
                for (const auto& [key, _] : jsonPtr->data) {
                    std::cout << key << " ";
                }
                std::cout << std::endl;

                // 提示用户输入下一级键
                std::string nextKey;
                std::cout << "请输入下一级键: ";
                std::cin >> nextKey;

                keys.push_back(nextKey);  // 将用户输入的键添加到 keys 中
                return query(keys, i + 1, *jsonPtr);  // 继续查询
            }
        }
        else if (auto arrayPtr = std::get_if<KJsonArray>(&it->second.value_)) {
            if (i == keys.size() - 1) {
                std::cout << "未完全指定键路径。可用索引范围: 1 - " << arrayPtr->size() << std::endl;
                // 提示用户输入下一级键
                std::string nextKey;
                std::cout << "请输入数组索引: ";
                std::cin >> nextKey;

                // 尝试将输入的索引转换为整数
                try {
                    int index = std::stoi(nextKey);
                    if (index > 0 && index <= arrayPtr->size()) {
                        keys.push_back(std::to_string(index)); // 将索引添加到 keys 中
                        return arrayPtr->query(keys, i + 1, *arrayPtr);  // 继续查询
                    }
                    else {
                        std::cout << "索引超出范围: " << index << std::endl;
                        return false;
                    }
                }
                catch (const std::invalid_argument&) {
                    std::cout << "无效的索引输入: " << nextKey << std::endl;
                    return false;
                }
            }
            else {
                int index = std::stoi(keys[++i]);
                if (index > 0 && index <= arrayPtr->size()) {
                    // 递归调用
                    return arrayPtr->query(keys, i, *arrayPtr);
                }
                else {
                    std::cout << "索引超出范围: " << index << std::endl;
                    return false;
                }
            }
        }
        else {
            std::cout << "未知数据类型。" << std::endl;
            return false;
        }
    }
    return false;
}

// 交互式查询函数
void KJsonObject::interactiveQuery() {
    std::vector<std::string> keys;
    std::string input;
    getchar();
    // 获取初始的键路径
    std::cout << "请输入查询的内容(空格分隔每层键): ";
    std::getline(std::cin, input);
    std::istringstream iss(input);
    std::string key;
    while (iss >> key) {
        keys.push_back(key);
    }

    // 从当前对象开始查询
    query(keys, 0, *this);
}


bool KJsonObject::serializeToJsonFile(const std::string& filePath) const {
    std::string content = serializeValueToJson();
    if (writeFile(filePath, content))
        return true;
    else 
        return false;
}

// 实现XML序列化并写入文件
bool KJsonObject::serializeToXMLFile(const std::string& filePath) const {
    std::ofstream file(filePath);
    std::ostringstream oss;
    for (const auto& [key, value] : data) {
        serializeValueToXML(key, value, oss, 0);
    }
    if (file.is_open()) {
        file << oss.str();
        file.close();
        return true;
    }
    else
        return false;
}

bool KJsonObject::writeFile(const std::string& filePath, const std::string& content) {
    std::ofstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filePath);
        return false;
    }
    file.put(0xEF);
    file.put(0xBB);
    file.put(0xBF);

    file << content;
    return true;
}

std::string KJsonObject::serializeValueToJson(int indentLevel) const {
    std::string jsonString = std::string(indentLevel, ' ') + "{\n";
    bool first = true;
    std::string indent(indentLevel + 4, ' '); // 当前层级的缩进

    for (const auto& [key, value] : data) {
        if (!first) {
            jsonString += ",\n";
        }
        first = false;

        // 添加键
        jsonString += indent + "\"" + key + "\": ";

        // 处理不同类型的值
        if (auto strPtr = std::get_if<std::string>(&value.value_)) {
            jsonString += "\"" + *strPtr + "\"";
        }
        else if (auto intPtr = std::get_if<int>(&value.value_)) {
            jsonString += std::to_string(*intPtr);
        }
        else if (auto doublePtr = std::get_if<double>(&value.value_)) {
            jsonString += std::to_string(*doublePtr);
        }
        else if (auto boolPtr = std::get_if<bool>(&value.value_)) {
            jsonString += *boolPtr ? "true" : "false";
        }
        else if (std::get_if<std::nullptr_t>(&value.value_)) {
            jsonString += "null";
        }
        else if (auto vectorPtr = std::get_if<KJsonArray>(&value.value_)) {
            jsonString += "[\n";
            std::string itemSeparator;
            for (const auto& item : *vectorPtr) {
                jsonString += itemSeparator + indent + "    " + item->serialize(indentLevel + 4); // 增加缩进
                itemSeparator = ",\n"; // 设置分隔符
            }
            jsonString += "\n" + indent + "]";
        }
        else if (auto jsonPtr = std::get_if<KJsonObject>(&value.value_)) {
            jsonString += jsonPtr->serializeValueToJson(indentLevel + 4); // 增加缩进
        }
    }

    jsonString += "\n" + std::string(indentLevel, ' ') + "}";
    return jsonString;
}


// 内部辅助函数：序列化键值对为XML格式
void KJsonObject::serializeValueToXML(const std::string& key, const JsonValue& value, std::ostringstream& oss, int indent) {
    std::string indentation(indent, ' ');

    oss << indentation << "<" << key << ">";

    if (auto intPtr = std::get_if<int>(&value.value_)) {
        oss << *intPtr;
    }
    else if (auto doublePtr = std::get_if<double>(&value.value_)) {
        oss << *doublePtr;
    }
    else if (auto stringPtr = std::get_if<std::string>(&value.value_)) {
        std::string escapedString = escapeXml(*stringPtr);
        oss << escapedString;
    }
    else if (auto boolPtr = std::get_if<bool>(&value.value_)) {
        oss << (*boolPtr ? "true" : "false");
    }
    else if (std::get_if<std::nullptr_t>(&value.value_)) {
        oss << "null";
    }
    else if (auto jsonPtr = std::get_if<KJsonObject>(&value.value_)) {
        oss << "\n";
        for (const auto& [subKey, subValue] : jsonPtr->data) {
            serializeValueToXML(subKey, subValue, oss, indent + 4);
        }
        oss << indentation;
    }
    else if (auto arrayPtr = std::get_if<KJsonArray>(&value.value_)) {
        oss << "\n";
        // 直接调用 serializeValueToXML 处理每个项
        arrayPtr->serializeValueToXML(oss, *arrayPtr, indent + 4); // 使用 "value" 作为项的标签
        oss << indentation;
    }
    oss << "</" << key << ">\n";
}

//KJsonObject.cpp end