#include<iostream>
#include<windows.h>

namespace serial
{
    HANDLE hComm;

    void openPort(std::string port)
    {
        hComm = CreateFileA(port.c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
        if(hComm == INVALID_HANDLE_VALUE) return;

        DCB dcbSerialParam = {0};
        GetCommState(hComm, &dcbSerialParam);
        dcbSerialParam.BaudRate=CBR_115200;
        dcbSerialParam.ByteSize=8;
        dcbSerialParam.StopBits=ONESTOPBIT;
        dcbSerialParam.Parity=NOPARITY;
        dcbSerialParam.fDtrControl = DTR_CONTROL_ENABLE;
        SetCommState(hComm, &dcbSerialParam);
    }

    std::string readTag()
    {
        DWORD word = 0;
        std::string s = "";
        char c[1];
        while(1)
        {
            ReadFile(hComm, c, 1, &word, 0);
            if(c[0] == '\n' || c[0] == '\r' || c[0] == '\0') return s;
            s += c[0];
        }
        return s;
    }
}
