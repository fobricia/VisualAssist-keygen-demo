#pragma once
#include "BigInteger.hpp"
#include "GaloisField.hpp"
#include "EllipticCurveGF2m.hpp"
#include "VisualAssistFieldTraits.hpp"
#include "VisualAssistRandomGenerator.hpp"

#include "Hasher.hpp"
#include "HasherMd5Traits.hpp"
#include <algorithm>

struct VisualAssistCryptoConfig {
private:

    static EllipticCurveGF2m<GaloisField<VisualAssistFieldTraits>>::Point GenerateBasePoint(uint32_t Seed) {
        VisualAssistRandomGenerator Rnd(Seed);
        GaloisField<VisualAssistFieldTraits> RandomFieldValue;
        uint32_t RawRandomFieldValue[4];

        RawRandomFieldValue[3] = Rnd.NextRandomNumber() & 0x1ffff;
        RawRandomFieldValue[2] = Rnd.NextRandomNumber();
        RawRandomFieldValue[1] = Rnd.NextRandomNumber();
        RawRandomFieldValue[0] = Rnd.NextRandomNumber();

        uint32_t RootChooser = RawRandomFieldValue[0] & 1;

        while (true) {
            RandomFieldValue.SetValue(
                _mm_set_epi32(RawRandomFieldValue[3], RawRandomFieldValue[2], RawRandomFieldValue[1], RawRandomFieldValue[0])
            );

            // t = x^3 + a * x^2 + b
            //   = (x + a) * x^2 + b
            auto t = RandomFieldValue + Curve.GetParameterA();
            t *= RandomFieldValue.SquareValue();
            t += Curve.GetParameterB();

            auto Roots = GaloisField<VisualAssistFieldTraits>::SolveQuadratic({ GaloisFieldInitByOne{} }, RandomFieldValue, t);
            if (Roots.size() != 2) {
                ++RawRandomFieldValue[0];
                continue;
            }

            return Curve.CreatePoint(RandomFieldValue, Roots[RootChooser]) * Cofactor;  // cofactor = 2
        }
    }

    static std::string GeneratePublicKeyString(uint32_t BasePointGenerator, const EllipticCurveGF2m<GaloisField<VisualAssistFieldTraits>>::Point& PublicKey) {
        BigInteger Px(false, PublicKey.GetX().Serialize(), BigIntegerEndian::Little);
        BigInteger Py(false, PublicKey.GetY().Serialize(), BigIntegerEndian::Little);
        return std::to_string(BasePointGenerator)
            .append(1, ',')
            .append(Px.ToString(10, false))
            .append(1, ',')
            .append(Py.ToString(10, false));
    }

    static uint32_t GeneratePublicKeyStringMd5(const std::string& PublicKeyString) {
        auto s = PublicKeyString;
        Hasher Md5(HasherMd5Traits::InitByDefault{});
        uint32_t Md5Digest[4];

        std::reverse(s.begin(), s.end());

        Md5.Update(s.data(), s.length());
        Md5.Evaluate(Md5Digest);

        return Md5Digest[0] ^ Md5Digest[1] ^ Md5Digest[2] ^ Md5Digest[3];
    }

public:

    static inline const EllipticCurveGF2m<GaloisField<VisualAssistFieldTraits>> Curve{
        GaloisField<VisualAssistFieldTraits>{ GaloisFieldInitByOne{} },
        GaloisField<VisualAssistFieldTraits>{ GaloisFieldInitByOne{} },
    };

    static inline const BigInteger Order = "0xfffffffffffffffdbf91af6dea73";

    static inline const BigInteger Cofactor = 2;

    static inline const size_t NumberOfKeys = 2;

    struct Official {

        static inline const uint32_t Sym[] = {
            0x3199A66F,
            0x3297855E
        };

        static inline const uint32_t BasePointGenerator[] = {
            1329115615,
            4065234961
        };

        static inline const EllipticCurveGF2m<GaloisField<VisualAssistFieldTraits>>::Point G[] = {
            GenerateBasePoint(BasePointGenerator[0]),
            GenerateBasePoint(BasePointGenerator[1])
        };

        //
        // Of course, we don't have :-)
        //
        // static inline const BigInteger PrivateKey[] = {
        //     "",
        //     ""
        // };

        static inline const EllipticCurveGF2m<GaloisField<VisualAssistFieldTraits>>::Point PublicKey[] = {
            Curve.CreatePoint(
                { GaloisFieldInitByElement{}, _mm_set_epi32(0x1daa0, 0xd314df6c, 0x689c33e7, 0x6c94a943) }, 
                { GaloisFieldInitByElement{}, _mm_set_epi32(0x0aac7, 0x0f8ba549, 0xc3beacf6, 0xbd563e16) }
            ),
            Curve.CreatePoint(
                { GaloisFieldInitByElement{}, _mm_set_epi32(0x06d83, 0xe7aea424, 0x81e82dcc, 0x261f6b1e) },
                { GaloisFieldInitByElement{}, _mm_set_epi32(0x09c8c, 0xb5521d09, 0x87b104f9, 0x0ce203b8) }
            )
        };

        static inline const std::string PublicKeyString[] = {
            "1329115615,9626603984703850283064885442292035,3463780848057510008753765087591958",
            "4065234961,2221233238252903594850812155620126,3175203956977476891557515669668792"
        };

        static inline const uint32_t PublicKeyStringMd5[] = {
            0x04993A77,
            0xA97CA8A9
        };

    };

    struct Custom {

        static inline const uint32_t Sym[] = {
            0x3199A66F,
            0x3297855E
        };

        static inline const uint32_t BasePointGenerator[] = {
            2127088620,     // Armadillo Encrypt Template = "3"
            2127088620      // Armadillo Encrypt Template = "3"
        };

        static inline const EllipticCurveGF2m<GaloisField<VisualAssistFieldTraits>>::Point G[] = {
            GenerateBasePoint(BasePointGenerator[0]),
            GenerateBasePoint(BasePointGenerator[1])
        };

        static inline const BigInteger PrivateKey[] = {
            "0x2def66c7f63c047c2e7af50b55e6",       // 0x2def66c7f63c047c2e7aad777e6e + 0x000000004793d778
            "0x2def66c7f63c047c2e7ca2948191"        // 0x2def66c7f63c047c2e7aad777e6e + 0x00000001f51d0323
        };

        static inline const EllipticCurveGF2m<GaloisField<VisualAssistFieldTraits>>::Point PublicKey[] = {
            G[0] * PrivateKey[0],
            G[1] * PrivateKey[1]
        };

        static inline const std::string PublicKeyString[] = {
            GeneratePublicKeyString(BasePointGenerator[0], PublicKey[0]),
            GeneratePublicKeyString(BasePointGenerator[1], PublicKey[1])
        };

        static inline const uint32_t PublicKeyStringMd5[] = {
            GeneratePublicKeyStringMd5(PublicKeyString[0]),
            GeneratePublicKeyStringMd5(PublicKeyString[1])
        };

    };

};

