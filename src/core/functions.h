#pragma once

#include <iostream>
#include <cinttypes>
#include <vector>
#include <memory>
#include <cmath>
#include <cstring>
#include <chrono>
#include <fstream>
#include <random>
#include <algorithm>
#include <unordered_map>
#include <variant>

#define TIME_PROF(log, exec, unit)                                                                                                             \
    do                                                                                                                                         \
    {                                                                                                                                          \
        std::string str_unit = std::string(unit);                                                                                              \
        float a = 1;                                                                                                                           \
        if (str_unit == std::string("s"))                                                                                                      \
        {                                                                                                                                      \
            a = 1e-9;                                                                                                                          \
        }                                                                                                                                      \
        else if (str_unit == std::string("ms"))                                                                                                \
        {                                                                                                                                      \
            a = 1e-6;                                                                                                                          \
        }                                                                                                                                      \
        else if (str_unit == std::string("us"))                                                                                                \
        {                                                                                                                                      \
            a = 1e-3;                                                                                                                          \
        }                                                                                                                                      \
        else if (str_unit == std::string("ns"))                                                                                                \
        {                                                                                                                                      \
            a = 1;                                                                                                                             \
        }                                                                                                                                      \
        else                                                                                                                                   \
        {                                                                                                                                      \
            a = 1;                                                                                                                             \
            str_unit = std::string("ns");                                                                                                      \
        }                                                                                                                                      \
        auto start = std::chrono::high_resolution_clock::now();                                                                                \
        exec;                                                                                                                                  \
        auto elapsed = std::chrono::high_resolution_clock::now() - start;                                                                      \
        printf("[TIMEPROF]: " log ": ");                                                                                                       \
        printf("%.3f %s\n", static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(elapsed).count()) * a, str_unit.c_str()); \
    } while (0);

namespace ldpc
{
    /**
    * @brief Define outstream operator for vector to ease debugging.
    * 
    * @tparam T 
    * @param os 
    * @param v 
    * @return std::ostream& 
    */
    template <class T>
    std::ostream &operator<<(std::ostream &os, const std::vector<T> &v)
    {
        os << "[";
        typename std::vector<T>::const_iterator it = v.begin();
        for (; it != v.end(); ++it)
        {
            os << *it;
            if (it != v.end() - 1)
            {
                os << ", ";
            }
        }
        os << "]";
        return os;
    }

    using bits_t = int;
    using u64 = unsigned long;
    using u32 = unsigned int;

    using vec_bits_t = std::vector<bits_t>;
    using vec_u64 = std::vector<u64>;
    using vec_double_t = std::vector<double>;

    using mat_bits_t = std::vector<std::vector<bits_t>>;
    using mat_u64 = std::vector<std::vector<u64>>;
    using mat_double_t = std::vector<std::vector<double>>;

    struct
    {
        bool earlyTerm;
        u32 iterations;
        std::string type;
    } typedef decoder_param;

    struct
    {
        u64 seed;
        vec_double_t xRange;
        vec_double_t xVals;
        std::string type;
    } typedef channel_param;

    struct
    {
        u32 threads;
        u64 maxFrames;
        u64 fec;
        std::string resultFile;
    } typedef simulation_param;

    std::ostream &operator<<(std::ostream &os, const decoder_param &p);
    std::ostream &operator<<(std::ostream &os, const channel_param &p);
    std::ostream &operator<<(std::ostream &os, const simulation_param &p);

    void dec2bin(u64 val, uint8_t m);
    int sign(double a);

} // namespace ldpc
