#include <tchar.h>
#include <stdio.h>
#include <windows.h>
#include <xstring.hpp>
#include <VisualAssistCryptoConfig.hpp>
#include "VisualAssistKeygen.hpp"

void Help() {
    _putts(TEXT("Usage:"));
    _putts(TEXT("    VisualAssist-keygen.exe <username> <license count> <expire date>"));
    _putts(TEXT(""));
    _putts(TEXT("        <username>         The username licensed to."));
    _putts(TEXT("                           This parameter must be specified."));
    _putts(TEXT(""));
    _putts(TEXT("        <license count>    The maximum number of copies that can be run under the newly-generated license."));
    _putts(TEXT("                           The value should be in 1 ~ 65536."));
    _putts(TEXT("                           This parameter must be specified."));
    _putts(TEXT(""));
    _putts(TEXT("        <expire date>      The date when the newly-generated license expires."));
    _putts(TEXT("                           The format must be one of the following:"));
    _putts(TEXT("                               1. \"yyyy/mm/dd\""));
    _putts(TEXT("                               2. \"yyyy-mm-dd\""));
    _putts(TEXT("                               3. \"yyyy.mm.dd\""));
    _putts(TEXT("                           This parameter must be specified."));
    _putts(TEXT(""));
}

int _tmain(int argc, PTSTR argv[]) {
    if (argc == 4) {
        try {
            uint32_t Year;
            uint32_t Month;
            uint32_t Day;
            if (_stscanf_s(argv[3], TEXT("%u/%u/%u"), &Year, &Month, &Day) != 3) {
                if (_stscanf_s(argv[3], TEXT("%u-%u-%u"), &Year, &Month, &Day) != 3) {
                    if (_stscanf_s(argv[3], TEXT("%u.%u.%u"), &Year, &Month, &Day) != 3) {
                        throw std::invalid_argument("Please specify a date with correct format.");
                    }
                }
            }

            auto Info = VisualAssistKeygen<VisualAssistCryptoConfig>::GenerateRegisterInfo(
                std::xstring(argv[1]).explicit_string().c_str(), 
                _tcstol(argv[2], nullptr, 0), 
                Year, 
                Month, 
                Day
            );

            _tprintf_s(TEXT("%hs\n"), Info.KeyName.c_str());
            _tprintf_s(TEXT("%hs\n"), Info.KeyCode.c_str());
            return 0;
        } catch (std::exception& e) {
            _tprintf_s(TEXT("Internal Error: %hs\n"), e.what());
        }
    } else {
        Help();
        return -1;
    }
}
