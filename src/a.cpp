#include<iostream>
#include<backend.h>
#include<string.h>
#include<pthread.h>
#include<unistd.h>

void usbWriteOK()
{
    system("echo -ne \"1\" > /dev/ttyUSB0");
}
void usbWriteBAD()
{
    system("echo -ne \"2\" > /dev/ttyUSB0");
}

std::vector<User> USERS;

void* t_smjena(void*)
{
    while(1)
    {
        auto tm = getTimeNowChrono();
        if(tm.tm_hour == 19 && tm.tm_min >= 30)
        {
            for(int i = 0; i < USERS.size(); i++)
            {
                if(USERS[i].isPresent) addUserRecord(&USERS[i]);
            }
        }
        sleep(60);
    }
    return nullptr;
}

void* t_rebase(void*)
{
    while(1) //TODO: not working, add r/w access bools
    {
        sleep(5);
        USERS = getUsers();
    }
    return nullptr;
}

int main()
{
    //TODO: Refactor threads in a new file
    pthread_t thread_id;
    pthread_create(&thread_id, 0, t_smjena, 0);
    // pthread_create(&thread_id, 0, t_rebase, 0);

    USERS = getUsers();

    FILE *arduino = fopen("/dev/ttyUSB0", "r");
    char usb[1000];
    std::string badRead = getTimeNow();
    while(1)
    {
        fread(usb, 10, 1, arduino); usb[8] = 0;
        bool match = false;
        for(int i = 0; i < USERS.size(); i++) //TODO: for(auto user : USERS)
        {
            if(strcmp(usb, USERS[i].tag.c_str()) == 0)
            {
                match = true;
                if(USERS[i].lastEntry.size() == 0 || getTimeDiff(USERS[i].lastEntry) >= 2) //TODO: uredi ovaj if
                {
                    if(USERS[i].isPresent)
                    {
                        std::cout << "Dobrodošao " << USERS[i].ime << std::endl;
                        usbWriteOK();
                    }
                    else
                    {
                        std::cout << "Vidimo se " << USERS[i].ime << std::endl;
                        usbWriteOK();
                    }
                    addUserRecord(&USERS[i]);
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
