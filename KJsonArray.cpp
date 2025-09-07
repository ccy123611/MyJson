#include"Headerfiles.h"
#include"auxiliary.h"
//KJsonArray.cpp
bool KJsonArray::query(std::vector<std::string>& keys, size_t startIndex, const KJsonArray& currentJson) const {
    // 获取当前的键
    const std::string& key = keys[startIndex];
    int index = std::stoi(key); // 将键解析为索引

    if (index <= 0 || index > currentJson.size()) {
        std::cout << "索引超出范围: " << index << std::endl;
        return false;
    }

    // 递归处理数组中的元素
    const JsonValue& value = *currentJson.elements[index - 1]; // 索引从1开始

    if (const auto* jsonPtr = std::get_if<KJsonObject>(&value.value_)) {
        std::cout << "数组元素为Json对象。可用键: ";
        for (const auto& [key, _] : jsonPtr->data) {
            std::cout << key << " ";
        }
        std::cout << std::endl;

        // 提示用户输入下一级键
        std::string nextKey;
        std::cout << "请输入下一级键: ";
        std::cin >> nextKey;

        keys.push_back(nextKey);  // 将用户输入的键添加到 keys 中
        // 继续递归查询
        return jsonPtr->query(keys, startIndex + 1, *jsonPtr);
    }
    else if (const auto* arrayPtr = std::get_if<KJsonArray>(&value.value_)) {
        // 如果当前元素是KJsonArray，进行递归查询
        if (startIndex == keys.size() - 1) {
            std::cout << "当前查询元素是数组，可用索引范围: 1 - " << arrayPtr->size() << std::endl;
            // 提示用户输入下一级键
            std::string nextKey;
            std::cout << "请输入数组索引: ";
            std::cin >> nextKey;

            try {
                int index = std::stoi(nextKey);
                if (index > 0 && index <= arrayPtr->size()) {
                    keys.push_back(std::to_string(index)); // 将索引添加到 keys 中
                    return arrayPtr->query(keys, startIndex + 1, *arrayPtr);  // 继续查询
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
        else return arrayPtr->query(keys, startIndex + 1, *arrayPtr);
    }
    else {
        // 输出结果
        std::cout << "结果: ";
        if (auto intPtr = std::get_if<int>(&value.value_)) {
            std::cout << *intPtr << std::endl;
        }
        else if (auto floatPtr = std::get_if<double>(&value.value_)) {
            std::cout << *floatPtr << std::endl;
        }
        else if (auto stringPtr = std::get_if<std::string>(&value.value_)) {
            std::cout << *stringPtr << std::endl;
        }
        else if (auto boolPtr = std::get_if<bool>(&value.value_)) {
            std::cout << (*boolPtr ? "true" : "false") << std::endl;
        }
        else if (std::get_if<std::nullptr_t>(&value.value_)) {
            std::cout << "null" << std::endl;
        }
        return true;
    }
}

void KJsonArray::interactiveQuery() {
    std::vector<std::string> keys;
    std::string input;
    getchar();

    // 获取初始的键路径
    std::cout << "请输入数组索引可用索引范围: 1 - " << this->elements.size() << std::endl;
    std::getline(std::cin, input);
    std::istringstream iss(input);
    std::string key;
    while (iss >> key) {
        keys.push_back(key);
    }

    // 从当前对象开始查询
    if (!query(keys, 0, *this)) {
        std::cout << "查询失败。" << std::endl;
    }
}


bool KJsonArray::serializeToJsonFile(const std::string& filePath) const {
    std::string content = serializeValueToJson();
    if (writeFile(filePath, content))
        return true;
    else 
        return false;
}

// 实现XML序列化并写入文件
bool KJsonArray::serializeToXMLFile(const std::string& filePath) const {
    std::ofstream file(filePath);
    std::ostringstream oss;

    // 调用新的序列化函数
    serializeValueToXML(oss, *this, 0);

    if (file.is_open()) {
        file << oss.str();
        file.close();
        return true;
    }
    else {
        return false;
    }
}



void KJsonArray::serializeValueToXML(std::ostringstream& oss, const KJsonArray& jsonArray, int indentLevel) {
    // 添加开始标签
    oss << std::string(indentLevel, ' ') << "<Array>\n";

    // 遍历数组中的每个元素
    for (size_t i = 0; i < jsonArray.elements.size(); ++i) {
        const JsonValue& value = *jsonArray.elements[i];

        oss << std::string(indentLevel + 2, ' ') << "<Element index=\"" << (i + 1) << "\">\n";

        // 根据元素类型序列化
        if (auto intPtr = std::get_if<int>(&value.value_)) {
            oss << std::string(indentLevel + 4, ' ') << *intPtr;
        }
        else if (auto floatPtr = std::get_if<double>(&value.value_)) {
            oss << std::string(indentLevel + 4, ' ') << *floatPtr;
        }
        else if (auto stringPtr = std::get_if<std::string>(&value.value_)) {
            oss << std::string(indentLevel + 4, ' ');
            std::string escapedString = escapeXml(*stringPtr);
            oss << escapedString;
        }
        else if (auto boolPtr = std::get_if<bool>(&value.value_)) {
            oss << std::string(indentLevel + 4, ' ') << (*boolPtr ? "true" : "false");
        }
        else if (std::get_if<std::nullptr_t>(&value.value_)) {
            oss << std::string(indentLevel + 4, ' ') << "null";
        }
        else if (auto jsonArrayPtr = std::get_if<KJsonArray>(&value.value_)) {
            serializeValueToXML(oss, *jsonArrayPtr, indentLevel + 4); // 递归调用
        }
        else if (auto jsonObjectPtr = std::get_if<KJsonObject>(&value.value_)) {
            for (const auto& [key, value] : jsonObjectPtr->data) {
                jsonObjectPtr->serializeValueToXML(key, value, oss, indentLevel + 4);
            }
            oss << std::string(indentLevel, ' ');
        }

        oss << std::string(indentLevel + 2, ' ') << "</Element>\n";
    }

    // 添加结束标签
    oss << std::string(indentLevel, ' ') << "</Array>\n";
}

std::string KJsonArray::serializeValueToJson(int indentLevel) const {
    bool first = true;
    std::string indent(indentLevel + 4, ' '); // 当前层级的缩进
    std::string jsonString = std::string(indentLevel, ' ') + "[\n";

    for (const auto& element : elements) {
        if (!first) {
            jsonString += ",\n";
        }
        first = false;

        // 处理不同类型的值
        if (auto strPtr = std::get_if<std::string>(&element->value_)) {
            jsonString += indent + "\"" + *strPtr + "\"";
        }
        else if (auto intPtr = std::get_if<int>(&element->value_)) {
            jsonString += indent + std::to_string(*intPtr);
        }
        else if (auto doublePtr = std::get_if<double>(&element->value_)) {
            jsonString += indent + std::to_string(*doublePtr);
        }
        else if (auto boolPtr = std::get_if<bool>(&element->value_)) {
            jsonString += indent + (*boolPtr ? "true" : "false");
        }
        else if (std::get_if<std::nullptr_t>(&element->value_)) {
            jsonString += indent + "null";
        }
        else if (auto jsonPtr = std::get_if<KJsonObject>(&element->value_)) {
            jsonString += jsonPtr->serializeValueToJson(indentLevel + 4); // 调用KJsonObject的序列化
        }
        else if (auto arrayPtr = std::get_if<KJsonArray>(&element->value_)) {
            jsonString += arrayPtr->serializeValueToJson(indentLevel + 4); // 调用KJsonArray的序列化
        }
        // 这里可以扩展其他数据类型的处理
    }

    jsonString += "\n" + std::string(indentLevel, ' ') + "]";
    return jsonString;
}



bool KJsonArray::writeFile(const std::string& filePath, const std::string& content) {
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

//KJsonArray.cpp end