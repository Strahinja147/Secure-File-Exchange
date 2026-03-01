#pragma once
#include <vector>
#include <cstdint>
#include <random>

class A52
{
private:
    uint32_t delaybit; // U knjizi je 'static bit delaybit'
    uint32_t R1, R2, R3, R4;
    std::vector<uint8_t> sesijskiKljuc;

   
    int GetBit(uint32_t registar, int pozicija);
    int Majority(uint32_t w1, uint32_t w2, uint32_t w3);
    uint32_t Parity(uint32_t x);

    
    uint32_t ClockOne(uint32_t reg, uint32_t mask, uint32_t taps, uint32_t loaded_bit);
    void ClockAll(bool allP, bool loaded);

public:
    A52();
    void PostaviKljuc(const std::vector<uint8_t>& noviKljuc);
    std::vector<uint8_t> GenerisiNasumicanKljuc();

    void Inicijalizuj(uint32_t frejm);
    uint8_t GenerisiSledeciBit();

    void Kodiraj(std::vector<uint8_t>& podaci);
    /*void KodirajUCTRModu(std::vector<uint8_t>& podaci);*/
    void KodirajUCTRModu(std::vector<uint8_t>& podaci, const std::vector<uint8_t>& nonce);
};