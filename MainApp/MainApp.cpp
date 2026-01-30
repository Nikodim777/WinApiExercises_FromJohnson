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
    CP_C_WIN_NO_BUF,
    CP_C_DIFF,
    CP_WIN_AUX,
    PRINT_STRS,
    PROMPT,
    CAT,
    ANSII2UNICODE,
    PWD,
    OPTIONS,
    SORT,
    __Count
};

static constexpr std::array<PCWSTR, static_cast<int>(CommandCode::__Count)> a_wszCommands = { { L"cpTextC", L"cpC", L"cpCpp", L"cpCWin", L"cpCWinNB", L"cpCDiff",
    L"cpWinAux", L"printStrs", L"prompt", L"cat", L"a2u", L"pwd", L"opt", L"sort"}};
static constexpr std::array<size_t, static_cast<int>(CommandCode::__Count)> a_cArgs = { { 4, 4, 4, 5, 5, 5, 
    4, 6, 3, 2, 5, 2, 2, 3 } };

VOID PrintHelp()
{
    std::wcout << L"Варианты использования:" << std::endl <<
        L"MainApp cpTextC SRC DST - копирование utf-16 файла SRC в файл DST, реализация на c." << std::endl <<
        L"MainApp cpC SRC DST - копирование файла SRC в файл DST, реализация на с." << std::endl <<
        L"MainApp cpCpp SRC DST - копирование файла SRC в файл DST, реализация на с++." << std::endl <<
        L"MainApp cpCWin SRC DST - копирование файла SRC в файл DST, реализация на с с использованием winapi." << std::endl <<
        L"MainApp cpCWin SRC DST - копирование файла SRC в файл DST, реализация на с с использованием winapi без системного буфера." << std::endl <<
        L"Тома SRC и DST должны быть одинаковы, размер сектора определяется по файлу DST." << std::endl <<
        L"MainApp cpCDiff SRC DST IS_TB - копирование файла SRC в файл DST в разных режимах, реализация на с." << std::endl <<
        L"Если IS_TB = true, SRC открывается в текстовом режиме UNICODE, а DST - в бинарном режиме; иначе - наоборот." << std::endl <<
        L"MainApp cpWinAux SRC DST - копирование файла SRC в файл DST, с использованием вспомогательной функции winapi." << std::endl <<
        L"MainApp printStrs PATH STR1 STR2 STR3 - вывод нескольких строк STR, в файл или на консоль PATH." << std::endl <<
        L"MainApp prompt PROMPT IS_NEED_ECHO - выводит запрос PROMPT, считывает и выводит ответ пользователя с эхом ввода или без." << std::endl <<
        L"MainApp cat [-s] [files...] - выводит содержимое файлов. если указан флаг s - не выводятся сообщения об ошибках," << std::endl <<
        L"если файл уже существует. Если файлов не передано - выводится входной поток." << std::endl <<
        L"MainApp a2u SRC DST IS_RW - преобразование ASCII файла SRC в UNICODE файл DST." << std::endl <<
        L"Если IS_RW = true и DST уже существует, - DST перезаписыватся, иначе выводится вопрос о перезаписи." << std::endl <<
        L"Работает только с базовыми ASCII символами." << std::endl <<
        L"MainApp pwd - вывод текущей директории." << std::endl <<
        L"MainApp opt [flags] [args]. flags - флаги(-x -t -y); args - любые аргументы без -." << std::endl <<
        L"MainApp sort EN_US - тест сортировки строк с учётом текущей локали, если EN_US = true - используется локаль en-US." << std::endl;
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
        [&argv]() { DWORD dwFlags = 0;
            if (!wcscmp(argv[4], L"seq")) dwFlags |= FILE_FLAG_SEQUENTIAL_SCAN;
            CopyFileCWin(argv[2], argv[3], dwFlags); },
        [&argv]() { DWORD dwFlags = 0;
            if (!wcscmp(argv[4], L"seq")) dwFlags |= FILE_FLAG_SEQUENTIAL_SCAN;
            CopyFileCWinNoBuf(argv[2], argv[3], dwFlags); },
        [&argv]() { CopyFileCDiff(argv[2], argv[3], !wcscmp(argv[4], L"true")); },
        [&argv]() { CopyFileWinAux(argv[2], argv[3]); },
        [&argv]() { HANDLE hFile = CreateFile(argv[2], GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, 0, NULL);
            PrintStrs(hFile, argv[3], L"\n", argv[4], L"\n", argv[5], L"\n", NULL);
            CloseHandle(hFile); },
        [&argv]() { WCHAR wszResponse[16];
            Prompt(argv[2], 16, wszResponse, !wcscmp(argv[3], L"true"));
            std::wcout << wszResponse << std::endl << GetLastError() << std::endl; },
        [&argc, &argv]() { BOOL bSilence = argc > 2 && !wcscmp(argv[2], L"-s");
            DWORD nFirstFile = 2lu + (bSilence ? 1lu : 0lu);
            CatFiles(argc - nFirstFile, &argv[nFirstFile], bSilence); },
        [&argv]() { AnsiToUnicode(argv[2], argv[3], !wcscmp(argv[4], L"true")); },
        []() { PrintCurrentDir(); },
        [&argc, &argv]() { BOOL bX, bT, bY;
            DWORD dwFirstArg = GetOptions(argc - 1 , &argv[1], L"txy", TRUE, &bT, &bX, &bY);
            std::wcout << bX << bT << bY << std::endl;
            for (DWORD dwIter = dwFirstArg + 1; dwIter < argc; dwIter++) { std::wcout << argv[dwIter]; }
            std::wcout << std::endl; },
        [&argv]() { PCWSTR strs[] = { L"откуда-то", L"we'ya", L"were", L"да", L"we're", L"Дак"};
            StringsSort(6, strs, !wcscmp(argv[2], L"true"));
            for (DWORD dwIter = 0; dwIter < 6; dwIter++) std::wcout << strs[dwIter] << std::endl; }
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
