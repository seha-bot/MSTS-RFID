#include<iostream>
#include<backend.h>
#include<string.h>
// #include<pthread.h>
#include<windows.h>

void usbWriteOK()
{
    system("echo -ne \"1\" > /dev/ttyUSB0");
}
void usbWriteBAD()
{
    system("echo -ne \"2\" > /dev/ttyUSB0");
}

std::vector<User> USERS;

DWORD WINAPI t_smjena(LPVOID lpParameter)
{
    while(1)
    {
        auto tm = getTimeNowChrono();
        if(tm.tm_hour == 19 && tm.tm_min >= 30)
        {
            for(int i = 0; i < USERS.size(); i++)
            {
                if(USERS[i].isPresent) db::addUserRecord(&USERS[i]);
            }
        }
        Sleep(60000);
    }
    return 0;
}

DWORD WINAPI t_rebase(LPVOID lpParameter)
{
    while(1)
    {
        for(auto user : USERS)
        {
            db::userSync(&user);
            updateUser(&user);
        }
        Sleep(5000);
    }
    return 0;
}

int main()
{
    DWORD thread_id;
	CreateThread(0, 0, t_smjena, 0, 0, 0);
	CreateThread(0, 0, t_rebase, 0, 0, 0);

    USERS = getUsers();

    char usb[1000];
    std::string badRead = getTimeNow();
    while(1)
    {
        FILE *arduino = fopen("/dev/ttyUSB0", "r"); //TODO: add better null safety
        if(!arduino) continue;
        fread(usb, 10, 1, arduino); usb[8] = 0;
        fclose(arduino);
        
        bool match = false;
        for(auto& user : USERS)
        {
            if(strcmp(usb, user.tag.c_str()) == 0)
            {
                match = true;
                if(getTimeDiff(user.lastEntry) >= 2)
                {
                    if(db::addUserRecord(&user) != 0) match = false;
                    if(user.isPresent)
                    {
                        std::cout << "Dobrodošao " << user.ime << std::endl;
                        usbWriteOK();
                    }
                    else
                    {
                        std::cout << "Vidimo se " << user.ime << std::endl;
                        usbWriteOK();
                    }
                }
                break;
            }
        }
        if(!match && getTimeDiff(badRead) > 1)
        {
            badRead = getTimeNow();
            std::cout << "TAG DID NOT MATCH" << std::endl;
            usbWriteBAD();
        }
    }
    return 0;
}
