#pragma once
#include <stddef.h>
#include <stdint.h>
#include <gmp.h>
#include <initializer_list>
#include <vector>
#include <string>
#include <type_traits>
#include <stdexcept>

enum class BigIntegerEndian { Little, Big };

class BigInteger {
private:
    mpz_t m_Value;
public:

    BigInteger() noexcept {
        mpz_init(m_Value);
    }

    template<typename __IntegerType>
    BigInteger(__IntegerType SmallInteger) noexcept : m_Value{} {
        // __IntegerType must be a integer type, i.e. char, int, unsigned long...
        static_assert(std::is_integral<__IntegerType>::value);

        if constexpr (std::is_signed<__IntegerType>::value) {
            mpz_init_set_sx(m_Value, SmallInteger);
        } else {
            mpz_init_set_ux(m_Value, SmallInteger);
        }
    }

    BigInteger(bool IsNegative, const void* lpBytes, size_t cbBytes, BigIntegerEndian Endian) noexcept : m_Value{} {
        mpz_init(m_Value);
        mpz_import(m_Value, cbBytes, Endian == BigIntegerEndian::Little ? -1 : 1, sizeof(unsigned char), 0, 0, lpBytes);
        if (IsNegative) {
            mpz_neg(m_Value, m_Value);
        }
    }

    BigInteger(bool IsNegative, const std::initializer_list<uint8_t>& Bytes, BigIntegerEndian Endian) noexcept : m_Value{} {
        mpz_init(m_Value);
        mpz_import(m_Value, Bytes.size(), Endian == BigIntegerEndian::Little ? -1 : 1, sizeof(unsigned char), 0, 0, Bytes.begin());
        if (IsNegative) {
            mpz_neg(m_Value, m_Value);
        }
    }

    BigInteger(bool IsNegative, const std::vector<uint8_t>& Bytes, BigIntegerEndian Endian) noexcept : m_Value{} {
        mpz_init(m_Value);
        mpz_import(m_Value, Bytes.size(), Endian == BigIntegerEndian::Little ? -1 : 1, sizeof(unsigned char), 0, 0, Bytes.data());
        if (IsNegative) {
            mpz_neg(m_Value, m_Value);
        }
    }

    BigInteger(const char* lpszValue) noexcept : m_Value{} {
        mpz_init_set_str(m_Value, lpszValue, 0);
    }

    BigInteger(const std::string& szValue) noexcept : m_Value{} {
        mpz_init_set_str(m_Value, szValue.c_str(), 0);
    }

    BigInteger(const BigInteger& Other) noexcept : m_Value{} {
        mpz_init(m_Value);
        mpz_set(m_Value, Other.m_Value);
    }

    BigInteger(BigInteger&& Other) noexcept : m_Value{} {
        mpz_init(m_Value);
        mpz_swap(m_Value, Other.m_Value);
    }

    BigInteger& operator=(const BigInteger& Other) noexcept {
        if (this != std::addressof(Other)) {
            mpz_set(m_Value, Other.m_Value);
        }

        return *this;
    }

    BigInteger& operator=(BigInteger&& Other) noexcept {
        if (this != std::addressof(Other)) {
            mpz_swap(m_Value, Other.m_Value);
            mpz_set_ux(Other.m_Value, 0);
        }

        return *this;
    }

    template<typename __IntegerType>
    BigInteger& operator=(__IntegerType SmallInteger) noexcept {
        // __IntegerType must be a integer type, i.e. char, int, unsigned long...
        static_assert(std::is_integral<__IntegerType>::value);

        if constexpr (std::is_signed<__IntegerType>::value) {
            mpz_set_sx(m_Value, SmallInteger);
        } else {
            mpz_set_ux(m_Value, SmallInteger);
        }

        return *this;
    }

    BigInteger& operator=(const char* lpszValue) noexcept {
        mpz_init_set_str(m_Value, lpszValue, 0);
        return *this;
    }

    BigInteger& operator=(std::string& szValue) noexcept {
        mpz_init_set_str(m_Value, szValue.c_str(), 0);
        return *this;
    }

    [[nodiscard]]
    bool operator==(const BigInteger& Other) const noexcept {
        return mpz_cmp(m_Value, Other.m_Value) == 0;
    }

    [[nodiscard]]
    bool operator!=(const BigInteger& Other) const noexcept {
        return mpz_cmp(m_Value, Other.m_Value) != 0;
    }

    [[nodiscard]]
    bool operator<(const BigInteger& Other) const noexcept {
        return mpz_cmp(m_Value, Other.m_Value) < 0;
    }

    [[nodiscard]]
    bool operator<=(const BigInteger& Other) const noexcept {
        auto d = mpz_cmp(m_Value, Other.m_Value);
        return d < 0 || d == 0;
    }

    [[nodiscard]]
    bool operator>(const BigInteger& Other) const noexcept {
        return mpz_cmp(m_Value, Other.m_Value) > 0;
    }

    [[nodiscard]]
    bool operator>=(const BigInteger& Other) const noexcept {
        auto d = mpz_cmp(m_Value, Other.m_Value);
        return d > 0 || d == 0;
    }

    [[nodiscard]]
    BigInteger operator-() const noexcept {
        BigInteger Result;
        mpz_neg(Result.m_Value, m_Value);
        return Result;
    }

    [[nodiscard]]
    BigInteger operator+(const BigInteger& Other) const noexcept {
        BigInteger Result;
        mpz_add(Result.m_Value, m_Value, Other.m_Value);
        return Result;
    }

    BigInteger& operator+=(const BigInteger& Other) noexcept {
        mpz_add(m_Value, m_Value, Other.m_Value);
        return *this;
    }

    [[nodiscard]]
    BigInteger operator-(const BigInteger& Other) const noexcept {
        BigInteger Result;
        mpz_sub(Result.m_Value, m_Value, Other.m_Value);
        return Result;
    }

    BigInteger& operator-=(const BigInteger& Other) noexcept {
        mpz_sub(m_Value, m_Value, Other.m_Value);
        return *this;
    }

    [[nodiscard]]
    BigInteger operator*(const BigInteger& Other) const noexcept {
        BigInteger Result;
        mpz_mul(Result.m_Value, m_Value, Other.m_Value);
        return Result;
    }

    BigInteger& operator*=(const BigInteger& Other) noexcept {
        mpz_mul(m_Value, m_Value, Other.m_Value);
        return *this;
    }

    [[nodiscard]]
    BigInteger operator/(const BigInteger& Other) const noexcept {
        BigInteger Result;
        mpz_fdiv_q(Result.m_Value, m_Value, Other.m_Value);
        return Result;
    }

    BigInteger& operator/=(const BigInteger& Other) noexcept {
        mpz_fdiv_q(m_Value, m_Value, Other.m_Value);
        return *this;
    }

    [[nodiscard]]
    BigInteger operator%(const BigInteger& Other) const noexcept {
        BigInteger Result;
        mpz_fdiv_r(Result.m_Value, m_Value, Other.m_Value);
        return Result;
    }

    BigInteger& operator%=(const BigInteger& Other) noexcept {
        mpz_fdiv_r(m_Value, m_Value, Other.m_Value);
        return *this;
    }

    [[nodiscard]]
    BigInteger operator~() const noexcept {
        BigInteger Result;
        mpz_com(Result.m_Value, m_Value);
        return Result;
    }

    [[nodiscard]]
    BigInteger operator&(const BigInteger& Other) const noexcept {
        BigInteger Result;
        mpz_and(Result.m_Value, m_Value, Other.m_Value);
        return Result;
    }

    BigInteger& operator&=(const BigInteger& Other) noexcept {
        mpz_and(m_Value, m_Value, Other.m_Value);
        return *this;
    }

    [[nodiscard]]
    BigInteger operator|(const BigInteger& Other) const noexcept {
        BigInteger Result;
        mpz_ior(Result.m_Value, m_Value, Other.m_Value);
        return Result;
    }

    BigInteger& operator|=(const BigInteger& Other) noexcept {
        mpz_ior(m_Value, m_Value, Other.m_Value);
        return *this;
    }

    [[nodiscard]]
    BigInteger operator^(const BigInteger& Other) const noexcept {
        BigInteger Result;
        mpz_xor(Result.m_Value, m_Value, Other.m_Value);
        return Result;
    }

    BigInteger& operator^=(const BigInteger& Other) noexcept {
        mpz_xor(m_Value, m_Value, Other.m_Value);
        return *this;
    }

    BigInteger& operator++() noexcept {
        mpz_add_ui(m_Value, m_Value, 1);
        return *this;
    }

    BigInteger operator++(int) noexcept {
        BigInteger Result(*this);
        mpz_add_ui(m_Value, m_Value, 1);
        return Result;
    }

    BigInteger& operator--() noexcept {
        mpz_sub_ui(m_Value, m_Value, 1);
        return *this;
    }

    BigInteger operator--(int) noexcept {
        BigInteger Result(*this);
        mpz_sub_ui(m_Value, m_Value, 1);
        return Result;
    }

    [[nodiscard]]
    bool IsZero() const noexcept {
        return mpz_sgn(m_Value) == 0;
    }

    [[nodiscard]]
    bool IsPositive() const noexcept {
        return mpz_sgn(m_Value) > 0;
    }

    [[nodiscard]]
    bool IsNegative() const noexcept {
        return mpz_sgn(m_Value) < 0;
    }

    [[nodiscard]]
    bool IsOne() const noexcept {
        return mpz_cmp_si(m_Value, 1) == 0;
    }

    BigInteger& InverseMod(const BigInteger& N) {
        if (N.IsNegative() || N.IsZero() || N.IsOne()) {
            throw std::invalid_argument("N must be greater than 1");
        } else {
            mpz_invert(m_Value, m_Value, N.m_Value);
            return *this;
        }
    }

    [[nodiscard]]
    BigInteger InverseModValue(const BigInteger& N) const {
        if (N.IsNegative() || N.IsZero() || N.IsOne()) {
            throw std::invalid_argument("N must be greater than 1");
        } else {
            BigInteger Result;
            mpz_invert(Result.m_Value, m_Value, N.m_Value);

            return Result;
        }
    }

    BigInteger& Load(bool IsNegative, const void* lpBytes, size_t cbBytes, BigIntegerEndian Endian) noexcept {
        mpz_import(m_Value, cbBytes, Endian == BigIntegerEndian::Little ? -1 : 1, sizeof(uint8_t), 0, 0, lpBytes);
        if (IsNegative) {
            mpz_neg(m_Value, m_Value);
        }
        return *this;
    }

    BigInteger& Load(bool IsNegative, const std::initializer_list<uint8_t>& Bytes, BigIntegerEndian Endian) noexcept {
        mpz_import(m_Value, Bytes.size(), Endian == BigIntegerEndian::Little ? -1 : 1, sizeof(uint8_t), 0, 0, Bytes.begin());
        if (IsNegative) {
            mpz_neg(m_Value, m_Value);
        }
        return *this;
    }

    BigInteger& Load(bool IsNegative, const std::vector<uint8_t>& Bytes, BigIntegerEndian Endian) noexcept {
        mpz_import(m_Value, Bytes.size(), Endian == BigIntegerEndian::Little ? -1 : 1, sizeof(uint8_t), 0, 0, Bytes.data());
        if (IsNegative) {
            mpz_neg(m_Value, m_Value);
        }
        return *this;
    }

    void DumpAbsoluteValue(void* lpBuffer, size_t cbBuffer, BigIntegerEndian Endian) const {
        size_t bit_size = mpz_sizeinbase(m_Value, 2);
        size_t storage_size = (bit_size + 7) / 8;

        if (cbBuffer >= storage_size) {
            size_t bytes_written;
            mpz_export(lpBuffer, &bytes_written, Endian == BigIntegerEndian::Little ? -1 : 1, sizeof(uint8_t), 0, 0, m_Value);
            memset(reinterpret_cast<uint8_t*>(lpBuffer) + bytes_written, 0, cbBuffer - bytes_written);
        } else {
            throw std::length_error("Insufficient buffer.");
        }
    }

    [[nodiscard]]
    std::vector<uint8_t> DumpAbsoluteValue(BigIntegerEndian Endian) const noexcept {
        size_t bit_size = mpz_sizeinbase(m_Value, 2);
        size_t storage_size = (bit_size + 7) / 8;
        std::vector<uint8_t> bytes(storage_size);

        mpz_export(bytes.data(), nullptr, Endian == BigIntegerEndian::Little ? -1 : 1, sizeof(uint8_t), 0, 0, m_Value);

        return bytes;
    }

    [[nodiscard]]
    size_t BitLength() const noexcept {
        return mpz_sizeinbase(m_Value, 2);
    }

    [[nodiscard]]
    bool TestBit(size_t i) const noexcept {
        return mpz_tstbit(m_Value, i) != 0;
    }

    void SetBit(size_t i) noexcept {
        mpz_setbit(m_Value, i);
    }

    [[nodiscard]]
    std::string ToString(size_t Base, bool LowerCase = false) const {
        if (2 <= Base && Base <= 10 + 26) {
            int base = LowerCase ? static_cast<int>(Base) : -static_cast<int>(Base);
            std::string s(mpz_sizeinbase(m_Value, static_cast<int>(Base)) + 2, '\x00');
            
            mpz_get_str(s.data(), base, m_Value);

            while (s.back() == '\x00') {
                s.pop_back();
            }

            return s;
        } else {
            throw std::invalid_argument("Invalid base value.");
        }
    }

    ~BigInteger() {
        mpz_clear(m_Value);
    }
};

