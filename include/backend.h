#include<JSON.h>
#include<curl.h>
#include<time_diff.h>

const std::string BASE_URL = "https://msts-rfid-default-rtdb.europe-west1.firebasedatabase.app/";
const std::string USERS_ENDPOINT = "USERS";
const std::string STIME_ENDPOINT = "STIME";

class User
{
public:
    std::string tag;
    std::string ime;
    std::string prezime;
    bool isPresent;
    std::string lastEntry;

    User(std::string tag, std::string ime, std::string prezime, bool isPresent, std::string lastEntry)
    {
        this->tag = tag;
        this->ime = ime;
        this->prezime = prezime;
        this->isPresent = isPresent;
        this->lastEntry = lastEntry;
    }
};

std::vector<User> getUsers()
{
    JSON json;
    json = json.TranslateJSON(getSiteData(BASE_URL + USERS_ENDPOINT));
    std::vector<JSON> list = json.GetAllO();
    std::vector<std::string> keys = json.getKeys();

    std::vector<User> USERS;
    for(int i = 0; i < list.size(); i++)
    {
        USERS.push_back(User(keys[i], list[i].GetS("ime"), list[i].GetS("prezime"), list[i].GetB("is_present"), list[i].GetS("last_entry")));
    }
    return USERS;
}

void updateUser(User *user)
{
    JSON json;
    json.Write("ime", user->ime);
    json.Write("prezime", user->prezime);
    json.Write("is_present", user->isPresent);
    json.Write("last_entry", user->lastEntry);
    setSiteData(BASE_URL + USERS_ENDPOINT + "/" + user->tag, truncateJSON(json.GenerateJSON()));
}

std::string getUserDateEndpoint(User *user, std::string date)
{
    std::string dMonth = date.substr(0, 7);
    std::string dDay = date.substr(8, 2);
    return BASE_URL + STIME_ENDPOINT + "/" + user->tag + "/" + dMonth + "/" + dDay;
}

std::vector<std::string> getUserRecords(User *user, std::string date)
{
    std::string records = getSiteData(getUserDateEndpoint(user, date));
    if(records == "null") return std::vector<std::string>();
    JSON json;
    json = json.TranslateJSON(records);
    return json.GetAllO()[0].GetAllS();
}

void setUserRecords(User *user, std::string date, std::vector<std::string> records)
{
    JSON json;
    for(int i = 0; i < records.size(); i++) json.Write(std::to_string(i), records[i]);
    setSiteData(getUserDateEndpoint(user, date), truncateJSON(json.GenerateJSON()));
}

void addUserRecord(User *user)
{
    std::string date = getTimeNow();
    user->lastEntry = date;
    user->isPresent = !user->isPresent;
    updateUser(user);

    auto records = getUserRecords(user, date);
    records.push_back(date.substr(11, 8));
    setUserRecords(user, date, records);
}

#include<offline.h>
