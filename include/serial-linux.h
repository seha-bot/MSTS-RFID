#include <iostream>
#include <unistd.h>

// Ovo je namespace za komunikaciju sa arduinom
// Ako zelite ovo portati na windows, samo prekopirajte kod sa windows brancha

namespace serial
{
    FILE *arduino;
    std::string port;

    // Na windowsu ce biti neki COM port, a na linuxu skontajte sami
    int openPort(std::string _port)
    {
        port = _port;
        arduino = fopen(port.c_str(), "rw");
        if (arduino == 0)
        {
            return 1;
        }
        return 0;
    }

    // Procita trenutni buffer sa serial porta i vrati kao string
    std::string readTag()
    {
        std::string s = "";
        char c;
        while (1)
        {
            c = fgetc(arduino);
            if (c == '\n' || c == '\r' || c == '\0')
                return s;
            s += c;
        }
        return s;
    }

    void writeStatus(std::string data_sent)
    {
        system(("echo '" + data_sent + "' >> " + port).c_str());
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
