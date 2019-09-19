#pragma once
#include <stddef.h>
#include <stdint.h>

class VisualAssistRandomGenerator {
private:
    static constexpr int32_t m = 100000000;
    static constexpr int32_t m1 = 10000;
    static constexpr int32_t b = 31415821;

    uint32_t m_Seed;

    [[nodiscard]]
    static uint32_t _mult(int32_t p, int32_t q) noexcept {
        uint32_t p1 = p / m1;
        uint32_t p0 = p % m1;
        uint32_t q1 = q / m1;
        uint32_t q0 = q % m1;
        return (((p0 * q1 + p1 * q0) % m1) * m1 + p0 * q0) % m;
    }

public:

    VisualAssistRandomGenerator(uint32_t Seed) noexcept :
        m_Seed(Seed) {}

    void SetSeed(uint32_t Seed) noexcept {
        m_Seed = Seed;
    }

    [[nodiscard]]
    uint32_t GetSeed() const noexcept {
        return m_Seed;
    }

    [[nodiscard]]
    uint32_t NextRandomRange(int32_t Range) noexcept {
        m_Seed = (_mult(m_Seed, b) + 1) % m;
        return  (((m_Seed / m1) * Range) / m1);
    }

    [[nodiscard]]
    uint32_t NextRandomNumber() noexcept {
        uint32_t n1 = NextRandomRange(256) << 24;
        uint32_t n2 = NextRandomRange(256) << 16;
        uint32_t n3 = NextRandomRange(256) << 8;
        uint32_t n4 = NextRandomRange(256);
        return n1 ^ n2 ^ n3 ^ n4;
    }
};
