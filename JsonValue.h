#pragma once
#include"KJsonArray.h"
#include"KJsonObject.h"

class JsonValue {
public:
    using ValueType = std::variant<std::string, int, double, bool, std::nullptr_t, KJsonArray, KJsonObject>;

    JsonValue() : value_(nullptr) {}

    JsonValue(const std::string& str) : value_(str) {}
    JsonValue(int i) : value_(i) {}
    JsonValue(double d) : value_(d) {}
    JsonValue(bool b) : value_(b) {}
    JsonValue(std::nullptr_t) : value_(nullptr) {}
    JsonValue(const KJsonArray& array) : value_(array) {}
    JsonValue(const KJsonObject& json) : value_(json) {}

    // Getter functions to access the value
    const ValueType& value() const { return value_; }

    std::string serialize(int indentLevel = 0) const {
        std::string jsonString;

        if (auto strPtr = std::get_if<std::string>(&value_)) {
            jsonString += "\"" + *strPtr + "\"";
        }
        else if (auto intPtr = std::get_if<int>(&value_)) {
            jsonString += std::to_string(*intPtr);
        }
        else if (auto doublePtr = std::get_if<double>(&value_)) {
            jsonString += std::to_string(*doublePtr);
        }
        else if (auto boolPtr = std::get_if<bool>(&value_)) {
            jsonString += *boolPtr ? "true" : "false";
        }
        else if (std::get_if<std::nullptr_t>(&value_)) {
            jsonString += "null";
        }
        else if (auto arrayPtr = std::get_if<KJsonArray>(&value_)) {
            jsonString += "[\n";
            std::string itemSeparator;
            for (const auto& item : *arrayPtr) {
                jsonString += itemSeparator + std::string(indentLevel + 4, ' ') + item->serialize(indentLevel + 4);
                itemSeparator = ",\n"; // 设置分隔符
            }
            jsonString += "\n" + std::string(indentLevel, ' ') + "]";
        }
        else if (auto jsonPtr = std::get_if<KJsonObject>(&value_)) {
            jsonString += jsonPtr->serializeValueToJson(indentLevel); // 增加缩进
        }

        return jsonString;
    }


private:
    ValueType value_;
    friend class KJsonObject;
    friend class KJsonArray;
};