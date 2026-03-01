#include "FSWatcher.h"

FSWatcher::FSWatcher() {
    this->hDir = INVALID_HANDLE_VALUE;
    this->prekidac = false;
    this->odabraniAlgoritam = 2;
    this->_lozinka = "";
}

FSWatcher::~FSWatcher() {
    stop();
}

void FSWatcher::start(const string& path, int algoritam, const string& lozinka) {
    if (prekidac) return;

    this->targetFolder = path;
    this->odabraniAlgoritam = algoritam;
    this->_lozinka = lozinka; 

    this->hDir = CreateFileA(
        targetFolder.c_str(),
        FILE_LIST_DIRECTORY,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS,
        NULL
    );

    if (this->hDir == INVALID_HANDLE_VALUE) {
        Logger::Log("GRESKA: Ne mogu da pristupim Target folderu: " + targetFolder);
        return;
    }

    this->prekidac = true;
    this->watchThread = thread(&FSWatcher::watchLoop, this);
    Logger::Log("FSW: Servis pokrenut sa unetom lozinkom.");
}

void FSWatcher::watchLoop() {
    char buffer[2048];
    DWORD bytesReturned;

    while (prekidac) {
        if (ReadDirectoryChangesW(hDir, buffer, sizeof(buffer), FALSE,
            FILE_NOTIFY_CHANGE_FILE_NAME,
            &bytesReturned, NULL, NULL))
        {
            FILE_NOTIFY_INFORMATION* fni = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(buffer);
            do {
                wstring wIme(fni->FileName, fni->FileNameLength / sizeof(wchar_t));
                string imeFajla(wIme.begin(), wIme.end());
                if (fni->Action == FILE_ACTION_ADDED && imeFajla.find(".bin") == string::npos) {
                    this_thread::sleep_for(std::chrono::milliseconds(800));

                    SecurityService ss;
                    string punaPutanja = targetFolder + "\\" + imeFajla;
                    if (ss.ZastitiFajl(punaPutanja, odabraniAlgoritam, _lozinka)) {
                        Logger::Log("FSW: Automatski obradjen fajl: " + imeFajla);
                    }
                }

                if (fni->NextEntryOffset == 0) break;
                fni = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(reinterpret_cast<BYTE*>(fni) + fni->NextEntryOffset);
            } while (true);
        }
        else {
            break;
        }
    }
}

void FSWatcher::stop() {
    if (!prekidac) return;
    prekidac = false;

    if (hDir != INVALID_HANDLE_VALUE) {
        CancelIoEx(hDir, NULL);
        CloseHandle(hDir);
        hDir = INVALID_HANDLE_VALUE;
    }

    if (watchThread.joinable()) {
        watchThread.join();
    }
    this->_lozinka = ""; 
    Logger::Log("FSW servis zaustavljen.");
}