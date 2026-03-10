#include "FileManager.h"

bool FileManager::UpisiFajl(const std::vector<uint8_t>& tekstZaUpis, const std::string& imeFajla, bool binarni)
{
    if (binarni)
    {
        std::fstream file(imeFajla, std::ios::binary | std::ios::out);

        if (!file.is_open())
        {
            std::cerr << "Ne mogu da otvorim binarni fajl!" << std::endl;
            return false;
        }

        file.write(reinterpret_cast<const char*>(tekstZaUpis.data()), tekstZaUpis.size());

        file.close();
    }
    else
    {
        std::fstream file(imeFajla, std::ios::out);

        if (!file.is_open()) {
            std::cerr << "Ne mogu da otvorim tekstualni fajl!" << std::endl;
            return false;
        }

        file.write(reinterpret_cast<const char*>(tekstZaUpis.data()), tekstZaUpis.size());

        file.close();
    }
    return true;
}

bool FileManager::ProcitajFajl(std::vector<uint8_t>& procitaniTekst, const std::string& imeFajla, bool binarni)
{

    if (binarni)
    {
        std::fstream file(imeFajla, std::ios::binary | std::ios::in);

        if (!file.is_open()) {
            std::cerr << "Ne mogu da otvorim binarni fajl!" << std::endl;
            return false;
        }

        file.seekg(0, std::ios::end);
        size_t velicinaFajla = file.tellg();

        file.seekg(0, std::ios::beg);
        procitaniTekst.resize(velicinaFajla);

        if (velicinaFajla > 0) {
            file.read(reinterpret_cast<char*>(procitaniTekst.data()), velicinaFajla);
        }

        file.close();
        return true;
    }
    else
    {
        std::fstream file(imeFajla, std::ios::in);

        if (!file.is_open()) {
            std::cerr << "Ne mogu da otvorim tekstualni fajl!" << std::endl;
            return false;
        }

        file.seekg(0, std::ios::end);
        size_t velicinaFajla = file.tellg();

        file.seekg(0, std::ios::beg);
        procitaniTekst.resize(velicinaFajla);

        if (velicinaFajla > 0) {
            file.read(reinterpret_cast<char*>(procitaniTekst.data()), velicinaFajla);
        }

        file.close();
    }

    return true;
}

bool FileManager::DaLiJeTekstualniPoEkstenziji(const std::string& putanja)
{
    std::filesystem::path p(putanja);
    std::string ext = p.extension().string();

    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    if (ext == ".txt" || ext == ".log" || ext == ".csv" || ext == ".json" || ext == ".xml") {
        return true;
    }
    return false;
}
