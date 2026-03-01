#pragma once
#include<iostream>
#include<fstream>
#include<string>
#include <vector>
#include <filesystem>
#include <algorithm>
using namespace std;
class FileManager
{
public:
	FileManager() {}

	bool UpisiFajl(const std::vector<uint8_t>& tekstZaUpis, const std::string& imeFajla, bool binarni = true);
	bool ProcitajFajl(std::vector<uint8_t>& procitaniTekst, const std::string& imeFajla, bool binarni = true);
	static bool DaLiJeTekstualniPoEkstenziji(const std::string& putanja);
};

