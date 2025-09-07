#include"Headerfiles.h"

//KJson.cpp

std::string KJson::readFile(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: " + filePath);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

std::shared_ptr<KJson> KJson::parseFromFile(const std::string& filePath) {
    std::string jsonString = readFile(filePath);
    KJsonParser parser(jsonString.c_str(), jsonString.length());
    std::shared_ptr<KJson> result = parser.parse();
    if (parser.lastError != "") {
        std::cout << "����ʧ��,����ԭ��: " << parser.lastError << std::endl;
        return NULL;
    }
    if (auto arrayPtr = std::dynamic_pointer_cast<KJsonArray>(result)) {
        return arrayPtr;// ���� KJsonArray
    }
    else if (auto objectPtr = std::dynamic_pointer_cast<KJsonObject>(result)) {
        return objectPtr;// ���� KJsonObject
    }
    else {
        std::cout << "δ֪����" << std::endl;
        return NULL;
    }
}

//KJson.cpp end;






