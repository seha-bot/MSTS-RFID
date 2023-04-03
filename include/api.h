#include<iostream>
#include<string>
#include<vector>
DWORD thread_id;

//Sve funkcije rade operacije na lokalne fajlove unutar basePath
//Jedine funkcije koje komuniciraju sa bazom su userSync i getUsers
//One nece crashati app ako ne uspiju izvrsiti

namespace db
{
    //Ovo se treba promijeniti ako se pokrece na windows
    const std::string basePath = "C:\\db\\";
    const std::string dirHeader = "\\";

    //Ovo generiše path za spremanje na bazu i lokalno na osnovu date-a
    std::pair<std::string, std::string> getUserDateEndpoint(User *user, std::string date)
    {
        std::string dMonth = date.substr(0, 7);
        std::string dDay = date.substr(8, 2);
        return std::make_pair(basePath + user->tag + dirHeader + dMonth + dirHeader, dDay);
    }

    //Ovo vrati sve lokalne upise za neki dan
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

    //Ova funkcija lokalno spremi novi upis i promijeni status prisutnosti usera
    //Spremljeni upis je trenutno vrijeme
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

    //Ova funkcija spremi sve usere i njihove podatke lokalno u bazu
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

    //Ova funkcija sluzi za generisanje JSON dokumenta od nekog usera
    //Podaci koji su spremljeni u lokalnu bazu se konvertuju
    //Vratit ce samo datume kada je koji user usao i izasao
    //Koristi isti format kao i firebase baza
    //Prazni folderi se obrisu
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

    void userRecordStatus(User *user)
    {
        JSON json;
        json.Write("is_present", user->isPresent);
        json.Write("last_entry", user->lastEntry);
        io::setSiteData(BASE_URL + USERS_ENDPOINT + "/" + user->tag, truncateJSON(json.GenerateJSON()));
    }

    struct SYNCPARAMS
    {
	User* user;
	std::string month;
	std::string day;
	std::string* localRecords;
	int size;
    };

    DWORD WINAPI t_sync(LPVOID lpParameter)
    {
	SYNCPARAMS* params = (SYNCPARAMS*)lpParameter;
	auto user = params->user;
	auto month = params->month;
	auto day = params->day;
	auto localRecords = params->localRecords;
	auto size = params->size;
        std::string data = io::getSiteData(BASE_URL + STIME_ENDPOINT + "/" + user->tag + "/" + month + "/" + day);
        if(data.empty())
	{
	    delete[] localRecords;
	    delete params;
	    return 0;
	}
        std::vector<std::string> records;
        if(data != "null")
        {
            JSON json;
            json = json.TranslateJSON(data);
            records = json.GetAllO()[0].GetAllS();
        }

        records.insert(records.end(), localRecords, localRecords + size);
        JSON json; JSON array(JSON_ARRAY);
        for(auto record : records) array.Write(record);
        json.Write(day, array);
	userRecordStatus(user);
        if(io::setSiteData(BASE_URL + STIME_ENDPOINT + "/" + user->tag + "/" + month, truncateJSON(json.GenerateJSON())) == 0)
        {
            io::rmfile(basePath + user->tag + dirHeader + month + dirHeader + day + ".txt");
	    std::cout << "SYNC COMPLETE " << user->ime << std::endl;
        }
	delete[] localRecords;
	delete params;
	return 0;
    }

    //Ova funkcija ce sve lokalno spremljene dokumente od nekog usera poslati u bazu
    //Nece obrisati spremljene podatke iz baze, vec ce ih prvo procitati i onda nadodati na njih
    //To je loša metoda jer zahtjeva 2 upita, ali od koga je dobro je :)
    //Ako uspije, obrisat ce lokalne zapise
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

		SYNCPARAMS* params = new SYNCPARAMS;
		params->user = user;
		params->month = month;
		params->day = day;
                auto localRecordsVec = days[d].GetAllS();
		params->size = localRecordsVec.size();
		std::string* localRecords = new std::string[params->size];
		for(int i = 0; i < params->size; i++) localRecords[i] = localRecordsVec[i];
		params->localRecords = localRecords;
		CreateThread(0, 0, t_sync, params, 0, &thread_id);
            }
        }
    }

    //Ova funkcija prvo proba procitati usere sa firebase
    //Ako to ne uspije, citat ce iz lokalne baze
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

        if(USERS.empty()) std::cout << "Error: PROBLEM GETTING USERS" << std::endl;
        return USERS;
    }
}
