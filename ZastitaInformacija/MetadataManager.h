#pragma once
#include <string>
#include <vector>
#include <filesystem>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
namespace fs = std::filesystem;

class MetadataManager {
public:
    static std::string kreirajHeader(const std::string& putanjaDoFajla, const std::string& korisceniAlgoritam);
    static std::string kreirajTrailer(const std::string& md5Hash, const std::string& md5HashKodiran);
private:
    static std::string dobijVreme(const fs::path& p);
};