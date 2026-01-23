#include "fcntl.h"
#include "io.h"
#include "stdio.h"
#include "windows.h"

#include <array>
#include <string>
#include <iostream>
#include <functional>

#include "GeneralFunctions.h"

enum class CommandCode
{
    CP,
    __Count
};

VOID PrintHelp()
{
    std::wcout << L"Варианты использования:" << std::endl <<
        L"MainApp cpTextC SRC DST - копирование файла SRC в файл DST." << std::endl;
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

    std::array<PCWSTR, static_cast<int>(CommandCode::__Count)> a_wszCommands = { L"cpTextC" };
    std::array<size_t, static_cast<int>(CommandCode::__Count)> a_cArgs = { 4 };
    std::array<std::function<void()>, static_cast<int>(CommandCode::__Count)> a_Functions = { 
        [&argv]() { CopyTextFileC(argv[2], argv[3]); }
    };

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

        a_Functions[i]();
        break;
    }

    if (i == static_cast<size_t>(CommandCode::__Count))
    {
        PrintHelp();
    }
}
