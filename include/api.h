#include<iostream>
#include<string.h>
#include<dirent.h>

namespace db
{
    std::pair<std::string, std::string> getUserDateEndpoint(User *user, std::string date)
    {
        std::string dMonth = date.substr(0, 7);
        std::string dDay = date.substr(8, 2);
        return std::make_pair("db/" + user->tag + "/" + dMonth + "/", dDay);
    }

    std::vector<std::string> getUserRecords(User *user, std::string date)
    {
        auto path = db::getUserDateEndpoint(user, date);
        FILE * db = fopen(path.first.append(path.second.append(".txt")).c_str(), "r");
        if(!db) return std::vector<std::string>();
        fseek(db, 0, SEEK_END);
        long fsize = ftell(db);
        fseek(db, 0, SEEK_SET);
        char* string = new char[fsize + 1];
        fread(string, fsize, 1, db);
        std::string buffer = string;
        delete[] string;
        fclose(db);

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

    void addUserRecord(User *user)
    {
        std::string date = getTimeNow();
        user->lastEntry = date;
        user->isPresent = !user->isPresent;

        auto path = db::getUserDateEndpoint(user, date);
        system(("mkdir -p " + path.first).c_str());
        FILE * db = fopen(path.first.append(path.second.append(".txt")).c_str(), "a");
        if(!db) return;
        fprintf(db, "%s\n", date.substr(11, 8).c_str());
        fclose(db);
    }

    std::vector<std::string> readDir(std::string path)
    {
        DIR * dir = opendir(path.c_str());
        if(!dir) return std::vector<std::string>();
        struct dirent * f;
        std::vector<std::string> files;
        while(f = readdir(dir)) if(strcmp(f->d_name, ".") != 0 && strcmp(f->d_name, "..") != 0) files.push_back(f->d_name);
        closedir(dir);
        return files;
    }

    JSON toJson(User *user)
    {
        JSON json;
        auto months = readDir("db/" + user->tag);
        for(auto month : months)
        {
            JSON jmonths;
            auto days = readDir("db/" + user->tag + "/" + month);
            if(days.size() == 0)
            {
                system(("rm -r db/" + user->tag + "/" + month + "/").c_str());
            }
            for(auto day : days)
            {
                JSON jday(JSON_ARRAY);
                auto records = getUserRecords(user, month + "-" + day.substr(0, 2));
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

                std::string data = getSiteData(BASE_URL + STIME_ENDPOINT + "/" + user->tag + "/" + month + "/" + day);
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
                if(setSiteData(BASE_URL + STIME_ENDPOINT + "/" + user->tag + "/" + month, truncateJSON(json.GenerateJSON())) == 0)
                {
                    system(("rm db/" + user->tag + "/" + month + "/" + day + ".txt").c_str());
                }
            }
        }
    }
}
