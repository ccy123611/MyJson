#include "Headerfiles.h"

std::string escapeXml(const std::string& input) {
    std::ostringstream oss;
    for (char ch : input) {
        switch (ch) {
        case '<':
            oss << "&lt;";
            break;
        case '>':
            oss << "&gt;";
            break;
        case '&':
            oss << "&amp;";
            break;
        case '"':
            oss << "&quot;";
            break;
        case '\'':
            oss << "&apos;";
            break;
        default:
            oss << ch; // 其他字符直接添加
            break;
        }
    }
    return oss.str();
}