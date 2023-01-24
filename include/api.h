#include<iostream>
#include<string>
#include<vector>

namespace db
{
    const std::string basePath = "/db/";
    const std::string dirHeader = "/";

    std::pair<std::string, std::string> getUserDateEndpoint(User *user, std::string date)
    {
        std::string dMonth = date.substr(0, 7);
        std::string dDay = date.substr(8, 2);
        return std::make_pair(basePath + user->tag + dirHeader + dMonth + dirHeader, dDay);
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
        io::mkdir(path.first);

        if(io::appendFile(path.first.append(path.second.append(".txt")), date.substr(11, 8)))
        {
            std::cout << "Error: failed writing new record to file." << std::endl;
            return 1;
        }

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
            j_user.Write("last_entry", user.lastEntry);
            json.Write(user.tag, j_user);
        }
        if(json.GetAllO().empty()) return;
        if(io::writeFile(basePath + "USERS.json", json.GenerateJSON()))
        {
            std::cout << "ERROR: can't write users." << std::endl;
            return;
        }
    }

    JSON toJson(User *user)
    {
        JSON json;
        auto months = io::readDir(basePath + user->tag);
        for(auto month : months)
        {
            JSON jmonths;
            auto days = io::readDir(basePath + user->tag + dirHeader + month);
            if(days.size() == 0)
            {
                io::rmdir(basePath + user->tag + dirHeader + month + dirHeader);
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

    int userSync(User *user)
    {
        int status = 1;
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
                if(data.empty()) continue;
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
                    io::rmfile(basePath + user->tag + dirHeader + month + dirHeader + day + ".txt");
                    status = 0;
                }
                else status = 1;
            }
        }
        return status;
    }
}
