#pragma once
#include <string>
#include <thread>
#include <atomic>
#include <windows.h> 
#include "SecurityService.h"
#include "Logger.h"

using namespace std;

class FSWatcher {
private:
    string targetFolder;
    int odabraniAlgoritam;
    string _lozinka; 

    atomic<bool> prekidac;
    thread watchThread;
    HANDLE hDir;

    void watchLoop();

public:
    FSWatcher();
    ~FSWatcher();

   
    void start(const string& path, int algoritam, const string& lozinka);
    void stop();
};