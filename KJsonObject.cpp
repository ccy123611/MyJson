#include"Headerfiles.h"
#include"auxiliary.h"

//KJsonObject.cpp


JsonValue& KJsonObject::operator[](const std::string& key) {
    lock();
    // ���Ҽ��Ƿ����
    auto it = data.find(key);
    if (it != data.end()) {
        // ����ҵ������ض�Ӧ��ֵ
        return it->second;
    }

    // ���û���ҵ�������һ���µļ�ֵ�Բ����ظ�ֵ������
    data[key] = JsonValue(); // Ĭ�Ϲ���һ�� JsonValue
    unlock();
    return data[key];

}

KJsonObject KJsonObject::parse(const std::string& jsonString) {
    lock();
    KJsonParser parser(jsonString.c_str(), jsonString.length());
    std::shared_ptr<KJson> json = parser.parse();
    if (auto objectPtr = std::dynamic_pointer_cast<KJsonObject>(json)) {
        return *objectPtr;// ���� KJsonObject
    }
    unlock();
    return KJsonObject();
}

void KJsonObject::add(const std::string& key, const std::string& value) {
    lock();
    std::string pending_data = "";
    pending_data += "{\"" + key + "\": " + value + "}"; // ��� value ���ַ���
    KJsonObject tmpjsonObject = parse(pending_data);
    data[key] = tmpjsonObject.data[key]; // �Զ������Ƶ�
    unlock();
}

void KJsonObject::remove(const std::string& key) {
    lock();
    data.erase(key);
    unlock();
}

// ������ѯ�����������𼶲�ѯ�߼�
bool KJsonObject::query(std::vector<std::string>& keys, size_t startIndex, const KJsonObject& currentJson) const {
    for (size_t i = startIndex; i < keys.size(); ++i) {
        const std::string& key = keys[i];
        auto it = currentJson.data.find(key);

        if (it == currentJson.data.end()) {
            std::cout << "δ�ҵ���: " << key << std::endl;
            return false;
        }

        // ������Ͳ���������
        if (auto intPtr = std::get_if<int>(&it->second.value_)) {
            std::cout << "���: " << *intPtr << std::endl;
            return true;
        }
        else if (auto floatPtr = std::get_if<double>(&it->second.value_)) {
            std::cout << "���: " << *floatPtr << std::endl;
            return true;
        }
        else if (auto stringPtr = std::get_if<std::string>(&it->second.value_)) {
            std::cout << "���: " << *stringPtr << std::endl;
            return true;
        }
        else if (auto boolPtr = std::get_if<bool>(&it->second.value_)) {
            std::cout << "���: " << (*boolPtr ? "true" : "false") << std::endl;
            return true;
        }
        else if (auto nullPtr = std::get_if<std::nullptr_t>(&it->second.value_)) {
            std::cout << "���: null" << std::endl;
            return true;
        }
        else if (auto jsonPtr = std::get_if<KJsonObject>(&it->second.value_)) {
            // �ݹ����
            if (query(keys, i + 1, *jsonPtr)) {
                return true;
            }
            else if (i == keys.size() - 1) {
                std::cout << "δ��ȫָ����·�������ü�: ";
                for (const auto& [key, _] : jsonPtr->data) {
                    std::cout << key << " ";
                }
                std::cout << std::endl;

                // ��ʾ�û�������һ����
                std::string nextKey;
                std::cout << "��������һ����: ";
                std::cin >> nextKey;

                keys.push_back(nextKey);  // ���û�����ļ���ӵ� keys ��
                return query(keys, i + 1, *jsonPtr);  // ������ѯ
            }
        }
        else if (auto arrayPtr = std::get_if<KJsonArray>(&it->second.value_)) {
            if (i == keys.size() - 1) {
                std::cout << "δ��ȫָ����·��������������Χ: 1 - " << arrayPtr->size() << std::endl;
                // ��ʾ�û�������һ����
                std::string nextKey;
                std::cout << "��������������: ";
                std::cin >> nextKey;

                // ���Խ����������ת��Ϊ����
                try {
                    int index = std::stoi(nextKey);
                    if (index > 0 && index <= arrayPtr->size()) {
                        keys.push_back(std::to_string(index)); // ��������ӵ� keys ��
                        return arrayPtr->query(keys, i + 1, *arrayPtr);  // ������ѯ
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
            else {
                int index = std::stoi(keys[++i]);
                if (index > 0 && index <= arrayPtr->size()) {
                    // �ݹ����
                    return arrayPtr->query(keys, i, *arrayPtr);
                }
                else {
                    std::cout << "����������Χ: " << index << std::endl;
                    return false;
                }
            }
        }
        else {
            std::cout << "δ֪�������͡�" << std::endl;
            return false;
        }
    }
    return false;
}

// ����ʽ��ѯ����
void KJsonObject::interactiveQuery() {
    std::vector<std::string> keys;
    std::string input;
    getchar();
    // ��ȡ��ʼ�ļ�·��
    std::cout << "�������ѯ������(�ո�ָ�ÿ���): ";
    std::getline(std::cin, input);
    std::istringstream iss(input);
    std::string key;
    while (iss >> key) {
        keys.push_back(key);
    }

    // �ӵ�ǰ����ʼ��ѯ
    query(keys, 0, *this);
}


bool KJsonObject::serializeToJsonFile(const std::string& filePath) const {
    std::string content = serializeValueToJson();
    if (writeFile(filePath, content))
        return true;
    else 
        return false;
}

// ʵ��XML���л���д���ļ�
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
    std::string indent(indentLevel + 4, ' '); // ��ǰ�㼶������

    for (const auto& [key, value] : data) {
        if (!first) {
            jsonString += ",\n";
        }
        first = false;

        // ��Ӽ�
        jsonString += indent + "\"" + key + "\": ";

        // ����ͬ���͵�ֵ
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
                jsonString += itemSeparator + indent + "    " + item->serialize(indentLevel + 4); // ��������
                itemSeparator = ",\n"; // ���÷ָ���
            }
            jsonString += "\n" + indent + "]";
        }
        else if (auto jsonPtr = std::get_if<KJsonObject>(&value.value_)) {
            jsonString += jsonPtr->serializeValueToJson(indentLevel + 4); // ��������
        }
    }

    jsonString += "\n" + std::string(indentLevel, ' ') + "}";
    return jsonString;
}


// �ڲ��������������л���ֵ��ΪXML��ʽ
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
        // ֱ�ӵ��� serializeValueToXML ����ÿ����
        arrayPtr->serializeValueToXML(oss, *arrayPtr, indent + 4); // ʹ�� "value" ��Ϊ��ı�ǩ
        oss << indentation;
    }
    oss << "</" << key << ">\n";
}

//KJsonObject.cpp end