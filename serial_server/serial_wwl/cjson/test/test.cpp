#include <iostream>
#include <fstream>
#include "json.h"
#include <stdint.h>
#include <string>
using namespace std;

#define flag_dbg(format...)                          \
    {                                                \
        printf("%s -- %d:", __FUNCTION__, __LINE__); \
        printf("\e[1;31m");                          \
        printf(format);                              \
        printf(" \e[0m");                            \
    }
/*
 *  测试json的解析功能
 *  读取一个test.json文件
 *  讲里面的内容读取出来并显示
 * */
typedef struct person
{
    std::string name;
    std::string age;
    std::string sex;
    std::string phone_num;
} person;
typedef struct family_location
{
    std::string contient;
    std::string country;
    std::string province;
} location;
typedef struct family
{
    std::string family_name;
    person father;
    person mother;
    person son;
    person daughter;
    family_location location;
} family;

void parse_config_param_json(const char *filepath)
{
    std::ifstream ifs;
    Json::Reader reader;
    Json::Value root;
    bool ret;
    family happy_family;
    if (NULL == filepath)
    {
        flag_dbg("filepath NULL\n");
        return;
    }
    ifs.open(filepath);
    if (!ifs.is_open())
    {
        flag_dbg("%s open failed\n", filepath);
        return;
    }

    if (!reader.parse(ifs, root, false))
    {
        flag_dbg("parse failed\n");
        return;
    }

    happy_family.family_name = root["family_name"].asString();
    const auto &family_member_store = root["family_members"];
    const auto &family_member_father = family_member_store["father"];
    happy_family.father.name = family_member_father["name"].asString();
    happy_family.father.age = family_member_father["age"].asString();
    happy_family.father.sex = family_member_father["sex"].asString();
    happy_family.father.phone_num = family_member_father["phone_num"].asString();
    const auto &family_member_mother = family_member_store["mother"];
    happy_family.mother.name = family_member_mother["name"].asString();
    happy_family.mother.age = family_member_mother["age"].asString();
    happy_family.mother.sex = family_member_mother["sex"].asString();
    happy_family.mother.phone_num = family_member_mother["phone_num"].asString();
    const auto &family_member_son = family_member_store["son"];
    happy_family.son.name = family_member_son["name"].asString();
    happy_family.son.age = family_member_son["age"].asString();
    happy_family.son.sex = family_member_son["sex"].asString();
    happy_family.son.phone_num = family_member_son["phone_num"].asString();
    const auto &family_member_daughter = family_member_store["daughter"];
    happy_family.daughter.name = family_member_daughter["name"].asString();
    happy_family.daughter.age = family_member_daughter["age"].asString();
    happy_family.daughter.sex = family_member_daughter["sex"].asString();
    happy_family.daughter.phone_num = family_member_daughter["phone_num"].asString();
    const auto &family_member_location = root["location"];
    happy_family.location.contient = family_member_location["contient"].asString();
    happy_family.location.country = family_member_location["country"].asString();
    happy_family.location.province = family_member_location["province"].asString();
    std::cout << happy_family.family_name << endl;
    std::cout << happy_family.father.name << endl;
    std::cout << happy_family.father.age << endl;
    std::cout << happy_family.father.sex << endl;
    std::cout << happy_family.father.phone_num << endl;
    std::cout << happy_family.mother.name << endl;
    std::cout << happy_family.mother.age << endl;
    std::cout << happy_family.mother.sex << endl;
    std::cout << happy_family.mother.phone_num << endl;
    std::cout << happy_family.son.name << endl;
    std::cout << happy_family.son.age << endl;
    std::cout << happy_family.son.sex << endl;
    std::cout << happy_family.son.phone_num << endl;
    std::cout << happy_family.daughter.name << endl;
    std::cout << happy_family.daughter.age << endl;
    std::cout << happy_family.daughter.sex << endl;
    std::cout << happy_family.daughter.phone_num << endl;
    std::cout << happy_family.location.contient << endl;
    std::cout << happy_family.location.country << endl;
    std::cout << happy_family.location.province << endl;
}
/*
 *  解析一个json string 
 * */

void parse_json_string_demo(void) {

    Json::Reader reader;
    Json::Value root;
    person man;
    string lily = "{\"name\":\"lily\",\"age\":\"20\",\"sex\":\"female\",\"phone_num\":\"123\"}";
    if (!reader.parse(lily, root, false))
    {
        flag_dbg("parse failed\n");
        return;
    }
    man.name = root["name"].asString();

    std::cout << man.name << std::endl;
}


void usage(void)
{
    std::cout << "invalid use\n"
              << std::endl
              << "eg : ./test1 ./test.json" << std::endl;
}
int main(int32_t argc, char **argv)
{
    if (argc < 2)
    {
        usage();
        return -1;
    }
    std::string filename = argv[1];
    if (filename.empty())
    {
        flag_dbg("filename is empty\n");
        return -1;
    }
    parse_config_param_json(filename.c_str());

/*
 *  直接解析json string 
 * */
    parse_json_string_demo();
    return 0;
}
