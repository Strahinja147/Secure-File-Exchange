#pragma once
#include <string>
#include <fstream>
#include <iostream>
#include <chrono>
#include <iomanip>
using namespace std;
class Logger {
public:
    static void Log(const std::string& poruka);

private:
    static std::string getTimestamp();

    static const string LOG_FAJL;
};