#define _CRT_SECURE_NO_WARNINGS
#include "MetadataManager.h"
#include <sstream> // Za std::stringstream
#include <iomanip> // Za std::put_time
#include <chrono>  // Za rad sa vremenom
#include <ctime>   // Za std::localtime i std::time_t


std::string MetadataManager::kreirajHeader(const std::string& putanjaDoFajla, const std::string& korisceniAlgoritam)
{
    std::filesystem::path p(putanjaDoFajla);

    if (!std::filesystem::exists(p)) {
        return ""; 
    }
    json j;

    j["fileName"] = p.filename().string();    
    j["fileSize"] = (long long)std::filesystem::file_size(p); 
    j["dateOfCreation"] = dobijVreme(p);             
    j["cipherAlgorithm"] = korisceniAlgoritam;       
    j["hashAlgorithm"] = "MD5";                      
 
    return j.dump(4);
}

std::string MetadataManager::kreirajTrailer(const std::string& md5Hash, const std::string& md5HashKodiran)
{
    json j;
    j["originalHash"] = md5Hash;
    j["encryptedHash"] = md5HashKodiran;
    return j.dump();
}
std::string MetadataManager::dobijVreme(const std::filesystem::path& p) {
 
    auto sada = std::chrono::system_clock::now();
    std::time_t ctime = std::chrono::system_clock::to_time_t(sada);

    std::tm gmt;
    localtime_s(&gmt, &ctime);

    std::stringstream ss;
    ss << std::put_time(&gmt, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}
