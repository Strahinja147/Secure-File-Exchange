#include "Logger.h"

#include <sstream> 
#include <iomanip> 
#include <chrono>  
#include <ctime>   

const string Logger::LOG_FAJL = "aktivnosti.log";

void Logger::Log(const std::string& poruka)
{
    std::string unos = getTimestamp() + " " + poruka;

    //std::cout << unos << std::endl;

    std::ofstream fajl(LOG_FAJL, std::ios::app);
    if (fajl.is_open()) {
        fajl << unos << std::endl;
        fajl.flush();
        fajl.close();
    }

}

std::string Logger::getTimestamp()
{
    auto sada = std::chrono::system_clock::now();
    std::time_t ctime = std::chrono::system_clock::to_time_t(sada);

    std::tm gmt;
    localtime_s(&gmt, &ctime);

    std::stringstream ss;
    ss << std::put_time(&gmt, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}
