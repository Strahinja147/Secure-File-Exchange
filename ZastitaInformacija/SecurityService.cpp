#include "SecurityService.h"
#include <filesystem>
namespace fs = std::filesystem;


SecurityService::SecurityService()
{

}

bool SecurityService::ZastitiFajl(const string& imeFajla, int algoritamZaKodiranje, string lozinka)
{
    FileManager fileManager;
    MD5 md5;
    vector<uint8_t> podaci;
    string md5Hesh;
    string md5HeshKodirani;

    std::filesystem::path putanja(imeFajla);
    string samoImeFajla = putanja.filename().string();
    string folderX = "Zasticeni";

    if (!std::filesystem::exists(folderX)) {
        std::filesystem::create_directory(folderX);
    }

    string izlazniBin = folderX + "/" + samoImeFajla + ".bin";

    bool binarniMod = !FileManager::DaLiJeTekstualniPoEkstenziji(imeFajla);

    if (!fileManager.ProcitajFajl(podaci, imeFajla, binarniMod))
    {
        Logger::Log("Nije uspesno procitan fajl: " + imeFajla + "!");
        return false;
    }

    Logger::Log("Fajl " + samoImeFajla + " uspesno procitan.");

    md5Hesh = md5.izracunaj(podaci);

    
    KeyMaterial km = KeyHelper::Derive(lozinka);
    string algoname;

    if (algoritamZaKodiranje == 1) 
    {
        algoname = "SimpleSubstitution";
        SimpleSubstitution ss;
        
        ss.GenerisiIzLozinke(km.fullHash);
        ss.Kodiraj(podaci);
        Logger::Log("Kodirano: Simple Substitution.");
    }
    else if (algoritamZaKodiranje == 2) // A5/2 CTR
    {
        algoname = "A52";
        A52 a52;
        
        a52.PostaviKljuc(km.key64);
        
        a52.KodirajUCTRModu(podaci, km.nonce160);
        Logger::Log("Kodirano: A5/2 CTR mod.");
    }
    else if (algoritamZaKodiranje == 3) // A5/2 Standard
    {
        algoname = "A52_Standard";
        A52 a52;
        a52.PostaviKljuc(km.key64);
        a52.Inicijalizuj(0); 
        a52.Kodiraj(podaci);
        Logger::Log("Kodirano: A5/2 Standard Stream.");
    }

    md5HeshKodirani = md5.izracunaj(podaci);

    string headerJSON = MetadataManager::kreirajHeader(imeFajla, algoname);
    string trailerJSON = MetadataManager::kreirajTrailer(md5Hesh, md5HeshKodirani);

    vector<uint8_t> finalniPaket;

    uint32_t headerLen = (uint32_t)headerJSON.size();
    uint8_t* hLenPtr = reinterpret_cast<uint8_t*>(&headerLen);
    for (int i = 0; i < 4; i++) finalniPaket.push_back(hLenPtr[i]);
    finalniPaket.insert(finalniPaket.end(), headerJSON.begin(), headerJSON.end());

    finalniPaket.insert(finalniPaket.end(), podaci.begin(), podaci.end());

    uint32_t trailerLen = (uint32_t)trailerJSON.size();
    uint8_t* tLenPtr = reinterpret_cast<uint8_t*>(&trailerLen);
    for (int i = 0; i < 4; i++) finalniPaket.push_back(tLenPtr[i]);
    finalniPaket.insert(finalniPaket.end(), trailerJSON.begin(), trailerJSON.end());

    if (!fileManager.UpisiFajl(finalniPaket, izlazniBin, true))
    {
        Logger::Log("Greska pri upisu zasticenog fajla: " + izlazniBin);
        return false;
    }

    Logger::Log("Uspesno zasticeno: " + samoImeFajla + ".bin");
    return true;
}
bool SecurityService::OdpakujFajl(const string& imeFajla, string lozinka)
{
    FileManager fileManager;
    MD5 md5;
    vector<uint8_t> procitaniPaket;

    if (!fileManager.ProcitajFajl(procitaniPaket, imeFajla, true)) return false;

    if (procitaniPaket.size() < 12) {
        Logger::Log("Greska: Paket je premali ili ostecen.");
        return false;
    }

    uint32_t headerLen;
    memcpy(&headerLen, &procitaniPaket[0], 4);

    string headerJsonStr(procitaniPaket.begin() + 4, procitaniPaket.begin() + 4 + headerLen);
    string originalniNaziv, korisceniAlgoritam;
    long long fileSize;

    try {
        auto jHead = nlohmann::json::parse(headerJsonStr);
        originalniNaziv = jHead["fileName"];
        korisceniAlgoritam = jHead["cipherAlgorithm"];
        fileSize = jHead["fileSize"];
    }
    catch (...) {
        Logger::Log("Greska: Nevalidan JSON Heder u paketu.");
        return false;
    }

    size_t startOfData = 4 + headerLen;
    if (procitaniPaket.size() < startOfData + fileSize + 4) {
        Logger::Log("Greska: Paket je kraci nego sto heder navodi.");
        return false;
    }
    vector<uint8_t> podaci(procitaniPaket.begin() + startOfData, procitaniPaket.begin() + startOfData + fileSize);

    size_t startOfTrailerSize = startOfData + fileSize;
    uint32_t trailerLen;
    memcpy(&trailerLen, &procitaniPaket[startOfTrailerSize], 4);

    string trailerJsonStr(procitaniPaket.begin() + startOfTrailerSize + 4, procitaniPaket.end());
    string sacuvaniMD5, sacuvaniMD5Kodiran;

    try {
        auto jTrail = nlohmann::json::parse(trailerJsonStr);
        sacuvaniMD5 = jTrail["originalHash"];
        sacuvaniMD5Kodiran = jTrail["encryptedHash"];
    }
    catch (...) {
        Logger::Log("Greska: Nevalidan JSON Trailer u paketu.");
        return false;
    }

    string trenutniMD5Kodirani = md5.izracunaj(podaci);
    if (trenutniMD5Kodirani != sacuvaniMD5Kodiran)
    {
        Logger::Log("GRESKA: MD5 kriptovanih podataka se ne poklapa! Paket je ostecen.");
        return false;
    }
    Logger::Log("Integritet kriptovanog paketa potvrdjen.");

    KeyMaterial km = KeyHelper::Derive(lozinka);

    if (korisceniAlgoritam == "SimpleSubstitution")
    {
        SimpleSubstitution ss;
        ss.GenerisiIzLozinke(km.fullHash);
        ss.Dekodiraj(podaci);
        Logger::Log("Dekriptovano koriscenjem Simple Substitution.");
    }
    else if (korisceniAlgoritam == "A52")
    {
        A52 a52;
        a52.PostaviKljuc(km.key64);
        a52.KodirajUCTRModu(podaci, km.nonce160);
        Logger::Log("Dekriptovano koriscenjem A5/2 CTR.");
    }
    else if (korisceniAlgoritam == "A52_Standard")
    {
        A52 a52;
        a52.PostaviKljuc(km.key64);
        a52.Inicijalizuj(0);
        a52.Kodiraj(podaci);
        Logger::Log("Dekriptovano koriscenjem A5/2 Standard.");
    }

    string finalniMD5 = md5.izracunaj(podaci);
    if (finalniMD5 == sacuvaniMD5)
    {
        Logger::Log("MD5 originala se poklapa. Lozinka je ispravna.");

        bool originalBioBinarni = !FileManager::DaLiJeTekstualniPoEkstenziji(originalniNaziv);

        std::filesystem::path p(imeFajla);
        string izlaznaPutanja = (p.parent_path() / ("DEKRIPTOVANO_" + originalniNaziv)).string();

        if (!fileManager.UpisiFajl(podaci, izlaznaPutanja, originalBioBinarni))
        {
            Logger::Log("Greska pri upisu dekriptovanog fajla.");
            return false;
        }
        Logger::Log("Fajl uspesno sacuvan: " + izlaznaPutanja);
    }
    else
    {
        Logger::Log("GRESKA: MD5 originala mismatch! Mozda je pogresna lozinka?");
        return false;
    }

    return true;
}

