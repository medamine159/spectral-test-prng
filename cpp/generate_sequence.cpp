// cpp/generate_sequence.cpp
#include <iostream>
#include <fstream>
#include <string>
#include <random>
#include <cstdint>

enum class PRNGType {
    LCG,
    RANDU,
    XORSHIFT32,
    MT19937
};

PRNGType parse_type(const std::string &name) {
    if (name == "lcg") return PRNGType::LCG;
    if (name == "randu") return PRNGType::RANDU;
    if (name == "xorshift32") return PRNGType::XORSHIFT32;
    if (name == "mt19937") return PRNGType::MT19937;
    throw std::runtime_error("Unknown generator type: " + name);
}

// Simple LCG: x_{n+1} = (a * x_n + c) mod m
struct LCG {
    uint64_t a;
    uint64_t c;
    uint64_t m;
    uint64_t state;

    LCG(uint64_t seed,
        uint64_t a_ = 1664525ULL,
        uint64_t c_ = 1013904223ULL,
        uint64_t m_ = (1ULL << 32))
        : a(a_), c(c_), m(m_), state(seed) {}

    uint32_t next_u32() {
        state = (a * state + c) % m;
        return static_cast<uint32_t>(state);
    }

    double next_double() {
        return static_cast<double>(next_u32()) / static_cast<double>(m);
    }
};


// RANDU: x_{n+1} = (65539 * x_n) mod 2^31
struct RANDU {
    uint32_t state;

    explicit RANDU(uint32_t seed) : state(seed) {}

    uint32_t next_u32() {
        const uint64_t a = 65539ULL;
        const uint64_t m = (1ULL << 31);
        state = static_cast<uint32_t>((a * state) % m);
        return state;
    }

    double next_double() {
        const double m = static_cast<double>(1ULL << 31);
        return static_cast<double>(next_u32()) / m;
    }
};

// Xorshift32
struct Xorshift32 {
    uint32_t state;

    explicit Xorshift32(uint32_t seed) : state(seed ? seed : 1u) {}

    uint32_t next_u32() {
        uint32_t x = state;
        x ^= x << 13;
        x ^= x >> 17;
        x ^= x << 5;
        state = x;
        return x;
    }

    double next_double() {
        return static_cast<double>(next_u32()) / static_cast<double>(UINT32_MAX);
    }
};

int main(int argc, char** argv) {
    if (argc < 5) {
        std::cerr << "Usage: " << argv[0]
                  << " <generator> <seed> <N> <output.csv>\n"
                     "  <generator> in {lcg, randu, xorshift32, mt19937}\n";
        return 1;
    }

    std::string gen_name = argv[1];
    uint64_t seed = std::stoull(argv[2]);
    uint64_t N = std::stoull(argv[3]);
    std::string out_path = argv[4];

    PRNGType type;
    try {
        type = parse_type(gen_name);
    } catch (const std::exception &e) {
        std::cerr << e.what() << "\n";
        return 1;
    }

    std::ofstream out(out_path);
    if (!out) {
        std::cerr << "Cannot open output file: " << out_path << "\n";
        return 1;
    }

    // Write header
    out << "x\n";

    switch (type) {
        case PRNGType::LCG: {
            LCG rng(seed);
            for (uint64_t i = 0; i < N; ++i) {
                out << rng.next_double() << "\n";
            }
            break;
        }
        case PRNGType::RANDU: {
            RANDU rng(static_cast<uint32_t>(seed));
            for (uint64_t i = 0; i < N; ++i) {
                out << rng.next_double() << "\n";
            }
            break;
        }
        case PRNGType::XORSHIFT32: {
            Xorshift32 rng(static_cast<uint32_t>(seed));
            for (uint64_t i = 0; i < N; ++i) {
                out << rng.next_double() << "\n";
            }
            break;
        }
        case PRNGType::MT19937: {
            std::mt19937 rng(static_cast<uint32_t>(seed));
            std::uniform_real_distribution<double> dist(0.0, 1.0);
            for (uint64_t i = 0; i < N; ++i) {
                out << dist(rng) << "\n";
            }
            break;
        }
    }

    return 0;
}
