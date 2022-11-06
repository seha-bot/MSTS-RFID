#include<iostream>
#include<string.h>
#include<backend.h>

void usbWriteOK()
{
    system("echo -ne \"1\" > /dev/ttyUSB0");
}
void usbWriteBAD()
{
    system("echo -ne \"2\" > /dev/ttyUSB0");
}

int main()
{
    auto USERS = getUsers();

    FILE *arduino = fopen("/dev/ttyUSB0", "r");
    char usb[1000];
    std::string badRead = getTimeNow();
    while(1)
    {
        fread(usb, 10, 1, arduino); usb[8] = 0;
        bool match = false;
        for(int i = 0; i < USERS.size(); i++)
        {
            if(strcmp(usb, USERS[i].tag.c_str()) == 0)
            {
                match = true;
                if(USERS[i].lastEntry.size() == 0 || getTimeDiff(USERS[i].lastEntry) >= 2)
                {
                    USERS[i].lastEntry = getTimeNow();
                    USERS[i].isPresent = !USERS[i].isPresent;
                    updateUser(USERS[i]);
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
