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
