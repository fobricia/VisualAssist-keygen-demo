#pragma once
#include <stddef.h>
#include <stdint.h>
#include <stdexcept>
#include <algorithm>
#include "BigInteger.hpp"

template<typename __FieldType>
class EllipticCurveGF2m {
private:

    // y^2 + xy = x^3 + Ax^2 + B
    __FieldType m_A;
    __FieldType m_B;

public:

    class Point {
    private:

        const EllipticCurveGF2m<__FieldType>& m_Curve;
        __FieldType m_X;
        __FieldType m_Y;

    public:

        // Create infinity point.
        Point(const EllipticCurveGF2m<__FieldType>& Curve) noexcept : 
            m_Curve(Curve),
            m_X(__FieldType::GetValueOfZero()),
            m_Y(__FieldType::GetValueOfZero()) {}

        // Create point with (X, Y)
        Point(const EllipticCurveGF2m<__FieldType>& Curve, const __FieldType& X, const __FieldType& Y) : 
            m_Curve(Curve), 
            m_X(X), 
            m_Y(Y) 
        {
            if (m_X.IsZero() && m_Y.IsZero()) {   // infinity point which is allowed
                return;
            }

            auto Left = m_Y.SquareValue() + m_X * m_Y;
            auto Right = (m_X + m_Curve.m_A) * m_X.SquareValue() + m_Curve.m_B;
            if (Left != Right) {
                throw std::invalid_argument("New point is not on the curve specified.");
            }
        }

        Point(const Point& Other) noexcept :
            m_Curve(Other.m_Curve),
            m_X(Other.m_X),
            m_Y(Other.m_Y) {}

        Point(Point&& Other) noexcept :
            m_Curve(Other.m_Curve),
            m_X(std::move(Other.m_X)),
            m_Y(std::move(Other.m_Y)) {}

        Point& operator=(const Point& Other) {
            if (this == std::addressof(Other)) {
                return *this;
            }

            if (m_Curve == Other.m_Curve) {
                m_X = Other.m_X;
                m_Y = Other.m_Y;
                return *this;
            } else {
                throw std::invalid_argument("Not on the same curve.");
            }
        }

        Point& operator=(Point&& Other) {
            if (this == std::addressof(Other)) {
                return *this;
            }

            if (m_Curve == Other.m_Curve) {
                m_X = std::move(Other.m_X);
                m_Y = std::move(Other.m_Y);
                return *this;
            } else {
                throw std::invalid_argument("Not on the same curve.");
            }
        }

        [[nodiscard]]
        bool operator==(const Point& Other) const noexcept {
            if (this == &Other) {
                return true;
            } else {
                return m_Curve == Other.m_Curve && m_X == Other.m_X && m_Y == Other.m_Y;
            }
        }

        [[nodiscard]]
        bool operator!=(const Point& Other) const noexcept {
            if (this == &Other) {
                return false;
            } else {
                return m_Curve != Other.m_Curve || m_X != Other.m_X || m_Y != Other.m_Y;
            }
        }

        [[nodiscard]]
        bool IsAtInfinity() const noexcept {
            return m_X.IsZero() && m_Y.IsZero();
        }

        [[nodiscard]]
        Point operator-() const noexcept {
            if (IsAtInfinity()) {
                return *this;
            } else {
                return Point(m_Curve, m_X, m_X + m_Y);
            }
        }

        Point& Double() noexcept {
            if (IsAtInfinity() == false) {
                if (m_X.IsZero()) {
                    m_Y.SetZero();
                } else {
                    auto m = m_Y / m_X + m_X;

                    // NewX = m ^ 2 + m + a
                    __FieldType NewX = m.SquareValue();
                    NewX += m;
                    NewX += m_Curve.m_A;

                    // NewY = X ^ 2 + (m + 1) * NewX
                    m_Y = m.AddOne();
                    m_Y *= NewX;
                    m_Y += m_X.Square();

                    m_X = NewX;
                }
            }
            return *this;
        }

        [[nodiscard]]
        Point DoubleValue() const noexcept {
            Point Result(m_Curve);

            if (IsAtInfinity() == false && m_X.IsZero() == false) {
                // m = X + Y / X
                auto m = m_Y / m_X + m_X;

                // NewX = m ^ 2 + m + a
                Result.m_X = m.SquareValue();
                Result.m_X += m;
                Result.m_X += m_Curve.m_A;

                // NewY = X ^ 2 + (m + 1) * NewX
                Result.m_Y = m.AddOne();
                Result.m_Y *= Result.m_X;
                Result.m_Y += m_X.SquareValue();
            }

            return Result;
        }

        [[nodiscard]]
        Point operator+(const Point& Other) const {
            // must be on the save curve
            if (m_Curve != Other.m_Curve) {
                throw std::invalid_argument("Not on the same curve.");
            }

            if (IsAtInfinity()) {
                return Other;
            }
            
            if (Other.IsAtInfinity()) {
                return *this;
            }
           
            if (*this == Other) {
                // If same point, use double point
                return DoubleValue();
            } else {
                Point Result(m_Curve);

                // if x-coordinates are the same, the result is infinity point
                // otherwise, do point addition
                if (m_X != Other.m_X) {
                    // m = (Y0 + Y1) / (X0 + X1)
                    auto m = (m_Y + Other.m_Y) / (m_X + Other.m_X);

                    // NewX = m ^ 2 + m + X0 + X1 + a
                    Result.m_X = m.SquareValue();
                    Result.m_X += m;
                    Result.m_X += m_X;
                    Result.m_X += Other.m_X;
                    Result.m_X += m_Curve.m_A;

                    // NewY = m * (X0 + NewX) + NewX + Y0
                    Result.m_Y = m_X + Result.m_X;
                    Result.m_Y *= m;
                    Result.m_Y += Result.m_X;
                    Result.m_Y += m_Y;
                }

                return Result;
            }
        }

        Point& operator+=(const Point& Other) {
            // must be on the save curve
            if (m_Curve != Other.m_Curve) {
                throw std::invalid_argument("Not on the same curve.");
            }

            if (Other.IsAtInfinity()) {
                return *this;
            }

            if (IsAtInfinity()) {
                m_X = Other.m_X;
                m_Y = Other.m_Y;
                return *this;
            }
           
            if (*this == Other) {
                Double();
            } else {
                if (m_X != Other.m_X) {
                    // m = (Y0 + Y1) / (X0 + X1)
                    auto m = (m_Y + Other.m_Y) / (m_X + Other.m_X);

                    // NewX = m ^ 2 + m + X0 + X1 + a
                    auto NewX = m.SquareValue();
                    NewX += m;
                    NewX += m_X;
                    NewX += Other.m_X;
                    NewX += m_Curve.m_A;

                    // NewY = m * (X0 + NewX) + NewX + Y0
                    m_X += NewX;
                    m_X *= m;
                    m_X += NewX;
                    m_Y += m_X;

                    m_X = NewX;
                } else {
                    m_X.SetZero();
                    m_Y.SetZero();
                }
            }

            return *this;
        }

        [[nodiscard]]
        Point operator-(const Point& Other) const {
            Point Result = -Other;
            Result += *this;
            return Result;
        }

        Point& operator-=(const Point& Other) {
            return *this += -Other;
        }

        [[nodiscard]]
        Point operator*(const BigInteger& N) const noexcept {
            Point Result(m_Curve);

            if (N.IsZero() == false) {
                Point temp(*this);
                size_t bit_length = N.BitLength();

                for (size_t i = 0; i < bit_length; ++i) {
                    if (N.TestBit(i) == true)
                        Result += temp;
                    temp.Double();
                }

                return Result;
            }

            return Result;
        }

        Point& operator*=(const BigInteger& N) noexcept {
            if (N.IsZero()) {
                m_X.SetZero();
                m_Y.SetZero();
            } else {
                Point Result(m_Curve);
                size_t bit_length = N.BitLength();

                for (size_t i = 0; i < bit_length; ++i) {
                    if (N.TestBit(i) == true)
                        Result += *this;
                    Double();
                }

                m_X = Result.m_X;
                m_Y = Result.m_Y;
            }

            return *this;
        }

        // SEC 1: Elliptic Curve Cryptography
        //     2.3.3 Elliptic-Curve-Point-to-Octet-String Conversion
        [[nodiscard]]
        std::vector<uint8_t> Dump() const noexcept {
            if (IsAtInfinity()) {
                std::vector<uint8_t> bytes = { 0x00 };
                return bytes;
            } else {
                std::vector<uint8_t> bytes = { 0x04 };
                std::vector<uint8_t> xbytes = m_X.Dump();
                std::vector<uint8_t> ybytes = m_Y.Dump();
                std::reverse(xbytes.begin(), xbytes.end());     // to big endian
                std::reverse(ybytes.begin(), ybytes.end());     // to big endian
                bytes.insert(bytes.end(), xbytes.begin(), xbytes.end());
                bytes.insert(bytes.end(), ybytes.begin(), ybytes.end());
                return bytes;
            }
        }

        // SEC 1: Elliptic Curve Cryptography
        //     2.3.3 Elliptic-Curve-Point-to-Octet-String Conversion
        [[nodiscard]]
        std::vector<uint8_t> DumpCompressed() const noexcept {
            if (IsAtInfinity()) {
                std::vector<uint8_t> bytes = { 0x00 };
                return bytes;
            } else {
                std::vector<uint8_t> bytes(1);
                std::vector<uint8_t> xbytes = m_X.Dump();
                std::vector<uint8_t> zbytes = (m_Y / m_X).Dump();

                if (zbytes[0] & 1) {
                    bytes[0] = 0x03;
                } else {
                    bytes[0] = 0x02;
                }

                std::reverse(xbytes.begin(), xbytes.end());     // to big endian
                bytes.insert(bytes.end(), xbytes.begin(), xbytes.end());
                return bytes;
            }
        }

        void Load(const std::vector<uint8_t>& SerializedBytes) {
            if (SerializedBytes.size() == 1 && SerializedBytes[0] == 0) {
                m_X.SetZero();
                m_Y.SetZero();
                return;
            }

            if (SerializedBytes.size() == 2 * __FieldType::BinaryByteSizeValue + 1 && SerializedBytes[0] == 0x04) {
                std::vector<uint8_t> RawNewX(
                    SerializedBytes.begin() + 1 + 0 * __FieldType::BinaryByteSizeValue, 
                    SerializedBytes.begin() + 1 + 1 * __FieldType::BinaryByteSizeValue
                );
                std::vector<uint8_t> RawNewY(
                    SerializedBytes.begin() + 1 + 1 * __FieldType::BinaryByteSizeValue, 
                    SerializedBytes.begin() + 1 + 2 * __FieldType::BinaryByteSizeValue
                );
                
                __FieldType NewX, NewY;
                NewX.Load(RawNewX);
                NewY.Load(RawNewY);

                auto Left = NewY.SquareValue() + NewX * NewY;
                auto Right = (NewX + m_Curve.m_A) * NewX.SquareValue() + m_Curve.m_B;
                if (Left != Right) {
                    throw std::invalid_argument("Serialized point is not on the curve.");
                }

                m_X = NewX;
                m_Y = NewY;
                return;
            }

            throw std::invalid_argument("Invalid serialized bytes.");
        }

        void LoadCompressed(const std::vector<uint8_t>& SerializedBytes) {
            if (SerializedBytes.size() == 1 && SerializedBytes[0] == 0) {
                m_X.SetZero();
                m_Y.SetZero();
                return;
            }

            if (SerializedBytes.size() == 1 + __FieldType::BinaryByteSizeValue && (SerializedBytes[0] == 0x02 || SerializedBytes[0] == 0x03)) {
                std::vector<uint8_t> RawNewX(
                    SerializedBytes.begin() + 1 + 0 * __FieldType::BinaryByteSizeValue,
                    SerializedBytes.begin() + 1 + 1 * __FieldType::BinaryByteSizeValue
                );

                __FieldType NewX;
                NewX.Load(RawNewX);

                if (NewX.IsZero()) {
                    m_X = NewX;
                    m_Y = m_Curve.m_B.SquareRootValue();
                } else {
                    auto beta = NewX + m_Curve.m_A + m_Curve.B * NewX.InverseValue().SquareValue();
                    auto roots = __FieldType::SolveQuadratic(__FieldType::GetOne(), __FieldType::GetOne(), beta);
                    if (roots.size() < 2) {
                        throw std::invalid_argument("Serialized point is not on the curve.");
                    }

                    auto zbytes = roots[0].Dump();
                    if (SerializedBytes[0] == 0x02 && (zbytes[0] & 1 == 0) || SerializedBytes[0] == 0x03 && (zbytes[0] & 1 != 0)) {
                        m_X = NewX;
                        m_Y = NewX * roots[0];
                    } else {
                        m_X = NewX;
                        m_Y = NewX * roots[1];
                    }
                }

                return;
            }

            throw std::invalid_argument("Invalid serialized bytes.");
        }

        [[nodiscard]]
        const __FieldType& GetX() const noexcept {
            return m_X;
        }

        [[nodiscard]]
        const __FieldType& GetY() const noexcept {
            return m_Y;
        }
    };

    EllipticCurveGF2m(const __FieldType& A, const __FieldType& B) : 
        m_A(A), 
        m_B(B) 
    {
        if (m_B.IsZero()) {
            throw std::invalid_argument("B cannot be zero.");
        }
    }

    [[nodiscard]]
    bool operator==(const EllipticCurveGF2m<__FieldType>& Other) const noexcept {
        return this == &Other || m_A == Other.m_A && m_B == Other.m_B;
    }

    [[nodiscard]]
    bool operator!=(const EllipticCurveGF2m<__FieldType>& Other) const noexcept {
        return this != &Other && (m_A != Other.m_A || m_B != Other.m_B);
    }

    [[nodiscard]]
    const __FieldType& GetParameterA() const noexcept {
        return m_A;
    }

    [[nodiscard]]
    const __FieldType& GetParameterB() const noexcept {
        return m_B;
    }

    [[nodiscard]]
    Point CreateInfinityPoint() const noexcept {
        return Point(*this);
    }

    [[nodiscard]]
    Point CreatePoint(const __FieldType& X, const __FieldType& Y) const {
        return Point(*this, X, Y);
    }
};
