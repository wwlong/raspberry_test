# tools_study
## 格式化获取JSON格式的文件内容分
1.  打开JSON文件,获取Json格式的内容
2.  创建JSON reader并parse
3.  对parse的结果进行获取

### 例如格式话获取一个test.json的内容
#### step0
    std::ifstream ifs;
    Json::Reader reader;
    Json::Value root;
#### step1:
    ifs.open(filepath);
#### step2:
    reader.parse(ifs, root, false)
#### step3:简单粗暴一个个的解析
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