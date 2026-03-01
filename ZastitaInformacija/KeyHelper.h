#pragma once
#include <vector>
#include <string>
#include "picosha2.h"

struct KeyMaterial {
    std::vector<uint8_t> key64;    // 8 bajtova (za A5/2 KeySetup)
    std::vector<uint8_t> nonce160; // 20 bajtova (Fiksni deo za CTR)
    std::vector<uint8_t> fullHash; // 32 bajta (za Simple Substitution)
};

class KeyHelper {
public:
    static KeyMaterial Derive(std::string password) {
        std::vector<uint8_t> hash(picosha2::k_digest_size);
        picosha2::hash256(password.begin(), password.end(), hash.begin(), hash.end());

        KeyMaterial km;
        // 1. Key64 (0-7)
        km.key64.assign(hash.begin(), hash.begin() + 8);

        // 2. Nonce160 (8-27) - OVO JE NEDOSTAJALO
        km.nonce160.assign(hash.begin() + 8, hash.begin() + 28);

        // 3. FullHash (0-31)
        km.fullHash = hash;

        return km;
    }
};