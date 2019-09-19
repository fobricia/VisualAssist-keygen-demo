#pragma once
#include <stddef.h>
#include <stdint.h>
#include <vector>

template<uint32_t __Polynomial>
struct HasherCrc32Traits {
    static constexpr size_t DigestSizeValue = 32 / 8;
    using DigestType = uint32_t;

    struct Constant {
        using LookupTableType = uint32_t[256];

        static const LookupTableType& InitializeLookupTable() noexcept {
            static LookupTableType LookupTable = {};

            if (LookupTable[1] == 0) {
                for (unsigned i = 0; i < 256; ++i) {
                    uint32_t result = i;
                    for (unsigned j = 0; j < 8; ++j) {
                        if (result % 2) {
                            result /= 2;
                            result ^= __Polynomial;
                        } else {
                            result /= 2;
                        }
                    }
                    LookupTable[i] = result;
                }
            }

            return LookupTable;
        }

        static inline const LookupTableType& LookupTable = InitializeLookupTable();
    };

    struct Context {
        uint32_t InitialValue;
        uint32_t Value;

        void Initialize() noexcept {
            InitialValue = 0;
            Reset();
        }

        void Initialize(uint32_t InitialVal) noexcept {
            InitialValue = InitialVal;
            Reset();
        }

        void Reset() noexcept {
            Value = InitialValue;
        }

        void Update(const void* lpData, size_t cbData) noexcept {
            auto pbData = reinterpret_cast<const uint8_t*>(lpData);
            Value = ~Value;
            for (size_t i = 0; i < cbData; ++i) {
                Value = (Value >> 8) ^ Constant::LookupTable[static_cast<uint8_t>(Value) ^ pbData[i]];
            }
            Value = ~Value;
        }

        void Evaluate(void* lpDigest) const noexcept {
            *reinterpret_cast<uint32_t*>(lpDigest) = Value;
        }

        DigestType Evaluate() const noexcept {
            return Value;
        }

        void Destroy() noexcept {
            Value = 0;
        }
    };

    struct InitByDefault {
        using TraitsType = HasherCrc32Traits<__Polynomial>;

        static Context Impl() noexcept {
            Context NewCtx;
            NewCtx.Initialize();
            return NewCtx;
        }
    };

    struct InitByCustomInitialValue {
        using TraitsType = HasherCrc32Traits<__Polynomial>;

        static Context Impl(uint32_t InitialVal) noexcept {
            Context NewCtx;
            NewCtx.Initialize(InitialVal);
            return NewCtx;
        }
    };
};

#ifdef _MSC_VER
#include <windows.h>

#ifdef __cplusplus
extern "C" {
#endif

#pragma comment(lib, "ntdll")

    // available on WindowsXP and above
    NTSYSAPI
    DWORD
    NTAPI
    RtlComputeCrc32(
        _In_ DWORD InitialCrc,
        _In_reads_bytes_(Size) const void* Buffer,
        _In_ size_t Size
    );

#ifdef __cplusplus
}
#endif

template<>
struct HasherCrc32Traits<0xEDB88320> {
    static constexpr size_t DigestSizeValue = 32 / 8;
    using DigestType = uint32_t;

    struct Context {
        uint32_t InitialValue;
        uint32_t Value;

        void Initialize() noexcept {
            InitialValue = 0;
            Reset();
        }

        void Initialize(uint32_t InitialVal) noexcept {
            InitialVal = InitialVal;
            Reset();
        }

        void Reset() noexcept {
            Value = InitialValue;
        }

        void Update(const void* lpData, size_t cbData) noexcept {
            Value = RtlComputeCrc32(Value, lpData, cbData);
        }

        void Evaluate(void* lpDigest) const noexcept {
            *reinterpret_cast<uint32_t*>(lpDigest) = Value;
        }

        DigestType Evaluate() const noexcept {
            return Value;
        }

        void Destroy() noexcept {
            Value = 0;
        }
    };

    struct InitByDefault {
        using TraitsType = HasherCrc32Traits<0xEDB88320>;

        static Context Impl() noexcept {
            Context NewCtx;
            NewCtx.Initialize();
            return NewCtx;
        }
    };

    struct InitByCustomInitialValue {
        using TraitsType = HasherCrc32Traits<0xEDB88320>;

        static Context Impl(uint32_t InitialVal) noexcept {
            Context NewCtx;
            NewCtx.Initialize(InitialVal);
            return NewCtx;
        }
    };
};

#endif  // #ifdef _MSC_VER

