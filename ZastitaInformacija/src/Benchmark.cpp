#include "Benchmark.h"
#include "A52.h"
#include "MD5.h"
#include "SimpleSubstitution.h"
#include "KeyHelper.h"

void Benchmark::RunAll(const std::string& lozinka) {
    
    std::vector<uint8_t> testData(10 * 1024 * 1024, 0xAA);

    std::cout << "\n--- STARTING AUTOMATED BENCHMARK (10MB DATA) ---\n";
    TestSimpleSubstitution(testData, lozinka);
    TestA52CTR(testData, lozinka);
    TestMD5(testData);
    std::cout << "--- BENCHMARK COMPLETED ---\n";
}

void Benchmark::TestA52CTR(const std::vector<uint8_t>& data, const std::string& lozinka) {
    auto podaci = data;
    A52 a52;
    KeyMaterial km = KeyHelper::Derive(lozinka);
    a52.PostaviKljuc(km.key64);

    auto start = std::chrono::high_resolution_clock::now();
    a52.Inicijalizuj(0); 
    a52.Kodiraj(podaci);
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> diff = end - start;
    double mbps = 10.0 / diff.count();
    std::cout << "A5/2 :       " << std::fixed << std::setprecision(2) << mbps << " MB/s\n";
}

void Benchmark::TestMD5(const std::vector<uint8_t>& data) {
    MD5 md5;
    auto start = std::chrono::high_resolution_clock::now();
    md5.izracunaj(data);
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> diff = end - start;
    double mbps = 10.0 / diff.count();
    std::cout << "MD5 Hashing:         " << std::fixed << std::setprecision(2) << mbps << " MB/s\n";
}

void Benchmark::TestSimpleSubstitution(const std::vector<uint8_t>& data, const std::string& lozinka) {
    auto podaci = data;
    SimpleSubstitution ss;
    KeyMaterial km = KeyHelper::Derive(lozinka);
    ss.GenerisiIzLozinke(km.fullHash);

    auto start = std::chrono::high_resolution_clock::now();
    ss.Kodiraj(podaci);
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double> diff = end - start;
    double mbps = 10.0 / diff.count();
    std::cout << "Simple Substitution: " << std::fixed << std::setprecision(2) << mbps << " MB/s\n";
}