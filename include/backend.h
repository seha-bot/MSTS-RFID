#include<JSON.h>
#include<iostream>
#include<curl.h>
#include<time_diff.h>

const std::string BASE_URL = "https://msts-rfid-default-rtdb.europe-west1.firebasedatabase.app/";
const std::string USERS_ENDPOINT = "USERS";

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

size_t getUserCount()
{
    JSON json;
    json = json.TranslateJSON(getSiteData(BASE_URL + USERS_ENDPOINT));
    return json.GetAllO()[0].GetAllO().size();
}

void updateUser(User user)
{
    JSON json;
    json.Write("ime", user.ime);
    json.Write("prezime", user.prezime);
    json.Write("is_present", user.isPresent);
    json.Write("last_entry", user.lastEntry);
    setSiteData(BASE_URL + USERS_ENDPOINT + "/" + user.tag, truncateJSON(json.GenerateJSON()));
}
