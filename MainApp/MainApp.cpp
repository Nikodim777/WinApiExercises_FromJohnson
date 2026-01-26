#include "fcntl.h"
#include "io.h"
#include "stdio.h"
#include "windows.h"

#include <array>
#include <chrono>
#include <iostream>
#include <functional>
#include <string>

#include "GeneralFunctions.h"

enum class CommandCode
{
    CP_TEXT_C,
    CP_C,
    CP_CPP,
    CP_C_WIN,
    CP_C_DIFF,
    __Count
};

static constexpr std::array<PCWSTR, static_cast<int>(CommandCode::__Count)> a_wszCommands = { L"cpTextC", L"cpC", L"cpCpp", L"cpCWin", L"cpCDiff"};
static constexpr std::array<size_t, static_cast<int>(CommandCode::__Count)> a_cArgs = { 4, 4, 4, 4, 5 };

VOID PrintHelp()
{
    std::wcout << L"Варианты использования:" << std::endl <<
        L"MainApp cpTextC SRC DST - копирование utf-16 файла SRC в файл DST, реализация на c." << std::endl <<
        L"MainApp cpC SRC DST - копирование файла SRC в файл DST, реализация на с." << std::endl <<
        L"MainApp cpCpp SRC DST - копирование файла SRC в файл DST, реализация на с++." << std::endl <<
        L"MainApp cpCWin SRC DST - копирование файла SRC в файл DST, реализация на с с использованием winapi." << std::endl <<
        L"MainApp cpCDiff SRC DST IS_TB - копирование файла SRC в файл DST в разных режимах, реализация на с." << std::endl <<
        L"Если IS_TB = true, SRC открывается в текстовом режиме UNICODE, а DST - в бинарном режиме; иначе - наоборот." << std::endl;
}

int wmain(DWORD argc, PCWSTR argv[])
{
    _setmode(_fileno(stdin), _O_U16TEXT);
    _setmode(_fileno(stdout), _O_U16TEXT);
    _setmode(_fileno(stderr), _O_U16TEXT);
    if (argc < 2)
    {
        PrintHelp();
        return ERROR_BAD_ARGUMENTS;
    }

    std::array<std::function<void()>, static_cast<int>(CommandCode::__Count)> a_Functions = {
        [&argv]() { CopyTextFileC(argv[2], argv[3]); },
        [&argv]() { CopyFileC(argv[2], argv[3]); },
        [&argv]() { CopyFileCpp(argv[2], argv[3]); },
        [&argv]() { CopyFileCWin(argv[2], argv[3]); },
        [&argv]() { CopyFileCDiff(argv[2], argv[3], !wcscmp(argv[4], L"true")); }
    };

    static_assert(static_cast<size_t>(CommandCode::__Count) == a_wszCommands.size());
    static_assert(a_cArgs.size() == a_wszCommands.size());
    static_assert(a_Functions.size() == a_wszCommands.size());

    size_t i = 0;
    for( ;i < static_cast<size_t>(CommandCode::__Count); i++)
    {
        if (wcscmp(argv[1], a_wszCommands[i]))
            continue;

        if (argc < a_cArgs[i])
        {
            PrintHelp();
            return ERROR_BAD_ARGUMENTS;
        }

        auto start = std::chrono::high_resolution_clock::now();
        a_Functions[i]();
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        std::wcout << L"Время выполнения " << duration.count() << L" микросекунд." << std::endl;
        break;
    }

    if (i == static_cast<size_t>(CommandCode::__Count))
    {
        PrintHelp();
    }
}
