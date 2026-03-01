#pragma once
#include <winsock2.h>
#include <ws2tcpip.h>
#include <string>
#include <thread>
#include <atomic>
#include <vector>
#include "Logger.h"
#include "SecurityService.h"

#pragma comment(lib, "ws2_32.lib")

class NetworkService {
private:
    SOCKET listensock;
    std::atomic<bool> isRunning;
    std::thread serverThread;
    std::string _sessionPassword; // Lozinka sesije za automatsko otpakivanje

    void serverLoop(int port);

public:
    NetworkService();
    ~NetworkService();

    // Dodat argument lozinka
    void pokreniServer(int port, const std::string& lozinka);
    void zaustaviServer();

    bool posaljiFajl(const std::string& ipAdresa, int port, const std::string& putanjaDoFajla);
};