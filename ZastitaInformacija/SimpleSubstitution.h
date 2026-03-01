#pragma once
#include <vector>
#include <iostream>
#include <string>
#include <numeric>
#include <algorithm>
#include <cstdint>

class SimpleSubstitution
{
private:
    std::vector<uint8_t> kljuc;
    std::vector<uint8_t> inverzniKljuc;
    void azurirajInverzniKljuc();

public:
    
    SimpleSubstitution();

    
    void GenerisiIzLozinke(const std::vector<uint8_t>& seed);

    void Kodiraj(std::vector<uint8_t>& tekstZaKodiranje);
    void Dekodiraj(std::vector<uint8_t>& tekstZaDekodiranje);
    void PostaviKljuc(const std::vector<uint8_t>& noviKljuc);
};