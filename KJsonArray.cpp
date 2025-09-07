#include"Headerfiles.h"
#include"auxiliary.h"
//KJsonArray.cpp
bool KJsonArray::query(std::vector<std::string>& keys, size_t startIndex, const KJsonArray& currentJson) const {
    // ��ȡ��ǰ�ļ�
    const std::string& key = keys[startIndex];
    int index = std::stoi(key); // ��������Ϊ����

    if (index <= 0 || index > currentJson.size()) {
        std::cout << "����������Χ: " << index << std::endl;
        return false;
    }

    // �ݹ鴦�������е�Ԫ��
    const JsonValue& value = *currentJson.elements[index - 1]; // ������1��ʼ

    if (const auto* jsonPtr = std::get_if<KJsonObject>(&value.value_)) {
        std::cout << "����Ԫ��ΪJson���󡣿��ü�: ";
        for (const auto& [key, _] : jsonPtr->data) {
            std::cout << key << " ";
        }
        std::cout << std::endl;

        // ��ʾ�û�������һ����
        std::string nextKey;
        std::cout << "��������һ����: ";
        std::cin >> nextKey;

        keys.push_back(nextKey);  // ���û�����ļ���ӵ� keys ��
        // �����ݹ��ѯ
        return jsonPtr->query(keys, startIndex + 1, *jsonPtr);
    }
    else if (const auto* arrayPtr = std::get_if<KJsonArray>(&value.value_)) {
        // �����ǰԪ����KJsonArray�����еݹ��ѯ
        if (startIndex == keys.size() - 1) {
            std::cout << "��ǰ��ѯԪ�������飬����������Χ: 1 - " << arrayPtr->size() << std::endl;
            // ��ʾ�û�������һ����
            std::string nextKey;
            std::cout << "��������������: ";
            std::cin >> nextKey;

            try {
                int index = std::stoi(nextKey);
                if (index > 0 && index <= arrayPtr->size()) {
                    keys.push_back(std::to_string(index)); // ��������ӵ� keys ��
                    return arrayPtr->query(keys, startIndex + 1, *arrayPtr);  // ������ѯ
                }
                else {
                    std::cout << "����������Χ: " << index << std::endl;
                    return false;
                }
            }
            catch (const std::invalid_argument&) {
                std::cout << "��Ч����������: " << nextKey << std::endl;
                return false;
            }
        }
        else return arrayPtr->query(keys, startIndex + 1, *arrayPtr);
    }
    else {
        // ������
        std::cout << "���: ";
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

    // ��ȡ��ʼ�ļ�·��
    std::cout << "������������������������Χ: 1 - " << this->elements.size() << std::endl;
    std::getline(std::cin, input);
    std::istringstream iss(input);
    std::string key;
    while (iss >> key) {
        keys.push_back(key);
    }

    // �ӵ�ǰ����ʼ��ѯ
    if (!query(keys, 0, *this)) {
        std::cout << "��ѯʧ�ܡ�" << std::endl;
    }
}


bool KJsonArray::serializeToJsonFile(const std::string& filePath) const {
    std::string content = serializeValueToJson();
    if (writeFile(filePath, content))
        return true;
    else 
        return false;
}

// ʵ��XML���л���д���ļ�
bool KJsonArray::serializeToXMLFile(const std::string& filePath) const {
    std::ofstream file(filePath);
    std::ostringstream oss;

    // �����µ����л�����
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
    // ��ӿ�ʼ��ǩ
    oss << std::string(indentLevel, ' ') << "<Array>\n";

    // ���������е�ÿ��Ԫ��
    for (size_t i = 0; i < jsonArray.elements.size(); ++i) {
        const JsonValue& value = *jsonArray.elements[i];

        oss << std::string(indentLevel + 2, ' ') << "<Element index=\"" << (i + 1) << "\">\n";

        // ����Ԫ���������л�
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
            serializeValueToXML(oss, *jsonArrayPtr, indentLevel + 4); // �ݹ����
        }
        else if (auto jsonObjectPtr = std::get_if<KJsonObject>(&value.value_)) {
            for (const auto& [key, value] : jsonObjectPtr->data) {
                jsonObjectPtr->serializeValueToXML(key, value, oss, indentLevel + 4);
            }
            oss << std::string(indentLevel, ' ');
        }

        oss << std::string(indentLevel + 2, ' ') << "</Element>\n";
    }

    // ��ӽ�����ǩ
    oss << std::string(indentLevel, ' ') << "</Array>\n";
}

std::string KJsonArray::serializeValueToJson(int indentLevel) const {
    bool first = true;
    std::string indent(indentLevel + 4, ' '); // ��ǰ�㼶������
    std::string jsonString = std::string(indentLevel, ' ') + "[\n";

    for (const auto& element : elements) {
        if (!first) {
            jsonString += ",\n";
        }
        first = false;

        // ����ͬ���͵�ֵ
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
            jsonString += jsonPtr->serializeValueToJson(indentLevel + 4); // ����KJsonObject�����л�
        }
        else if (auto arrayPtr = std::get_if<KJsonArray>(&element->value_)) {
            jsonString += arrayPtr->serializeValueToJson(indentLevel + 4); // ����KJsonArray�����л�
        }
        // ���������չ�����������͵Ĵ���
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