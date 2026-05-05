#pragma once
#include <chrono>
#include <vector>
#include <string>
#include <iostream>
#include <iomanip>

class Benchmark {
public:
    static void RunAll(const std::string& lozinka);
private:
    static void TestA52CTR(const std::vector<uint8_t>& data, const std::string& lozinka);
    static void TestMD5(const std::vector<uint8_t>& data);
    static void TestSimpleSubstitution(const std::vector<uint8_t>& data, const std::string& lozinka);
};