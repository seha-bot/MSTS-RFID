#include<iostream>

namespace db
{
    std::vector<std::string> getUserRecords(User *user, std::string date)
    {
        FILE * db = fopen("local.json", "r");
        fseek(db, 0, SEEK_END);
        long fsize = ftell(db);
        fseek(db, 0, SEEK_SET);
        char* string = new char[fsize + 1];
        fread(string, fsize, 1, db);
        std::string buffer = string;
        delete[] string;
        fclose(db);

        JSON json;
        json = json.TranslateJSON(truncateJSON(buffer));
        return json.GetO(user->tag).GetO(date.substr(0, 7)).GetO(date.substr(8, 2)).GetAllS();
    }

    void downloadDB()
    {
        FILE * db = fopen("local.json", "w");
        std::string data = getSiteData(BASE_URL + STIME_ENDPOINT);
        JSON json;
        json = json.TranslateJSON(data);
        data = json.GenerateJSON();
        fprintf(db, "%s\n", data.c_str());
        fclose(db);

        for(auto t : db::getUserRecords(&getUsers()[0], getTimeNow()))
        {
            std::cout << t << std::endl;
        }
    }
}
