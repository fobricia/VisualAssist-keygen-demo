#pragma once
#include <utility>

template<typename __HashTraits>
class Hasher {
public:

    using DigestType = typename __HashTraits::DigestType;

    static constexpr size_t DigestSizeValue = __HashTraits::DigestSizeValue;

private:

    using Context = typename __HashTraits::Context;

    Context m_Ctx;

public:

    template<typename __InitOptionType, typename... __Ts>
    Hasher(__InitOptionType, __Ts&& ... Args) :
        m_Ctx(__InitOptionType::Impl(std::forward<__Ts>(Args)...)) {}

    constexpr size_t DigestSize() const noexcept {
        return DigestSizeValue;
    }

    void Update(const void* lpBuffer, size_t cbBuffer) {
        m_Ctx.Update(lpBuffer, cbBuffer);
    }

    void Evaluate(void* lpDigest) const {
        m_Ctx.Evaluate(lpDigest);
    }

    DigestType Evaluate() const {
        return m_Ctx.Evaluate();
    }

    void Reset() {
        m_Ctx.Reset();
    }

    ~Hasher() {
        m_Ctx.Destroy();
    }
};

template<typename __InitOptionType, typename... __Ts>
Hasher(__InitOptionType, __Ts&& ... Args) -> 
    Hasher<typename __InitOptionType::TraitsType>;

