#include<JSON.h>
#include<iodisk.h>
#include<time_diff.h>

//TODO: simplify code structure: backend.h and api.h serve almost same purpose so merge them

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
        if(lastEntry.size() == 0) this->lastEntry = getTimeNow();
        else this->lastEntry = lastEntry;
    }
};


#include<api.h>

std::vector<User> getUsers()
{
    JSON json;
    std::string buffer = io::getSiteData(BASE_URL + USERS_ENDPOINT);
    if(buffer.empty()) buffer = truncateJSON(io::readFile(db::basePath + "USERS.json"));
    json = json.TranslateJSON(buffer);
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
    io::setSiteData(BASE_URL + USERS_ENDPOINT + "/" + user->tag, truncateJSON(json.GenerateJSON()));
}
