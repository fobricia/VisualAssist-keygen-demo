#pragma once
#include <stddef.h>
#include <stdint.h>
#include <initializer_list>
#include <vector>
#include <type_traits>
#include <utility>

struct GaloisFieldInitByZero {};
struct GaloisFieldInitByOne {};
struct GaloisFieldInitByBinary {};
struct GaloisFieldInitByElement {};

template<typename __FieldTraits>
class GaloisField {
private:

    using ElementType = typename __FieldTraits::ElementType;
    using TraceType = typename __FieldTraits::TraceType;

    ElementType m_Value;

    GaloisField(std::nullptr_t) noexcept {};

    GaloisField(const ElementType& e) noexcept :
        m_Value(e) {}

    GaloisField(ElementType&& e) noexcept :
        m_Value(std::move(e)) {}

public:

    GaloisField() noexcept {
        __FieldTraits::SetZero(m_Value);
    }

    GaloisField(GaloisFieldInitByZero) noexcept {
        __FieldTraits::SetZero(m_Value);
    }

    GaloisField(GaloisFieldInitByOne) noexcept {
        __FieldTraits::SetOne(m_Value);
    }

    GaloisField(GaloisFieldInitByBinary, const void* lpBinary, size_t cbBinary) {
        __FieldTraits::Deserialize(m_Value, lpBinary, cbBinary);
    }

    GaloisField(GaloisFieldInitByBinary, const std::vector<uint8_t>& Binary) {
        __FieldTraits::Deserialize(m_Value, Binary.data(), Binary.size());
    }

    GaloisField(GaloisFieldInitByBinary, const std::initializer_list<uint8_t>& Binary) {
        __FieldTraits::Deserialize(m_Value, Binary.begin(), Binary.size());
    }

    GaloisField(GaloisFieldInitByElement, const ElementType& Element) :
        m_Value(Element) { __FieldTraits::Verify(m_Value); }

    [[nodiscard]]
    const ElementType& GetValue() const noexcept {
        return m_Value;
    }

    GaloisField& SetValue(const ElementType& Element) {
        __FieldTraits::Verify(Element);
        m_Value = Element;
        return *this;
    }

    [[nodiscard]]
    bool IsZero() const noexcept {
        return __FieldTraits::IsZero(m_Value);
    }

    [[nodiscard]]
    bool IsOne() const noexcept {
        return __FieldTraits::IsOne(m_Value);
    }

    [[nodiscard]]
    bool operator==(const GaloisField& Other) const noexcept {
        return __FieldTraits::IsEqual(m_Value, Other.m_Value);
    }

    [[nodiscard]]
    bool operator!=(const GaloisField& Other) const noexcept {
        return __FieldTraits::IsEqual(m_Value, Other.m_Value) == false;
    }

    [[nodiscard]]
    GaloisField operator-() const noexcept {
        GaloisField Result(nullptr);
        __FieldTraits::Negative(Result.m_Value, m_Value);
        return Result;
    }

    [[nodiscard]]
    GaloisField operator+(const GaloisField& Other) const noexcept {
        GaloisField Result(nullptr);
        __FieldTraits::Add(Result.m_Value, m_Value, Other.m_Value);
        return Result;
    }

    GaloisField<__FieldTraits>& operator+=(const GaloisField<__FieldTraits>& Other) noexcept {
        __FieldTraits::AddAssign(m_Value, Other.m_Value);
        return *this;
    }

    [[nodiscard]]
    GaloisField operator-(const GaloisField& Other) const noexcept {
        GaloisField Result(nullptr);
        __FieldTraits::Substract(Result.m_Value, m_Value, Other.m_Value);
        return Result;
    }

    GaloisField& operator-=(const GaloisField& Other) noexcept {
        __FieldTraits::SubstractAssign(m_Value, Other.m_Value);
        return *this;
    }

    [[nodiscard]]
    GaloisField operator*(const GaloisField& Other) const noexcept {
        GaloisField Result(nullptr);
        __FieldTraits::Multiply(Result.m_Value, m_Value, Other.m_Value);
        return Result;
    }

    GaloisField& operator*=(const GaloisField& Other) noexcept {
        __FieldTraits::MultiplyAssign(m_Value, Other.m_Value);
        return *this;
    }

    [[nodiscard]]
    GaloisField operator/(const GaloisField& Other) const {
        GaloisField Result(nullptr);
        __FieldTraits::Divide(Result.m_Value, m_Value, Other.m_Value);
        return Result;
    }

    GaloisField& operator/=(const GaloisField& Other) {
        __FieldTraits::DivideAssign(m_Value, Other.m_Value);
        return *this;
    }

    GaloisField& operator++() noexcept {  // prefix ++
        __FieldTraits::AddOneAssign(m_Value);
        return *this;
    }

    GaloisField operator++(int) noexcept { // postfix ++
        GaloisField Prev(*this);
        __FieldTraits::AddOneAssign(m_Value);
        return Prev;
    }

    GaloisField& operator--() noexcept {  // prefix --
        __FieldTraits::SubstractOneAssign(m_Value);
        return *this;
    }

    GaloisField operator--(int) noexcept { // postfix --
        GaloisField Prev(*this);
        __FieldTraits::SubstractOneAssign(m_Value);
        return Prev;
    }

    GaloisField& Inverse() {
        __FieldTraits::InverseAssign(m_Value);
        return *this;
    }

    [[nodiscard]]
    GaloisField InverseValue() const {
        GaloisField Result(nullptr);
        __FieldTraits::Inverse(Result.m_Value, m_Value);
        return Result;
    }

    GaloisField& AddOne() noexcept {
        __FieldTraits::AddOneAssign(m_Value);
        return *this;
    }

    [[nodiscard]]
    GaloisField AddOneValue() const noexcept {
        GaloisField Result(nullptr);
        __FieldTraits::AddOne(Result.m_Value, m_Value);
        return Result;
    }

    GaloisField& SubstractOne() noexcept {
        __FieldTraits::SubstractOneAssign(m_Value);
        return *this;
    }

    [[nodiscard]]
    GaloisField SubstractOneValue() const noexcept {
        GaloisField Result(nullptr);
        __FieldTraits::SubstractOne(Result.m_Value, m_Value);
        return Result;
    }

    GaloisField& Square() noexcept {
        __FieldTraits::SquareAssign(m_Value);
        return *this;
    }

    [[nodiscard]]
    GaloisField SquareValue() const noexcept {
        GaloisField Result(nullptr);
        __FieldTraits::Square(Result.m_Value, m_Value);
        return Result;
    }

    GaloisField& SquareRoot() noexcept {
        __FieldTraits::SquareRootAssign(m_Value);
        return *this;
    }

    [[nodiscard]]
    GaloisField SquareRootValue() const noexcept {
        GaloisField Result(nullptr);
        __FieldTraits::SquareRoot(Result.m_Value, m_Value);
        return Result;
    }

    [[nodiscard]]
    TraceType Trace() const noexcept {
        TraceType tr;
        __FieldTraits::Trace(tr, m_Value);
        return tr;
    }

    [[nodiscard]]
    size_t Serialize(void* lpBinaryBuffer, size_t cbBinaryBuffer) const {
        return __FieldTraits::Serialize(m_Value, lpBinaryBuffer, cbBinaryBuffer);
    }

    [[nodiscard]]
    std::vector<uint8_t> Serialize() const noexcept {
        return __FieldTraits::Serialize(m_Value);
    }

    GaloisField& Deserialize(const void* lpBinary, size_t cbBinary) {
        __FieldTraits::Deserialize(m_Value, lpBinary, cbBinary);
        return *this;
    }

    GaloisField& Deserialize(const std::initializer_list<uint8_t>& Binary) {
        __FieldTraits::Deserialize(m_Value, Binary.begin(), Binary.size());
        return *this;
    }

    GaloisField& Deserialize(const std::vector<uint8_t>& Binary) {
        __FieldTraits::Deserialize(m_Value, Binary.data(), Binary.size());
        return *this;
    }

    GaloisField& SetZero() noexcept {
        __FieldTraits::SetZero(m_Value);
        return *this;
    }

    GaloisField& SetOne() noexcept {
        __FieldTraits::SetOne(m_Value);
        return *this;
    }

    static GaloisField GetValueOfZero() noexcept {
        return GaloisField(GaloisFieldInitByZero{});
    }

    static GaloisField GetValueOfOne() noexcept {
        return GaloisField(GaloisFieldInitByOne{});
    }

    // Solve "A * x^2 + B * x + C = 0"
    static std::vector<GaloisField> SolveQuadratic(const GaloisField& A, const GaloisField& B, const GaloisField& C) {
        std::vector<GaloisField> Roots;
        std::vector<ElementType> roots = __FieldTraits::SolveQuadratic(A.m_Value, B.m_Value, C.m_Value);

        for (size_t i = 0; i < roots.size(); ++i) {
            Roots.emplace_back(
                GaloisField(std::move(roots[i]))
            );
        }

        return Roots;
    }
};

