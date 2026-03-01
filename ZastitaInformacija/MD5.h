#pragma once
#include <vector>
#include <string>
#include <cstdint>
#include <iomanip>
#include <sstream>

class MD5 {
private:
    
    uint32_t h0, h1, h2, h3;

    inline uint32_t F(uint32_t x, uint32_t y, uint32_t z) { return (x & y) | (~x & z); }
    inline uint32_t G(uint32_t x, uint32_t y, uint32_t z) { return (x & z) | (y & ~z); }
    inline uint32_t H(uint32_t x, uint32_t y, uint32_t z) { return x ^ y ^ z; }
    inline uint32_t I(uint32_t x, uint32_t y, uint32_t z) { return y ^ (x | ~z); }

    inline uint32_t leftRotate(uint32_t x, uint32_t c) {
        return (x << c) | (x >> (32 - c));
    }

    void reset();

    void transformisiBlok(const uint8_t* blok);

    std::vector<uint8_t> dodajPadding(std::vector<uint8_t> podaci);

    std::string formatirajHes();

public:
    MD5();

    std::string izracunaj(const std::vector<uint8_t>& podaci);
};