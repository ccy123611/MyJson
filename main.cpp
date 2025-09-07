#include "Headerfiles.h"

void showMenu() {
    std::cout << "请选择一个操作:" << std::endl;
    std::cout << "0. 退出" << std::endl;
    std::cout << "1. 选择待解析文件" << std::endl;
    std::cout << "2. 查询" << std::endl;
    std::cout << "3. 更新" << std::endl;
    std::cout << "4. 输出为 JSON 文件" << std::endl;
    std::cout << "5. 输出为 XML 文件" << std::endl;
}

void showUpdateMenu() {
    std::cout << "请选择一个操作:" << std::endl;
    std::cout << "0. 退出" << std::endl;
    std::cout << "1. 添加" << std::endl;
    std::cout << "2. 更改" << std::endl;
    std::cout << "3. 删除" << std::endl;
}

int main() {
    KJson json;
    std::shared_ptr<KJson> pendingPtr;
    KJsonObject jsonObject;
    KJsonArray jsonArray;
    std::string filePath;
    int choice = -1;
    std::string key;
    std::string value;
    bool is_object = false;
    bool is_array = false;

    while (choice != 0) {
        showMenu();
        std::cout << "请输入您的选择: ";
        std::cin >> choice;
        getchar();
        try {
            switch (choice) {
            case 0:
                std::cout << "退出程序。" << std::endl;
                break;
            case 1:
                std::cout << "请输入待解析文件的路径: ";
                std::getline(std::cin, filePath);
                //filePath = "D:\\Visual Studio Code\\KJSON-file\\input.json";
                pendingPtr = json.parseFromFile(filePath);
                if (auto arrayPtr = std::dynamic_pointer_cast<KJsonArray>(pendingPtr)) {
                    jsonArray = *arrayPtr;
                    std::cout << "文件 " << filePath << " 已成功解析。" << std::endl;
                    std::cout << "解析的文件是一个Json数组" << std::endl;
                    is_array = true;
                    is_object = false;
                }
                else if (auto objectPtr = std::dynamic_pointer_cast<KJsonObject>(pendingPtr)) {
                    jsonObject = *objectPtr;
                    std::cout << "文件 " << filePath << " 已成功解析。" << std::endl;
                    std::cout << "解析的文件是一个Json对象" << std::endl;
                    is_object = true;
                    is_array = false;
                }
                else {
                    std::cout << "文件 " << filePath << " 解析失败。" << std::endl;
                }
                break;
            case 2:
                if (!is_array && !is_object) {
                    std::cout << "没用可用的KJson数据";
                    break;
                }
                else if (is_object) {
                    jsonObject.interactiveQuery();
                }
                else if (is_array) {
                    jsonArray.interactiveQuery();
                }
                break;
            case 3:
                if (!is_array && !is_object) {
                    std::cout << "没用可用的KJson数据";
                    break;
                }
                else if (is_object) {
                    showUpdateMenu();
                    std::cout << "请输入您的选择: ";
                    std::cin >> choice;
                    switch (choice) {
                    case 0:
                        break;
                    case 1:
                        std::cout << "请输入要添加的键: ";
                        std::cin >> key;
                        std::cout << "请输入对应的值: ";
                        std::cin >> value;
                        jsonObject.add(key, value);
                        break;
                    case 2:
                        std::cout << "请输入要更改的键: ";
                        std::cin >> key;
                        std::cout << "请输入对应的值: ";
                        std::cin >> value;
                        jsonObject.remove(key);
                        jsonObject.add(key, value);
                        break;
                    case 3:
                        std::cout << "请输入要删除的键: ";
                        std::cin >> key;
                        jsonObject.remove(key);
                        break;
                    }
                }
                else if (is_array) {
                    std::cout << "对于Json数组的更新操作暂未实现" << std::endl;
                    //showUpdateMenu();
                    //std::cout << "请输入您的选择: ";
                    //std::cin >> choice;
                    //switch (choice){
                    //case 0:
                    //    break;
                    //case 1:
                    //    //std::cout << "请输入对应的数组元素: ";
                    //    //std::cin >> value;
                    //    //jsonArray.add(key, value);
                    //    break;
                    //case 2:
                    //    //std::cout << "请输入要更改的键: ";
                    //    //std::cin >> key;
                    //    //std::cout << "请输入对应的值: ";
                    //    //std::cin >> value;
                    //    //jsonObject.remove(key);
                    //    //jsonObject.add(key, value);
                    //    break;
                    //case 3:
                    //    //std::cout << "请输入要删除的数组索引: ";
                    //    //std::cin >> key;
                    //    //jsonObject.remove(key);
                    //    break;
                    //}
                }
                break;
            case 4:
                if (!is_array && !is_object) {
                    std::cout << "没用可用的KJson数据";
                    break;
                }
                else if (is_object) {
                    std::cout << "请输入输出文件的路径: ";
                    std::getline(std::cin, filePath);
                    //filePath = "D:\\Visual Studio Code\\KJSON-file\\output.json";
                    if(jsonObject.serializeToJsonFile(filePath))
                        std::cout << "JSON 数据已成功写入文件 " << filePath << "。" << std::endl;
                    else
                        std::cout << "JSON 数据写入失败 " << std::endl;
                }
                else if (is_array) {
                    std::cout << "请输入输出文件的路径: ";
                    std::getline(std::cin, filePath);
                    //filePath = "D:\\Visual Studio Code\\KJSON-file\\output-array.json";
                    if(jsonArray.serializeToJsonFile(filePath))
                        std::cout << "JSON 数据已成功写入文件 " << filePath << "。" << std::endl;
                    else
                        std::cout << "JSON 数据写入失败 " << std::endl;
                }
                break;
            case 5:
                if (!is_array && !is_object) {
                    std::cout << "没用可用的KJson数据";
                    break;
                }
                else if (is_object) {
                    std::cout << "请输入输出文件的路径: ";
                    std::getline(std::cin, filePath);
                    //filePath = "D:\\Visual Studio Code\\KJSON-file\\output.xml";
                    if(jsonObject.serializeToXMLFile(filePath))
                        std::cout << "XML 数据已成功写入到文件 " << filePath << "。" << std::endl;
                    else 
                        std::cout << "XML 数据写入失败 " << std::endl;
                }
                else if (is_array) {
                    std::cout << "请输入输出文件的路径: ";
                    std::getline(std::cin, filePath);
                    //filePath = "D:\\Visual Studio Code\\KJSON-Nonfile\\output-array.xml";
                    if(jsonArray.serializeToXMLFile(filePath))
                        std::cout << "XML 数据已成功写入到文件 " << filePath << "。" << std::endl;
                    else 
                        std::cout << "XML 数据写入失败 " << std::endl;
                }
                break;
            default:
                std::cout << "无效的选择，请重试。" << std::endl;
            }
        }
        catch (const std::exception& e) {
            std::cerr << "错误: " << e.what() << std::endl;
        }
    }

    return 0;
}
