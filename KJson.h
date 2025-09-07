#pragma once
#include <string>
#include <variant>
#include <vector>
#include <iostream>
#include <fstream>
#include <memory>
#include <optional>
#include <unordered_map>
#include <variant>
#include <sstream>
#include <cctype>
#include <mutex>
#include <stdexcept>
#include <cstring>
#include <cstdint>

class KJson {
public:
    virtual ~KJson() = default; // 虚析构函数

    // 解析JSON文件并返回KJson对象
    std::shared_ptr<KJson> parseFromFile(const std::string& filePath);//调用parse

    static std::string readFile(const std::string& filePath);
};