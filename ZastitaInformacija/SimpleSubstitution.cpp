#include "SimpleSubstitution.h"

SimpleSubstitution::SimpleSubstitution()
{
    this->kljuc.resize(256);
    this->inverzniKljuc.resize(256);
}

void SimpleSubstitution::azurirajInverzniKljuc()
{
    for (int i = 0; i < 256; i++) {
        uint8_t sifrovaniBajt = this->kljuc[i];
        inverzniKljuc[sifrovaniBajt] = (uint8_t)i;
    }
}

void SimpleSubstitution::GenerisiIzLozinke(const std::vector<uint8_t>& seed)
{
    
    for (int i = 0; i < 256; i++) {
        this->kljuc[i] = (uint8_t)i;
    }

    for (int i = 0; i < 256; i++)
    {
        int j = seed[i % seed.size()] % 256;

        uint8_t temp = this->kljuc[i];
        this->kljuc[i] = this->kljuc[j];
        this->kljuc[j] = temp;
    }

    azurirajInverzniKljuc();
}

void SimpleSubstitution::Kodiraj(std::vector<uint8_t>& tekstZaKodiranje)
{
    for (size_t i = 0; i < tekstZaKodiranje.size(); i++)
    {
        tekstZaKodiranje[i] = kljuc[tekstZaKodiranje[i]];
    }
}

void SimpleSubstitution::Dekodiraj(std::vector<uint8_t>& tekstZaDekodiranje)
{
    for (size_t i = 0; i < tekstZaDekodiranje.size(); i++)
    {
        tekstZaDekodiranje[i] = inverzniKljuc[tekstZaDekodiranje[i]];
    }
}

void SimpleSubstitution::PostaviKljuc(const std::vector<uint8_t>& noviKljuc) {
    if (noviKljuc.size() == 256) {
        this->kljuc = noviKljuc;
        this->azurirajInverzniKljuc();
    }
    else {
        std::cerr << "Greska: Kljuc mora imati 256 bajtova!" << std::endl;
    }
}