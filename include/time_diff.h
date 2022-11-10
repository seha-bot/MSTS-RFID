#include<string>
#include<chrono>

//format YYYY-MM-DD.HH:mm:ss
const std::string getTimeNow() {
    time_t     now = time(0);
    std::tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);
    return buf;
}

int getTimeDiff(std::string oldTime)
{
    std::tm tm = {};
    strptime(oldTime.c_str(), "%Y-%m-%d.%X", &tm);
    auto last = std::chrono::system_clock::from_time_t(std::mktime(&tm));

    strptime(getTimeNow().c_str(), "%Y-%m-%d.%X", &tm);
    auto now = std::chrono::system_clock::from_time_t(std::mktime(&tm));

    return std::chrono::duration<double, std::milli>(now-last).count() * 0.001;
}

std::tm getTimeNowChrono()
{
    std::tm tm = {};
    strptime(getTimeNow().c_str(), "%Y-%m-%d.%X", &tm);
    return tm;
}
