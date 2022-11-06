#include<iostream>
#include<string.h>
#include<backend.h>

int main()
{
    auto USERS = getUsers();

    FILE *arduino = fopen("/dev/ttyUSB0", "r");
    char usb[1000];
    while(1)
    {
        fread(usb, 10, 1, arduino); usb[8] = 0;
        bool match = false;
        for(int i = 0; i < USERS.size(); i++)
        {
            if(strcmp(usb, USERS[i].tag.c_str()) == 0)
            {
                match = true;
                if(getTimeDiff(USERS[i].lastEntry) >= 2)
                {
                    USERS[i].lastEntry = currentDateTime();
                    USERS[i].isPresent = !USERS[i].isPresent;
                    updateUser(USERS[i]);
                    if(USERS[i].isPresent)
                    {
                        std::cout << "Dobrodošao " << USERS[i].ime << std::endl;
                    }
                    else
                    {
                        std::cout << "Vidimo se " << USERS[i].ime << std::endl;
                    }
                    
                }
                break;
            }
        }
        if(!match) std::cout << "TAG DID NOT MATCH" << std::endl;
    }
    return 0;
}
