#include<iostream>
#include<backend.h>
#include<string.h>
#include<serial.h>

std::vector<User> USERS;
bool user_lock = false;

DWORD WINAPI t_smjena(LPVOID lpParameter)
{
    while(1)
    {
        time_t tt = atot(getTimeNow());
        auto tm = *localtime(&tt);
        if(tm.tm_hour == 19 && tm.tm_min >= 30)
        {
            if(user_lock) continue;
            user_lock = true;
            for(int i = 0; i < USERS.size(); i++)
            {
                if(USERS[i].isPresent) db::addUserRecord(&USERS[i]);
            }
            db::recordUsers(USERS);
            user_lock = false;
        }
        Sleep(60000);
    }
    return 0;
}

DWORD WINAPI t_rebase(LPVOID lpParameter)
{
    while(1)
    {
        if(user_lock) continue;
        user_lock = true;
        for(auto user : USERS)
        {
            db::userSync(&user);
            updateUser(&user);
        }
        USERS = getUsers(); //TODO: fetch data from USERS.json if there is no internet
        db::recordUsers(USERS);
        user_lock = false;
        Sleep(5000);
    }
    return 0;
}

int main()
{
    DWORD thread_id;
    CreateThread(0, 0, t_smjena, 0, 0, 0);
    CreateThread(0, 0, t_rebase, 0, 0, 0);

    serial::openPort("COM5");
    std::string badRead = getTimeNow();
    while(1)
    {
        std::string usb = serial::readTag();
        if(usb.empty()) continue;

        bool match = false;
        if(user_lock) continue;
        user_lock = true;
        for(auto& user : USERS)
        {
            if(usb.compare(user.tag) == 0)
            {
                match = true;
                if(getTimeDiff(user.lastEntry) >= 2)
                {
                    if(db::addUserRecord(&user) == 0)
                    {
                        db::recordUsers(USERS);
                        if(user.isPresent)
                        {
                            std::cout << "Dobrodosao " << user.ime << std::endl;
                            serial::usbWriteOK();
                        }
                        else
                        {
                            std::cout << "Vidimo se " << user.ime << std::endl;
                            serial::usbWriteOK();
                        }
                    }
                    else match = false;
                }
                break;
            }
        }
        user_lock = false;
        if(!match && getTimeDiff(badRead) > 1)
        {
            badRead = getTimeNow();
            std::cout << "TAG DID NOT MATCH" << std::endl;
            //TODO: too much file write repetition code. Can be refactored.
            FILE * fp = fopen((db::basePath + "NEW").c_str(), "w");
            if(fp) fprintf(fp, "%s\n", usb.c_str());
            fclose(fp);
            serial::usbWriteBAD();
        }
    }
    return 0;
}
