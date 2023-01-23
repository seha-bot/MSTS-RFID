#include<iostream>
#include<unistd.h>

namespace serial
{
    FILE* arduino;

    int openPort(std::string port)
    {
        arduino = fopen(port.c_str(), "rw");
        if(arduino == 0) return 1;
        return 0;
    }

    std::string readTag()
    {
        std::string s = "";
        char c;
        while(1)
        {
            c = fgetc(arduino);
            if(c == '\n' || c == '\r' || c == '\0') return s;
            s += c;
        }
        return s;
    }

    void writeStatus(const char *data_sent)
    {
        fprintf(arduino, "%s", data_sent);
    }
    
    void usbWriteOK()
    {
        serial::writeStatus("1");
    }
    void usbWriteBAD()
    {
        serial::writeStatus("2");
    }
}
