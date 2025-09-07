#include "Headerfiles.h"

void showMenu() {
    std::cout << "��ѡ��һ������:" << std::endl;
    std::cout << "0. �˳�" << std::endl;
    std::cout << "1. ѡ��������ļ�" << std::endl;
    std::cout << "2. ��ѯ" << std::endl;
    std::cout << "3. ����" << std::endl;
    std::cout << "4. ���Ϊ JSON �ļ�" << std::endl;
    std::cout << "5. ���Ϊ XML �ļ�" << std::endl;
}

void showUpdateMenu() {
    std::cout << "��ѡ��һ������:" << std::endl;
    std::cout << "0. �˳�" << std::endl;
    std::cout << "1. ���" << std::endl;
    std::cout << "2. ����" << std::endl;
    std::cout << "3. ɾ��" << std::endl;
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
        std::cout << "����������ѡ��: ";
        std::cin >> choice;
        getchar();
        try {
            switch (choice) {
            case 0:
                std::cout << "�˳�����" << std::endl;
                break;
            case 1:
                std::cout << "������������ļ���·��: ";
                std::getline(std::cin, filePath);
                //filePath = "D:\\Visual Studio Code\\KJSON-file\\input.json";
                pendingPtr = json.parseFromFile(filePath);
                if (auto arrayPtr = std::dynamic_pointer_cast<KJsonArray>(pendingPtr)) {
                    jsonArray = *arrayPtr;
                    std::cout << "�ļ� " << filePath << " �ѳɹ�������" << std::endl;
                    std::cout << "�������ļ���һ��Json����" << std::endl;
                    is_array = true;
                    is_object = false;
                }
                else if (auto objectPtr = std::dynamic_pointer_cast<KJsonObject>(pendingPtr)) {
                    jsonObject = *objectPtr;
                    std::cout << "�ļ� " << filePath << " �ѳɹ�������" << std::endl;
                    std::cout << "�������ļ���һ��Json����" << std::endl;
                    is_object = true;
                    is_array = false;
                }
                else {
                    std::cout << "�ļ� " << filePath << " ����ʧ�ܡ�" << std::endl;
                }
                break;
            case 2:
                if (!is_array && !is_object) {
                    std::cout << "û�ÿ��õ�KJson����";
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
                    std::cout << "û�ÿ��õ�KJson����";
                    break;
                }
                else if (is_object) {
                    showUpdateMenu();
                    std::cout << "����������ѡ��: ";
                    std::cin >> choice;
                    switch (choice) {
                    case 0:
                        break;
                    case 1:
                        std::cout << "������Ҫ��ӵļ�: ";
                        std::cin >> key;
                        std::cout << "�������Ӧ��ֵ: ";
                        std::cin >> value;
                        jsonObject.add(key, value);
                        break;
                    case 2:
                        std::cout << "������Ҫ���ĵļ�: ";
                        std::cin >> key;
                        std::cout << "�������Ӧ��ֵ: ";
                        std::cin >> value;
                        jsonObject.remove(key);
                        jsonObject.add(key, value);
                        break;
                    case 3:
                        std::cout << "������Ҫɾ���ļ�: ";
                        std::cin >> key;
                        jsonObject.remove(key);
                        break;
                    }
                }
                else if (is_array) {
                    std::cout << "����Json����ĸ��²�����δʵ��" << std::endl;
                    //showUpdateMenu();
                    //std::cout << "����������ѡ��: ";
                    //std::cin >> choice;
                    //switch (choice){
                    //case 0:
                    //    break;
                    //case 1:
                    //    //std::cout << "�������Ӧ������Ԫ��: ";
                    //    //std::cin >> value;
                    //    //jsonArray.add(key, value);
                    //    break;
                    //case 2:
                    //    //std::cout << "������Ҫ���ĵļ�: ";
                    //    //std::cin >> key;
                    //    //std::cout << "�������Ӧ��ֵ: ";
                    //    //std::cin >> value;
                    //    //jsonObject.remove(key);
                    //    //jsonObject.add(key, value);
                    //    break;
                    //case 3:
                    //    //std::cout << "������Ҫɾ������������: ";
                    //    //std::cin >> key;
                    //    //jsonObject.remove(key);
                    //    break;
                    //}
                }
                break;
            case 4:
                if (!is_array && !is_object) {
                    std::cout << "û�ÿ��õ�KJson����";
                    break;
                }
                else if (is_object) {
                    std::cout << "����������ļ���·��: ";
                    std::getline(std::cin, filePath);
                    //filePath = "D:\\Visual Studio Code\\KJSON-file\\output.json";
                    if(jsonObject.serializeToJsonFile(filePath))
                        std::cout << "JSON �����ѳɹ�д���ļ� " << filePath << "��" << std::endl;
                    else
                        std::cout << "JSON ����д��ʧ�� " << std::endl;
                }
                else if (is_array) {
                    std::cout << "����������ļ���·��: ";
                    std::getline(std::cin, filePath);
                    //filePath = "D:\\Visual Studio Code\\KJSON-file\\output-array.json";
                    if(jsonArray.serializeToJsonFile(filePath))
                        std::cout << "JSON �����ѳɹ�д���ļ� " << filePath << "��" << std::endl;
                    else
                        std::cout << "JSON ����д��ʧ�� " << std::endl;
                }
                break;
            case 5:
                if (!is_array && !is_object) {
                    std::cout << "û�ÿ��õ�KJson����";
                    break;
                }
                else if (is_object) {
                    std::cout << "����������ļ���·��: ";
                    std::getline(std::cin, filePath);
                    //filePath = "D:\\Visual Studio Code\\KJSON-file\\output.xml";
                    if(jsonObject.serializeToXMLFile(filePath))
                        std::cout << "XML �����ѳɹ�д�뵽�ļ� " << filePath << "��" << std::endl;
                    else 
                        std::cout << "XML ����д��ʧ�� " << std::endl;
                }
                else if (is_array) {
                    std::cout << "����������ļ���·��: ";
                    std::getline(std::cin, filePath);
                    //filePath = "D:\\Visual Studio Code\\KJSON-Nonfile\\output-array.xml";
                    if(jsonArray.serializeToXMLFile(filePath))
                        std::cout << "XML �����ѳɹ�д�뵽�ļ� " << filePath << "��" << std::endl;
                    else 
                        std::cout << "XML ����д��ʧ�� " << std::endl;
                }
                break;
            default:
                std::cout << "��Ч��ѡ�������ԡ�" << std::endl;
            }
        }
        catch (const std::exception& e) {
            std::cerr << "����: " << e.what() << std::endl;
        }
    }

    return 0;
}
