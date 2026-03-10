#define _CRT_SECURE_NO_WARNINGS
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX 

#include <winsock2.h>
#include <windows.h> 
#include <iostream>
#include <string>
#include <filesystem>

#include "NetworkService.h" 
#include "FSWatcher.h"
#include "SecurityService.h"
#include "Logger.h"
#include "FileManager.h"
#include "A52.h"
using namespace std;
//namespace fs = std::filesystem;


void PrikaziMeni(int trenutniAlgoritam) {
    cout << "\n--- MENI (Indeks 19089) ---\n";
    cout << "Trenutni algoritam: ";
    switch (trenutniAlgoritam) {
    case 1: cout << "Simple Substitution\n"; break;
    case 2: cout << "A5/2 (CTR Mode)\n"; break;
    case 3: cout << "A5/2 (Standard Stream)\n"; break;
    }
    cout << "1. Pokreni FSW (Automatska zastita)\n";
    cout << "2. Zaustavi FSW\n";
    cout << "3. Promeni algoritam\n";
    cout << "4. Rucna enkripcija fajla\n";
    cout << "5. Rucna dekripcija .bin fajla\n";
    cout << "6. Pokreni Server (Automatski prijem i dekripcija)\n";
    cout << "7. Posalji fajl preko mreze\n";
    cout << "8. Promeni lozinku sesije\n";
    cout << "0. Izlaz\n";
    cout << "Izbor: ";
}

int main() {
    string targetFolder = "Target";
    string outputFolder = "Zasticeni";
    string downloadsFolder = "Downloads";

    if (!fs::exists(targetFolder)) fs::create_directory(targetFolder);
    if (!fs::exists(outputFolder)) fs::create_directory(outputFolder);
    if (!fs::exists(downloadsFolder)) fs::create_directory(downloadsFolder);

    FSWatcher fsw;
    NetworkService network;
    SecurityService security;

    string globalnaLozinka;
    cout << "=== Inicijalizacija sistema (Sync sa kolegom) ===\n";
    cout << "Unesite lozinku sesije: ";
    getline(cin, globalnaLozinka);

    int trenutniAlgoritam = 2; 
    bool fswAktivan = false;
    bool serverAktivan = false;
    int izbor = -1;

    Logger::Log("POCETAK NOVE SESIJE ----------------------------");

    while (izbor != 0) {
        cout << "\n[Status: FSW=" << (fswAktivan ? "ON" : "OFF")
            << " | Server=" << (serverAktivan ? "ON" : "OFF") << "]";
        cout << "\n[Lozinka: " << (globalnaLozinka.empty() ? "Nije postavljena" : "********") << "]";

        PrikaziMeni(trenutniAlgoritam);

        if (!(cin >> izbor)) {
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }
        cin.ignore(); 

        switch (izbor) {
        case 1: 
            if (!fswAktivan) {
                fsw.start(targetFolder, trenutniAlgoritam, globalnaLozinka);
                fswAktivan = true;
            }
            break;

        case 2: 
            if (fswAktivan) {
                fsw.stop();
                fswAktivan = false;
            }
            break;

        case 3: 
            cout << "Unesite (1: SS, 2: A5/2 CTR, 3: A5/2 Std): ";
            int noviAlg;
            if (cin >> noviAlg && noviAlg >= 1 && noviAlg <= 3) {
                trenutniAlgoritam = noviAlg;
                if (fswAktivan) {
                    fsw.stop();
                    fsw.start(targetFolder, trenutniAlgoritam, globalnaLozinka);
                    cout << "FSW restartovan sa novim algoritmom.\n";
                }
            }
            break;

        case 4: 
        {
            cout << "Putanja do fajla za enkripciju: ";
            string p; getline(cin, p);
            if (fs::exists(p)) {
                security.ZastitiFajl(p, trenutniAlgoritam, globalnaLozinka);
            }
            else {
                cout << "Fajl ne postoji.\n";
            }
            break;
        }

        case 5: 
        {
            cout << "Putanja do .bin fajla: ";
            string p; getline(cin, p);
            if (fs::exists(p)) {
                security.OdpakujFajl(p, globalnaLozinka);
            }
            else {
                cout << "Fajl ne postoji.\n";
            }
            break;
        }

        case 6: 
        {
            int port;
            cout << "Unesite port za server: "; cin >> port;
            network.pokreniServer(port, globalnaLozinka);
            serverAktivan = true;
            break;
        }

        case 7: 
        {
            string ip, fajl; int port;
            cout << "IP adresa primaoca: "; cin >> ip;
            cout << "Port: "; cin >> port;
            cin.ignore();
            cout << "Putanja do fajla: "; getline(cin, fajl);
            network.posaljiFajl(ip, port, fajl);
            break;
        }

        case 8: 
        {
            cout << "Unesite novu globalnu lozinku: ";
            getline(cin, globalnaLozinka);
            if (fswAktivan) {
                fsw.stop();
                fsw.start(targetFolder, trenutniAlgoritam, globalnaLozinka);
            }
            if (serverAktivan) {
                cout << "Server ce koristiti novu lozinku za naredne fajlove.\n";
            }
            break;
        }

        case 0:
            fsw.stop();
            network.zaustaviServer();
            Logger::Log("Program uspesno ugasen.");
            break;
        }
    }
    return 0;
}