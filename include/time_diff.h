#include<string>
#include<time.h>

//format YYYY-MM-DD.HH:mm:ss
const std::string getTimeNow() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
    return buf;
}

time_t atot(std::string strtime)
{
    time_t now = time(0);
    struct tm tstruct = {
        .tm_sec = stoi(strtime.substr(17, 2)),
        .tm_min = stoi(strtime.substr(14, 2)),
        .tm_hour = stoi(strtime.substr(11, 2)),
        .tm_mday = stoi(strtime.substr(8, 2)),
        .tm_mon = stoi(strtime.substr(5, 2)) - 1,
        .tm_year = stoi(strtime.substr(0, 4)) - 1900,
        .tm_wday = -1,
        .tm_yday = -1,
        .tm_isdst = -1
    };
    char       buf[80];
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
    return mktime(&tstruct);
}

int getTimeDiff(std::string oldTime)
{
    return atot(getTimeNow()) - atot(oldTime);
}
