#pragma once
#include <stddef.h>
#include <stdint.h>
#include <memory.h>
#include <vector>
#include <utility>
#include <type_traits>

struct HasherMd5Traits {
public:

    static constexpr size_t DigestSizeValue = 128 / 8;
    using DigestType = std::vector<uint8_t>;

    struct Constant {
        static constexpr uint32_t T[64] = {
            0xD76AA478, 0xE8C7B756, 0x242070DB, 0xC1BDCEEE, 0xF57C0FAF, 0x4787C62A, 0xA8304613, 0xFD469501,
            0x698098D8, 0x8B44F7AF, 0xFFFF5BB1, 0x895CD7BE, 0x6B901122, 0xFD987193, 0xA679438E, 0x49B40821,
            0xF61E2562, 0xC040B340, 0x265E5A51, 0xE9B6C7AA, 0xD62F105D, 0x02441453, 0xD8A1E681, 0xE7D3FBC8,
            0x21E1CDE6, 0xC33707D6, 0xF4D50D87, 0x455A14ED, 0xA9E3E905, 0xFCEFA3F8, 0x676F02D9, 0x8D2A4C8A,
            0xFFFA3942, 0x8771F681, 0x6D9D6122, 0xFDE5380C, 0xA4BEEA44, 0x4BDECFA9, 0xF6BB4B60, 0xBEBFBC70,
            0x289B7EC6, 0xEAA127FA, 0xD4EF3085, 0x04881D05, 0xD9D4D039, 0xE6DB99E5, 0x1FA27CF8, 0xC4AC5665,
            0xF4292244, 0x432AFF97, 0xAB9423A7, 0xFC93A039, 0x655B59C3, 0x8F0CCC92, 0xFFEFF47D, 0x85845DD1,
            0x6FA87E4F, 0xFE2CE6E0, 0xA3014314, 0x4E0811A1, 0xF7537E82, 0xBD3AF235, 0x2AD7D2BB, 0xEB86D391
        };

        static constexpr int Shift[64] = {
            7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22, 7, 12, 17, 22,
            5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20, 5, 9, 14, 20,
            4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23, 4, 11, 16, 23,
            6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21, 6, 10, 15, 21
        };

        static constexpr size_t Selector[64] = {
            0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
            1, 6, 11, 0, 5, 10, 15, 4, 9, 14, 3, 8, 13, 2, 7, 12,
            5, 8, 11, 14, 1, 4, 7, 10, 13, 0, 3, 6, 9, 12, 15, 2,
            0, 7, 14, 5, 12, 3, 10, 1, 8, 15, 6, 13, 4, 11, 2, 9
        };
    };

private:

    static constexpr size_t BlockSizeValue = 512 / 8;

    using BlockType = uint32_t[16]; static_assert(sizeof(BlockType) == BlockSizeValue);

    struct Utility {

        template<size_t __Index>
        [[nodiscard]]
        static inline uint32_t F(uint32_t X, uint32_t Y, uint32_t Z) noexcept {
            if constexpr (0 <= __Index && __Index < 16) {
                return (X & Y) | (~X & Z);
            } else if constexpr (16 <= __Index && __Index < 32) {
                return (X & Z) | (Y & ~Z);
            } else if constexpr (32 <= __Index && __Index < 48) {
                return X ^ Y ^ Z;
            } else if constexpr (48 <= __Index && __Index < 64) {
                return Y ^ (X | ~Z);
            } else {
                static_assert(__Index < 64, "_F failure! Out of range.");
            }
        }

        template<size_t __Index>
        static inline void FF(uint32_t& A, uint32_t& B, uint32_t& C, uint32_t& D, uint32_t K, int s, uint32_t T) noexcept {
            A += F<__Index>(B, C, D) + K + T;
            A = _rotl(A, s);
            A += B;
        }

        template<size_t __Index>
        static inline void LoopIteration(uint32_t& A, uint32_t& B, uint32_t& C, uint32_t& D, const BlockType& MessageBlock) noexcept {
            if constexpr (__Index % 4 == 0) {
                FF<__Index>(
                    A, B, C, D,
                    MessageBlock[Constant::Selector[__Index]],
                    Constant::Shift[__Index],
                    Constant::T[__Index]
                    );
            } else if constexpr (__Index % 4 == 1) {
                FF<__Index>(
                    D, A, B, C,
                    MessageBlock[Constant::Selector[__Index]],
                    Constant::Shift[__Index],
                    Constant::T[__Index]
                    );
            } else if constexpr (__Index % 4 == 2) {
                FF<__Index>(
                    C, D, A, B,
                    MessageBlock[Constant::Selector[__Index]],
                    Constant::Shift[__Index],
                    Constant::T[__Index]
                    );
            } else {
                FF<__Index>(
                    B, C, D, A,
                    MessageBlock[Constant::Selector[__Index]],
                    Constant::Shift[__Index],
                    Constant::T[__Index]
                    );
            }
        }

        template<size_t... __Indexes>
        static inline void Loop(uint32_t& A, uint32_t& B, uint32_t& C, uint32_t& D, const BlockType& MessageBlock, std::index_sequence<__Indexes...>) noexcept {
            (LoopIteration<__Indexes>(A, B, C, D, MessageBlock), ...);
        }

        // lpData must be aligned to 4 bytes.
        // Otherwise, it would cause performance degradation.
        static void ProcessBlock(uint32_t State[4], const void* lpData) noexcept {
            uint32_t AA = State[0];
            uint32_t BB = State[1];
            uint32_t CC = State[2];
            uint32_t DD = State[3];

            Loop(AA, BB, CC, DD, *reinterpret_cast<const BlockType*>(lpData), std::make_index_sequence<64>{});

            State[0] += AA;
            State[1] += BB;
            State[2] += CC;
            State[3] += DD;
        }
    };

public:

    struct Context {
        uint32_t InitialState[4];
        uint32_t State[4];
        uint64_t BytesRead;
        alignas(uint32_t) uint8_t MessageQueue[BlockSizeValue];

        void Initialize() noexcept {
            InitialState[0] = 0x67452301u;
            InitialState[1] = 0xEFCDAB89u;
            InitialState[2] = 0x98BADCFEu;
            InitialState[3] = 0x10325476u;
            Reset();
        }

        void Initialize(uint32_t S1, uint32_t S2, uint32_t S3, uint32_t S4) noexcept {
            InitialState[0] = S1;
            InitialState[1] = S2;
            InitialState[2] = S3;
            InitialState[3] = S4;
            Reset();
        }

        void Reset() noexcept {
            memcpy(State, InitialState, sizeof(uint32_t[4]));
            BytesRead = 0;
            memset(MessageQueue, 0, sizeof(MessageQueue));
        }

        void Update(const void* lpData, size_t cbData) noexcept {
            const uint8_t* pbData = reinterpret_cast<const uint8_t*>(lpData);
            size_t MessageQueueLength = BytesRead % BlockSizeValue;
            size_t UnprocessedBytes = MessageQueueLength + cbData;
            if (cbData <= UnprocessedBytes && UnprocessedBytes <= BlockSizeValue) {
                // In this branch, we can just save data to be hashed to MessageQueue and wait it to be processed.

                memcpy(MessageQueue + MessageQueueLength, pbData, cbData);
                if (UnprocessedBytes == BlockSizeValue) {
                    Utility::ProcessBlock(State, MessageQueue);
                }

                BytesRead += cbData;
            } else {
                // In this branch, we have
                //   "cbData > UnprocessedBytes" or "UnprocessedBytes > BlockSizeValue", which means `cbData` is large
                //   and we have to process one block by one block.

                if (MessageQueueLength) {
                    size_t BytesToRead = BlockSizeValue - MessageQueueLength;
                    memcpy(MessageQueue + MessageQueueLength, pbData, BytesToRead);
                    Utility::ProcessBlock(State, MessageQueue);
                    pbData += BytesToRead;
                    cbData -= BytesToRead;
                    BytesRead += BytesToRead;
                }

                if (reinterpret_cast<uintptr_t>(pbData) % sizeof(uint32_t) == 0) {
                    // In this case, pbData is aligned to 4 bytes.
                    // We can just process it in place.
                    while (cbData >= BlockSizeValue) {
                        Utility::ProcessBlock(State, pbData);
                        pbData += BlockSizeValue;
                        cbData -= BlockSizeValue;
                        BytesRead += BlockSizeValue;
                    }
                } else {
                    while (cbData >= BlockSizeValue) {
                        memcpy(MessageQueue, pbData, BlockSizeValue);
                        Utility::ProcessBlock(State, MessageQueue);
                        pbData += BlockSizeValue;
                        cbData -= BlockSizeValue;
                        BytesRead += BlockSizeValue;
                    }
                }

                if (cbData) {
                    memcpy(MessageQueue, pbData, cbData);
                    BytesRead += cbData;
                }
            }
        }

        void Evaluate(void* lpDigest) const noexcept {
            uint32_t ForkedState[4];
            memcpy(ForkedState, State, sizeof(ForkedState));

            alignas(uint32_t) uint8_t PaddedTailData[2 * BlockSizeValue] = {};
            size_t MessageQueueLength = BytesRead % BlockSizeValue;

            memcpy(PaddedTailData, MessageQueue, MessageQueueLength);
            PaddedTailData[MessageQueueLength] = 0x80;
            if (MessageQueueLength >= BlockSizeValue - sizeof(uint64_t)) {
                reinterpret_cast<uint64_t*>(
                    PaddedTailData + 2 * BlockSizeValue - sizeof(uint64_t)
                    )[0] = BytesRead * 8;
                Utility::ProcessBlock(ForkedState, PaddedTailData);
                Utility::ProcessBlock(ForkedState, PaddedTailData + BlockSizeValue);
            } else {
                reinterpret_cast<uint64_t*>(
                    PaddedTailData + BlockSizeValue - sizeof(uint64_t)
                    )[0] = BytesRead * 8;
                Utility::ProcessBlock(ForkedState, PaddedTailData);
            }

            memcpy(lpDigest, ForkedState, sizeof(ForkedState));
        }

        [[nodiscard]]
        DigestType Evaluate() const {
            std::vector<uint8_t> Digest(DigestSizeValue);
            Evaluate(Digest.data());
            return Digest;
        }

        void Destroy() noexcept {
            memset(InitialState, 0, sizeof(InitialState));
            memset(State, 0, sizeof(State));
            BytesRead = 0;
            memset(MessageQueue, 0, sizeof(MessageQueue));
        }
    };

    struct InitByDefault {
        using TraitsType = HasherMd5Traits;

        [[nodiscard]]
        static Context Impl() noexcept {
            Context NewCtx;
            NewCtx.Initialize();
            return NewCtx;
        }
    };

    struct InitByCustomInitialValue {
        using TraitsType = HasherMd5Traits;

        [[nodiscard]]
        static Context Impl(uint32_t S1, uint32_t S2, uint32_t S3, uint32_t S4) noexcept {
            Context NewCtx;
            NewCtx.Initialize(S1, S2, S3, S4);
            return NewCtx;
        }
    };
};
