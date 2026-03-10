#include "NetworkService.h"
#include "Logger.h"
#include "FileManager.h"

NetworkService::NetworkService() : listensock(INVALID_SOCKET), isRunning(false), _sessionPassword("") {
    WSAData wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        Logger::Log("GRESKA: Winsock inicijalizacija nije uspela.");
    }
}

NetworkService::~NetworkService() {
    zaustaviServer();
    WSACleanup();
}

void NetworkService::pokreniServer(int port, const std::string& lozinka) {
    if (isRunning) return;
    this->_sessionPassword = lozinka; 
    isRunning = true;
    serverThread = std::thread(&NetworkService::serverLoop, this, port);
}

void NetworkService::zaustaviServer() {
    isRunning = false;
    if (listensock != INVALID_SOCKET) {
        closesocket(listensock);
        listensock = INVALID_SOCKET;
    }
    if (serverThread.joinable()) {
        serverThread.join();
    }
    _sessionPassword = "";
}

void NetworkService::serverLoop(int port) {
    listensock = socket(AF_INET, SOCK_STREAM, 0);
    if (listensock == INVALID_SOCKET) return;

    int opt = 1;
    setsockopt(listensock, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));

    sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);

    if (::bind(listensock, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        closesocket(listensock);
        return;
    }

    listen(listensock, 3);
    Logger::Log("Server slusa na portu " + std::to_string(port) + " (Spreman za automatski prijem)");

    while (isRunning) {
        sockaddr_in client;
        int cLen = sizeof(sockaddr_in);
        SOCKET clientSock = accept(listensock, (struct sockaddr*)&client, &cLen);

        if (clientSock == INVALID_SOCKET) break;

        int nameLen = 0;
        if (recv(clientSock, (char*)&nameLen, sizeof(int), 0) <= 0) { closesocket(clientSock); continue; }

        std::vector<char> nameBuf(nameLen);
        recv(clientSock, nameBuf.data(), nameLen, 0);
        std::string fileName(nameBuf.begin(), nameBuf.end());

        long long fileSize = 0;
        recv(clientSock, (char*)&fileSize, sizeof(long long), 0);

        std::vector<uint8_t> fileData(fileSize);
        long long totalReceived = 0;
        while (totalReceived < fileSize) {
            int bytes = recv(clientSock, reinterpret_cast<char*>(fileData.data()) + totalReceived, (int)(fileSize - totalReceived), 0);
            if (bytes <= 0) break;
            totalReceived += bytes;
        }

        FileManager fm;
        std::string localPath = "Downloads/" + fileName;

        if (fm.UpisiFajl(fileData, localPath, true)) {
            Logger::Log("MREZA: Primljen fajl: " + fileName);

            if (fileName.find(".bin") != std::string::npos) {
                Logger::Log("MREZA: Detektovan .bin paket, pokrecem verifikaciju i dekripciju...");

                SecurityService ss;
                if (ss.OdpakujFajl(localPath, _sessionPassword)) {
                    Logger::Log("MREZA: Fajl uspesno dekriptovan u Downloads.");
                }
                else {
                    Logger::Log("MREZA: Dekripcija primljenog fajla nije uspela (Proveri lozinku ili integritet).");
                }
            }
        }
        closesocket(clientSock);
    }
}

bool NetworkService::posaljiFajl(const std::string& ipAdresa, int port, const std::string& putanjaDoFajla) {
    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == INVALID_SOCKET) return false;

    sockaddr_in server;
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    if (inet_pton(AF_INET, ipAdresa.c_str(), &server.sin_addr) <= 0) {
        Logger::Log("GRESKA: Nevalidna IP adresa!");
        return false;
    }

    if (connect(sock, (struct sockaddr*)&server, sizeof(server)) == SOCKET_ERROR) {
        Logger::Log("GRESKA: Ne mogu se povezati na " + ipAdresa);
        closesocket(sock);
        return false;
    }

    FileManager fm;
    std::vector<uint8_t> data;
    if (!fm.ProcitajFajl(data, putanjaDoFajla)) {
        closesocket(sock);
        return false;
    }

    std::string fileName = putanjaDoFajla.substr(putanjaDoFajla.find_last_of("/\\") + 1);
    int nameLen = (int)fileName.length();
    long long fileSize = (long long)data.size();

    send(sock, (char*)&nameLen, sizeof(int), 0);
    send(sock, fileName.c_str(), nameLen, 0);
    send(sock, (char*)&fileSize, sizeof(long long), 0);

    long long totalSent = 0;
    const char* ptr = reinterpret_cast<const char*>(data.data());

    while (totalSent < fileSize) {
        int sent = send(sock, ptr + totalSent, (int)(fileSize - totalSent), 0);
        if (sent == SOCKET_ERROR) {
            Logger::Log("GRESKA: Prekid slanja!");
            closesocket(sock);
            return false;
        }
        totalSent += sent;
    }

    Logger::Log("Fajl uspesno poslat: " + fileName);
    closesocket(sock);
    return true;
}