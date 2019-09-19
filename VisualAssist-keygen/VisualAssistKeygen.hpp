#pragma once
#include <windows.h>
#include <winternl.h>
#include <bcrypt.h>

#include <BigInteger.hpp>
#include <Hasher.hpp>
#include <HasherMd5Traits.hpp>
#include <HasherCrc32Traits.hpp>

#include <stdio.h>
#include <algorithm>
#include <string>
#include <vector>
#include <map>
#include <utility>
#include <system_error>

#pragma comment(lib, "ntdll")
#pragma comment(lib, "bcrypt")

template<typename __ConfigType>
class VisualAssistKeygen {
private:

    static inline const auto& Order         = __ConfigType::Order;
    static inline const auto& Sym           = __ConfigType::Custom::Sym[1];
    static inline const auto& G             = __ConfigType::Custom::G[1];
    static inline const auto& PrivateKey    = __ConfigType::Custom::PrivateKey[1];
    static inline const auto& PublicKey     = __ConfigType::Custom::PublicKey[1];

    struct ECCSignature {
        BigInteger r;
        BigInteger s;
    };

    static BigInteger GenerateHashInteger(const void* lpMessage, size_t cbMessage) {
        uint32_t RawHash[4];
        Hasher Md5(HasherMd5Traits::InitByDefault{});

        Md5.Update(lpMessage, cbMessage);
        Md5.Evaluate(RawHash);

        std::swap(RawHash[0], RawHash[3]);
        std::swap(RawHash[1], RawHash[2]);

        return BigInteger(false, RawHash, sizeof(RawHash), BigIntegerEndian::Little);
    }

    static BigInteger GenerateRandom() {
        uint8_t RawRandom[32];

        auto ntStatus = BCryptGenRandom(NULL, RawRandom, sizeof(RawRandom), BCRYPT_USE_SYSTEM_PREFERRED_RNG);
        if (!BCRYPT_SUCCESS(ntStatus)) {
            throw std::system_error(RtlNtStatusToDosError(ntStatus), std::system_category());
        }

        BigInteger Random(false, RawRandom, sizeof(RawRandom), BigIntegerEndian::Little);
        Random %= Order;

        return Random;
    }

    static ECCSignature Sign(const void* lpMessage, size_t cbMessage) {
        ECCSignature Signature;
        while (true) {
            BigInteger Rnd = GenerateRandom();
            auto R = G * Rnd;

            Signature.r.Load(false, R.GetX().Serialize(), BigIntegerEndian::Little);
            Signature.r %= Order;
            if (Signature.r.IsZero()) {
                continue;
            }

            BigInteger h = GenerateHashInteger(lpMessage, cbMessage);
            Signature.s = h + Signature.r * PrivateKey;
            Signature.s *= Rnd.InverseModValue(Order);
            Signature.s %= Order;
            if (Signature.s.IsZero()) {
                continue;
            }

            return Signature;
        }
    }

    static bool Verify(const void* lpMessage, size_t cbMessage, const ECCSignature& Signature) {
        if (Signature.r < 1 || Order <= Signature.r) {
            return false;
        }

        if (Signature.s < 1 || Order <= Signature.s) {
            return false;
        }

        auto h = GenerateHashInteger(lpMessage, cbMessage);
        auto s_inv = Signature.s.InverseModValue(Order);
        auto u1 = h * s_inv;
        auto u2 = Signature.r * s_inv;
        u1 %= Order;
        u2 %= Order;

        auto R = G * u1 + PublicKey * u2;
        if (R.IsAtInfinity()) {
            return false;
        }

        BigInteger RxInteger(false, R.GetX().Dump(), true);
        RxInteger %= Order;

        return RxInteger == Signature.r;
    }

    static std::string RemoveSpaceAndUpper(const std::string& String) {
        std::string NewString = String;
        auto endpos = std::remove(NewString.begin(), NewString.end(), ' ');
        NewString.erase(endpos, NewString.end());
        std::transform(NewString.begin(), NewString.end(), NewString.begin(), ::toupper);
        return NewString;
    }

    static uint16_t ArmadilloMakeDate(uint16_t Year, uint16_t Month, uint16_t Day) {
        constexpr int64_t UNIX_TIME_START = 0x019DB1DED53E8000;
        constexpr int64_t TICKS_PER_SECOND = 10000000;

        SYSTEMTIME stUnixTimestampZero = {};
        FILETIME ftUnixTimestampZero = {};

        stUnixTimestampZero.wYear = 1970;
        stUnixTimestampZero.wMonth = 1;
        stUnixTimestampZero.wDay = 1;
        if (SystemTimeToFileTime(&stUnixTimestampZero, &ftUnixTimestampZero) == FALSE) {
            auto err = GetLastError();
            throw std::system_error(err, std::system_category());
        }

        SYSTEMTIME st = {};
        FILETIME ft = {};
        st.wYear = Year;
        st.wMonth = Month;
        st.wDay = Day;
        if (SystemTimeToFileTime(&st, &ft) == FALSE) {
            auto err = GetLastError();
            throw std::system_error(err, std::system_category());
        }

        ULARGE_INTEGER UnixTimestampZero;
        ULARGE_INTEGER UnixTimestamp;
        UnixTimestampZero.LowPart = ftUnixTimestampZero.dwLowDateTime;
        UnixTimestampZero.HighPart = ftUnixTimestampZero.dwHighDateTime;
        UnixTimestamp.LowPart = ft.dwLowDateTime;
        UnixTimestamp.HighPart = ft.dwHighDateTime;
        UnixTimestamp.QuadPart = (UnixTimestamp.QuadPart - UnixTimestampZero.QuadPart) / 10000000;

        ULONGLONG Daystamp = UnixTimestamp.QuadPart / (24 * 3600);
        ULONGLONG Daystamp19990101 = 10592;

        if (Daystamp < Daystamp19990101) {
            throw std::invalid_argument("The date specified is too early.");
        }

        if (Daystamp - Daystamp19990101 > 0xffff) {
            throw std::invalid_argument("The date specified is too late.");
        }

        return static_cast<uint16_t>(Daystamp - Daystamp19990101);
    }

    static std::string ArmadilloMakeKeyCode(const std::vector<uint8_t>& KeyCodeData) {
        BigInteger n(false, KeyCodeData, BigIntegerEndian::Big);
        std::string str_n = n.ToString(32, false);
        std::string KeyCode;

        const char* from = "0123456789ABCDEFGHIJKLMNOPQRSTUV";
        const char* to = "0123456789ABCDEFGHJKMNPQRTUVWXYZ";
        std::map<char, char> trans;
        for (size_t i = 0; from[i]; ++i) {
            trans[from[i]] = to[i];
        }

        for (size_t i = 0; i < str_n.length(); ++i) {
            str_n[i] = trans[str_n[i]];
        }

        str_n.insert(str_n.begin(), '1');
        while (str_n.length() % 6) {
            str_n.insert(str_n.begin(), '0');
        }

        for (size_t i = 0; i < str_n.length(); i += 6) {
            KeyCode.append(str_n.begin() + i, str_n.begin() + i + 6);
            if (i + 6 != str_n.length()) {
                KeyCode.append(1, '-');
            }
        }

        return KeyCode;
    }

    template<typename... ArgTypes>
    static inline std::string StringFormat(const char* lpszFormat, ArgTypes&& ... Args) {
        std::string s(static_cast<size_t>(snprintf(nullptr, 0, lpszFormat, std::forward<ArgTypes>(Args)...)) + 1, '\x00');

        snprintf(s.data(), s.size(), lpszFormat, std::forward<ArgTypes>(Args)...);
        while (s.back() == '\x00') {
            s.pop_back();
        }

        return s;
    }

public:

    struct RegisterInfo {
        std::string KeyName;
        std::string KeyCode;
        std::string KeyNameCooked;
        std::vector<uint8_t> KeyCodeData;
        std::vector<uint8_t> DataTBS;
    };

    static RegisterInfo GenerateRegisterInfo(const char* lpszUserName, uint32_t LicenseCount, uint32_t Year, uint32_t Month, uint32_t Day) {
        for (size_t i = 0; lpszUserName[i]; ++i) {
            if ((0x20 <= lpszUserName[i] && lpszUserName[i] < 0x7f) == false) {
                throw std::invalid_argument("Each chars in username must be printable ASCII chars.");
            }
        }

        if (LicenseCount == 0 || LicenseCount > 0xffff) {
            throw std::invalid_argument("LicenseCount must be in 1 ~ 65535.");
        }

        SYSTEMTIME CurrentSystemTime;
        GetLocalTime(&CurrentSystemTime);

        if (CurrentSystemTime.wYear > Year && CurrentSystemTime.wMonth > Month && CurrentSystemTime.wDay > Day) {
            throw std::invalid_argument("Don't use date that is early than current date.");
        }

        if (Year < 2000 || 2000 + 127 < Year) {
            throw std::invalid_argument("Year must be in 2000 ~ 2127.");
        }
        
        uint16_t SupportEndDateInfo;
        if (Year % 4 == 0 && Year % 100 != 0 || Year % 400 == 0) {  // is leap year
            const uint8_t PossibleDays[12] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
            if (1 <= Month && Month <= 12 && 1 <= Day && Day <= PossibleDays[Month - 1]) {
                SupportEndDateInfo = (Year - 2000) | (Month << 7) | (Day << 11);
            } else {
                throw std::invalid_argument("Invalid month or day.");
            }
        } else {
            const uint8_t PossibleDays[12] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
            if (1 <= Month && Month <= 12 && 1 <= Day && Day <= PossibleDays[Month - 1]) {
                SupportEndDateInfo = (Year - 2000) | (Month << 7) | (Day << 11);
            } else {
                throw std::invalid_argument("Invalid month or day.");
            }
        }

        uint16_t CurrentArmadilloDate = ArmadilloMakeDate(CurrentSystemTime.wYear, CurrentSystemTime.wMonth, CurrentSystemTime.wDay);

        RegisterInfo Info;
        Info.KeyName = StringFormat("%s (%u-user license) Support ends %u.%u.%u", lpszUserName, LicenseCount, Year, Month, Day);
        Info.KeyNameCooked = RemoveSpaceAndUpper(Info.KeyName);
        Info.DataTBS.insert(Info.DataTBS.end(), reinterpret_cast<const uint8_t*>(&SupportEndDateInfo)[1]);
        Info.DataTBS.insert(Info.DataTBS.end(), reinterpret_cast<const uint8_t*>(&SupportEndDateInfo)[0]);
        Info.DataTBS.insert(Info.DataTBS.end(), reinterpret_cast<const uint8_t*>(&LicenseCount)[1]);
        Info.DataTBS.insert(Info.DataTBS.end(), reinterpret_cast<const uint8_t*>(&LicenseCount)[0]);
        Info.DataTBS.insert(Info.DataTBS.end(), reinterpret_cast<const uint8_t*>(&CurrentArmadilloDate)[1]);
        Info.DataTBS.insert(Info.DataTBS.end(), reinterpret_cast<const uint8_t*>(&CurrentArmadilloDate)[0]);
        Info.DataTBS.insert(Info.DataTBS.end(), reinterpret_cast<const uint8_t*>(&Sym)[3]);
        Info.DataTBS.insert(Info.DataTBS.end(), reinterpret_cast<const uint8_t*>(&Sym)[2]);
        Info.DataTBS.insert(Info.DataTBS.end(), reinterpret_cast<const uint8_t*>(&Sym)[1]);
        Info.DataTBS.insert(Info.DataTBS.end(), reinterpret_cast<const uint8_t*>(&Sym)[0]);
        Info.DataTBS.insert(
            Info.DataTBS.end(),
            reinterpret_cast<const uint8_t*>(Info.KeyNameCooked.data()),
            reinterpret_cast<const uint8_t*>(Info.KeyNameCooked.data()) + Info.KeyNameCooked.length()
        );

        Hasher Crc32(HasherCrc32Traits<0xEDB88320>::InitByDefault{});
        Crc32.Update(Info.KeyNameCooked.data(), Info.KeyNameCooked.length());
        VisualAssistRandomGenerator RndGen(~Crc32.Evaluate());
        for (size_t i = 0; i < 10; ++i) {
            Info.DataTBS[i] ^= RndGen.NextRandomRange(256);
        }

        auto Signature = Sign(Info.DataTBS.data(), Info.DataTBS.size());
        auto SignatureBytesR = Signature.r.DumpAbsoluteValue(BigIntegerEndian::Little);
        auto SignatureBytesS = Signature.s.DumpAbsoluteValue(BigIntegerEndian::Little);
        Info.KeyCodeData.insert(Info.KeyCodeData.end(), 0x01);
        Info.KeyCodeData.insert(Info.KeyCodeData.end(), Info.DataTBS.begin(), Info.DataTBS.begin() + 10);
        Info.KeyCodeData.insert(Info.KeyCodeData.end(), SignatureBytesS.begin(), SignatureBytesS.end());
        Info.KeyCodeData.insert(Info.KeyCodeData.end(), SignatureBytesR.begin(), SignatureBytesR.end());

        Info.KeyCode = ArmadilloMakeKeyCode(Info.KeyCodeData);

        return Info;
    }
};

