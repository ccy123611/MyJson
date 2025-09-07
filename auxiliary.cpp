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
            oss << ch; // �����ַ�ֱ�����
            break;
        }
    }
    return oss.str();
}