#include<iostream>

namespace db
{
    namespace
    {
        const std::string basePath = "C:\\db\\";
    }

    std::pair<std::string, std::string> getUserDateEndpoint(User *user, std::string date)
    {
        std::string dMonth = date.substr(0, 7);
        std::string dDay = date.substr(8, 2);
        return std::make_pair(basePath + user->tag + "\\" + dMonth + "\\", dDay);
    }

    std::vector<std::string> getUserRecords(User *user, std::string date)
    {
        auto path = db::getUserDateEndpoint(user, date);
        std::string buffer = io::readFile(path.first.append(path.second.append(".txt")));

        std::string record = "";
        std::vector<std::string> records;
        for(int i = 0; i < buffer.size(); i++)
        {
            if(buffer[i] == '\n')
            {
                records.push_back(record);
                record = "";
                continue;
            }
            record += buffer[i];
        }
        return records;
    }

    int addUserRecord(User *user)
    {
        std::string date = getTimeNow();
        auto path = db::getUserDateEndpoint(user, date);
        system(("if not exist \"" + path.first + "\" mkdir " + path.first).c_str());
        FILE * db = fopen(path.first.append(path.second.append(".txt")).c_str(), "a");
        if(!db) return 1;
        fprintf(db, "%s\n", date.substr(11, 8).c_str());
        fclose(db);

        user->lastEntry = date;
        user->isPresent = !user->isPresent;
        return 0;
    }

    void recordUsers(std::vector<User> USERS)
    {
        JSON json;
        for(auto user : USERS)
        {
            JSON j_user;
            j_user.Write("ime", user.ime);
            j_user.Write("prezime", user.prezime);
            j_user.Write("is_present", user.isPresent);
            json.Write(user.tag, j_user);
        }
        FILE * fp = fopen((basePath + "USERS.json").c_str(), "w");
        if(!fp)
        {
            std::cout << "ERROR: can't write users." << std::endl;
            return;
        }
        fprintf(fp, "%s\n", (json.GenerateJSON()).c_str());
        fclose(fp);
    }

    JSON toJson(User *user)
    {
        JSON json;
        auto months = io::readDir(basePath + user->tag);
        for(auto month : months)
        {
            JSON jmonths;
            auto days = io::readDir(basePath + user->tag + "\\" + month);
            if(days.size() == 0)
            {
                system(("rmdir " + basePath + user->tag + "\\" + month + "\\").c_str());
            }
            for(auto day : days)
            {
                JSON jday(JSON_ARRAY);
                auto records = db::getUserRecords(user, month + "-" + day.substr(0, 2));
                if(records.size() == 0) return JSON();
                for(auto record : records) jday.Write(record);
                jmonths.Write(day.substr(0, 2), jday);
            }
            json.Write(month, jmonths);
        }
        return json;
    }

    void userSync(User *user)
    {
        JSON local = db::toJson(user);
        auto months = local.GetAllO();
        for(int m = 0; m < months.size(); m++)
        {
            auto mkeys = local.getKeys();
            std::string month = mkeys[m];

            auto days = months[m].GetAllO();
            for(int d = 0; d < days.size(); d++)
            {
                auto dkeys = months[m].getKeys();
                std::string day = dkeys[d];

                std::string data = io::getSiteData(BASE_URL + STIME_ENDPOINT + "/" + user->tag + "/" + month + "/" + day);
                if(data.size() == 0) continue;
                std::vector<std::string> records;
                if(data != "null")
                {
                    JSON json;
                    json = json.TranslateJSON(data);
                    records = json.GetAllO()[0].GetAllS();
                }

                auto localRecords = days[d].GetAllS();
                records.insert(records.end(), localRecords.begin(), localRecords.end());
                JSON json; JSON array(JSON_ARRAY);
                for(auto record : records) array.Write(record);
                json.Write(day, array);
                if(io::setSiteData(BASE_URL + STIME_ENDPOINT + "/" + user->tag + "/" + month, truncateJSON(json.GenerateJSON())) == 0)
                {
                    system(("del " + basePath + user->tag + "\\" + month + "\\" + day + ".txt").c_str());
                }
            }
        }
    }
}
