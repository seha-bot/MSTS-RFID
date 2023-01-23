#include<iostream>
#include<backend.h>
#include<string.h>
#include<serial.h>
#include<pthread.h>

std::vector<User> USERS;
bool user_lock = false;
bool has_wifi = false;

void* t_smjena(void*)
{
    while(1)
    {
        time_t tt = atot(getTimeNow());
        auto tm = *localtime(&tt);
        if(tm.tm_hour == 19 && tm.tm_min >= 30)
        {
            if(user_lock) continue;
            user_lock = true;
            for(auto& user : USERS)
            {
                if(user.isPresent)
                {
                    std::cout << "Logging out " << user.ime << std::endl;
                    db::addUserRecord(&user);
                }
                db::userSync(&user);
            }
            db::recordUsers(USERS);
            user_lock = false;
        }
        sleep(60);
    }
    return nullptr;
}

int main()
{
    USERS = getUsers();
    if(USERS.empty()) return 0;
    db::recordUsers(USERS);

    if(serial::openPort("/dev/ttyUSB0"))
    {
        std::cout << "USB OPEN ERROR" << std::endl;
        return 0;
    }
    std::string badRead = getTimeNow();
    while(1)
    {
        std::string usb = truncateJSON(serial::readTag());
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

                        bool newStatus = db::userSync(&user) == 0;
                        if(!has_wifi && newStatus)
                        {
                            for(int i = 0; i < USERS.size(); i++)
                            {
                                if(USERS[i].tag != user.tag) db::userSync(&USERS[i]);
                            }
                        }
                        has_wifi = newStatus;

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
        if(!match && getTimeDiff(badRead) > 1)
        {
            badRead = getTimeNow();
            std::cout << "TAG DID NOT MATCH " << usb << std::endl;
            io::writeFile(db::basePath + "NEW", usb);
            serial::usbWriteBAD();
            USERS = getUsers();
        }
        user_lock = false;
    }
    return 0;
}
