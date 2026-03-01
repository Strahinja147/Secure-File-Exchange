#include "A52.h"

const uint32_t R1MASK = 0x07FFFF;
const uint32_t R2MASK = 0x3FFFFF;
const uint32_t R3MASK = 0x7FFFFF;
const uint32_t R4MASK = 0x01FFFF;

const uint32_t R1TAPS = 0x072000; // bits 18,17,16,13
const uint32_t R2TAPS = 0x300000; // bits 21,20
const uint32_t R3TAPS = 0x700080; // bits 22,21,20,7
const uint32_t R4TAPS = 0x010800; // bits 16,11

const uint32_t R4TAP1 = 0x000400; // bit 10
const uint32_t R4TAP2 = 0x000008; // bit 3
const uint32_t R4TAP3 = 0x000080; // bit 7

A52::A52() : R1(0), R2(0), R3(0), R4(0), delaybit(0) {}

int A52::GetBit(uint32_t registar, int pozicija) {
    return (registar >> pozicija) & 1;
}


uint32_t A52::Parity(uint32_t x) {
    x ^= x >> 16;
    x ^= x >> 8;
    x ^= x >> 4;
    x ^= x >> 2;
    x ^= x >> 1;
    return x & 1;
}


int A52::Majority(uint32_t w1, uint32_t w2, uint32_t w3) {
    int sum = (w1 != 0) + (w2 != 0) + (w3 != 0);
    return (sum >= 2) ? 1 : 0;
}


uint32_t A52::ClockOne(uint32_t reg, uint32_t mask, uint32_t taps, uint32_t loaded_bit) {
    uint32_t t = reg & taps;
    reg = (reg << 1) & mask;
    reg |= Parity(t);
    reg |= loaded_bit; 
    return reg;
}


void A52::ClockAll(bool allP, bool loaded) {
    int maj = Majority(R4 & R4TAP1, R4 & R4TAP2, R4 & R4TAP3);

    if (allP || (((R4 & R4TAP1) != 0) == maj))
        R1 = ClockOne(R1, R1MASK, R1TAPS, (uint32_t)loaded << 15);

    if (allP || (((R4 & R4TAP2) != 0) == maj))
        R2 = ClockOne(R2, R2MASK, R2TAPS, (uint32_t)loaded << 16);

    if (allP || (((R4 & R4TAP3) != 0) == maj))
        R3 = ClockOne(R3, R3MASK, R3TAPS, (uint32_t)loaded << 18);

    R4 = ClockOne(R4, R4MASK, R4TAPS, (uint32_t)loaded << 10);
}

void A52::Inicijalizuj(uint32_t frejm) {
    R1 = R2 = R3 = R4 = 0;
    delaybit = 0;

    
    for (int i = 0; i < 64; i++) {
        ClockAll(true, false);
        uint8_t keybit = (sesijskiKljuc[i / 8] >> (i % 8)) & 1;
        R1 ^= keybit; R2 ^= keybit; R3 ^= keybit; R4 ^= keybit;
    }

   
    for (int i = 0; i < 22; i++) {
        bool isLast = (i == 21);
        ClockAll(true, isLast); 
        uint8_t framebit = (frejm >> i) & 1;
        R1 ^= framebit; R2 ^= framebit; R3 ^= framebit; R4 ^= framebit;
    }

    
    for (int i = 0; i < 100; i++) {
        ClockAll(false, false);
    }

    
    GenerisiSledeciBit();
}

uint8_t A52::GenerisiSledeciBit() {
    uint8_t trenutniIzlaz = (uint8_t)delaybit;

    
    uint8_t topbits = ((R1 >> 18) ^ (R2 >> 21) ^ (R3 >> 22)) & 0x01;

    uint8_t m1 = Majority(R1 & 0x8000, (~R1) & 0x4000, R1 & 0x1000);
    uint8_t m2 = Majority((~R2) & 0x10000, R2 & 0x2000, R2 & 0x200);
    uint8_t m3 = Majority(R3 & 0x40000, R3 & 0x10000, (~R3) & 0x2000);

    delaybit = (topbits ^ m1 ^ m2 ^ m3) & 1;

    
    ClockAll(false, false);

    return trenutniIzlaz;
}

void A52::Kodiraj(std::vector<uint8_t>& podaci) {
    for (size_t i = 0; i < podaci.size(); i++) {
        uint8_t bajtRes = 0;
        for (int bitPos = 0; bitPos < 8; bitPos++) {
            uint8_t k = GenerisiSledeciBit();
            uint8_t p = (podaci[i] >> (7 - bitPos)) & 1;
            bajtRes |= ((p ^ k) << (7 - bitPos));
        }
        podaci[i] = bajtRes;
    }
}

void A52::KodirajUCTRModu(std::vector<uint8_t>& podaci, const std::vector<uint8_t>& nonce) {
    uint32_t frejm = 0;
    uint64_t ctr = 0;

    for (size_t i = 0; i < podaci.size(); i += 28) {
        this->Inicijalizuj(frejm++);

        std::vector<uint8_t> ulazniBlok(28, 0);
        std::copy(nonce.begin(), nonce.end(), ulazniBlok.begin());
        for (int k = 0; k < 8; k++) ulazniBlok[20 + k] = (uint8_t)(ctr >> (k * 8));
        ctr++;

        std::vector<uint8_t> keystream(28, 0);
        for (int j = 0; j < 28; j++) {
            if (j == 14) { 
                GenerisiSledeciBit(); GenerisiSledeciBit();
            }
            uint8_t k_bajt = 0;
            for (int bitPos = 0; bitPos < 8; bitPos++) {
                k_bajt |= (GenerisiSledeciBit() << (7 - bitPos));
            }
            
            keystream[j] = ulazniBlok[j] ^ k_bajt;
        }

        for (size_t j = 0; j < 28 && (i + j) < podaci.size(); j++) {
            podaci[i + j] ^= keystream[j];
        }
    }
}


void A52::PostaviKljuc(const std::vector<uint8_t>& noviKljuc) {
    if (noviKljuc.size() == 8) this->sesijskiKljuc = noviKljuc;
}

std::vector<uint8_t> A52::GenerisiNasumicanKljuc() {
    std::vector<uint8_t> noviKljuc(8);
    std::random_device rd;
    std::mt19937 g(rd());
    std::uniform_int_distribution<int> dist(0, 255);
    for (int i = 0; i < 8; i++) noviKljuc[i] = static_cast<uint8_t>(dist(g));
    return noviKljuc;
}