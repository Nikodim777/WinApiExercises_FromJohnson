#include <io.h>
#include <stdio.h>
#include <fcntl.h>

#include <windows.h>
#include <iostream>

#include "GeneralFunctions.h"

typedef struct Header
{
    DWORD cRecords = 0;
    DWORD cValidRecords = 0;
} SHeader, *PSHeader;

typedef struct Record
{
    DWORD cRef = 0;
    SYSTEMTIME tCreateTime = { 0 };
    SYSTEMTIME tRefTime = { 0 };
    SYSTEMTIME tUpdateTime = { 0 };
    WCHAR wszData[MAX_PATH] = { 0 };
} SRecord, *PSRecord;

INT wmain(DWORD argc, PCWSTR argv[])
{
    _setmode(_fileno(stdin), _O_U16TEXT);
    _setmode(_fileno(stdout), _O_U16TEXT);
    _setmode(_fileno(stderr), _O_U16TEXT);

    DWORD dwCreateFlag = argc > 2 ? CREATE_NEW : OPEN_EXISTING;
    HANDLE hFile = INVALID_HANDLE_VALUE;
    LARGE_INTEGER liCurrentPos = { 0 };
    OVERLAPPED ovrlpd = { 0, 0, 0, 0, NULL };
    OVERLAPPED ovrlpdZero = { 0, 0, 0, 0, NULL };
    SYSTEMTIME tSysTime = { 0 };
    Header sHeader = { 0 };
    Record sRecord = { 0 };
    DWORD cbRead = 0;
    DWORD cbWritten = 0;
    DWORD dwPos = 0;
    BOOL bResult = FALSE;

    if (argc < 2)
        ReportErrorAndExit(L"Недостаточно аргументов: RecordManager FilePath [NumRecords]", 1, FALSE);
    
    hFile = CreateFile(argv[1], FILE_READ_ACCESS | FILE_WRITE_ACCESS, 0, NULL, dwCreateFlag, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile == INVALID_HANDLE_VALUE)
        ReportErrorAndExit(L"Не удалось открыть файл", 1, TRUE);

    if (argc > 2)
    {
        sHeader.cRecords = wcstoul(argv[2], NULL, 10);
        if (sHeader.cRecords == 0)
            ReportErrorAndExit(L"Второй аргумент должен быть числом! Полученное значение - %d", 1, FALSE, errno);
        
        bResult = WriteFile(hFile, &sHeader, sizeof(SHeader), &cbWritten, NULL);
        if (!bResult)
            ReportErrorAndExit(L"Ошибка записи заголовка", 1, TRUE);

        liCurrentPos.QuadPart = sHeader.cRecords * sizeof(SRecord);
        dwPos = SetFilePointer(hFile, liCurrentPos.LowPart, &liCurrentPos.HighPart, FILE_BEGIN);
        if (dwPos == ULONG_MAX && GetLastError != NO_ERROR)
            ReportErrorAndExit(L"Ошибка перемещения указателя файла!", 1, TRUE);
        bResult = SetEndOfFile(hFile);
        if(!bResult)
            ReportErrorAndExit(L"Ошибка изменения размера файла!", 1, TRUE);
    }

    bResult = ReadFile(hFile, &sHeader, sizeof(SHeader), &cbRead, &ovrlpdZero);
    if (!bResult)
        ReportErrorAndExit(L"Ошибка чтения заголовка", 1, TRUE);
    std::wcout << L"Число непустых записей: " << sHeader.cValidRecords << std::endl;

    WCHAR wchCommand = 0;
    DWORD nRecord = 0;
    while (TRUE)
    {
        std::wcout << L"Введите команду: [r(ead), w(rite), d(elete), q(uit)] NumRecord" << std::endl;
        std::wcin >> wchCommand;
        if (wchCommand == L'q')
        {
            break;
        }

        std::wcin >> nRecord;
        liCurrentPos.QuadPart = (nRecord - 1) * sizeof(SRecord) + sizeof(SHeader);
        ovrlpd.Offset = liCurrentPos.LowPart;
        ovrlpd.OffsetHigh = liCurrentPos.HighPart;

        if (nRecord > sHeader.cRecords)
        {
            std::wcout << L"Достигнут предел числа записей!" << std::endl;
            continue;
        }

        BOOL bRecordUpdate = FALSE;
        BOOL bHeaderUpdate = FALSE;
        ReadFile(hFile, &sRecord, sizeof(SRecord), &cbRead, &ovrlpd);
        GetSystemTime(&tSysTime);

        if (wchCommand == L'r' || wchCommand == L'd')
        {
            if (sRecord.cRef == 0)
            {
                std::wcout << L"Указанная запись пуста!" << std::endl;
            }
            else
            {
                std::wcout << L"Число обращений: " << sRecord.cRef << std::endl
                    << L"Минута создания: " << sRecord.tCreateTime.wMinute << std::endl
                    << L"Минута обращения: " << sRecord.tRefTime.wMinute << std::endl
                    << L"Минута обновления: " << sRecord.tUpdateTime.wMinute << std::endl
                    << L"Данные: " << sRecord.wszData << std::endl;
                sRecord.cRef++;
                sRecord.tRefTime = tSysTime;
                bRecordUpdate = TRUE;
            }
            if (wchCommand == L'd')
            {
                sRecord.cRef = 0;
                sHeader.cValidRecords--;
                bRecordUpdate = TRUE;
                bHeaderUpdate = TRUE;
                std::wcout << L"Запись " << nRecord << L" успешно удалена!" << std::endl;
            }
        }
        else if (wchCommand == L'w')
        {
            Prompt(L"Введите данные", MAX_PATH, (PWSTR)&sRecord.wszData, TRUE);
            if (sRecord.cRef == 0)
            {
                sHeader.cValidRecords++;
                sRecord.tCreateTime = tSysTime;
                bHeaderUpdate = TRUE;
            }
            sRecord.cRef++;
            sRecord.tUpdateTime = tSysTime;
            bRecordUpdate = TRUE;
            std::wcout << L"Запись " << nRecord << L" успешно обновлена!" << std::endl;
        }
        else
        {
            ReportError(L"Неподдерживаемая команда %с", TRUE, wchCommand);
            continue;
        }

        if (bRecordUpdate)
            WriteFile(hFile, &sRecord, sizeof(SRecord), &cbWritten, &ovrlpd);
        if (bHeaderUpdate)
            WriteFile(hFile, &sHeader, sizeof(SHeader), &cbWritten, &ovrlpdZero);
    }

    std::wcout << L"Число непустых записей: " << sHeader.cValidRecords << std::endl;
    CloseHandle(hFile);
    return 0;
}
