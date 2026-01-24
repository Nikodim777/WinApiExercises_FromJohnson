#include "stdafx.h"

VOID
PrintWideErrorC(PCWSTR wszMessage, errno_t nError)
{
	WCHAR wszError[MAX_PATH] = { 0 };
	_wcserror_s(wszError, nError);
	fwprintf(stderr, L"%s: %s!\n", wszMessage, wszError);
}

VOID
PrintWideErrorCpp(PCWSTR wszMessage, errno_t nError)
{
	WCHAR wszError[MAX_PATH] = { 0 };
	_wcserror_s(wszError, nError);
	std::wcout << wszMessage << L": " << wszError << std::endl;
}


// Функция копирования текстовых Unicode-файлов. Реализация на чистом С.
VOID 
CopyTextFileC(_In_ PCWSTR wszSrcPath, _In_ PCWSTR wszDstPath)
{
	WCHAR wchBuffer[MAX_PATH] = { 0 };
	FILE* psFileIn = NULL;
	FILE* psFileOut = NULL;
	errno_t result = 0;

	wprintf(L"Открытие файлов %s и %s\n", wszSrcPath, wszDstPath);
	result = _wfopen_s(&psFileIn, wszSrcPath, L"r,ccs=UTF-16LE");
	if (result != 0 || psFileIn == NULL)
	{
		PrintWideErrorC(L"Не удалось открыть исходный файл", result);
		return;
	}
	result = _wfopen_s(&psFileOut, wszDstPath, L"w,ccs=UNICODE");
	if (result != 0 || psFileOut == NULL)
	{
		PrintWideErrorC(L"Не удалось создать файл назначения", result);
		fclose(psFileIn);
		return;
	}

	SIZE_T cRead = 0;
	SIZE_T cWritten = 0;
	wprintf(L"Копирование файла %s в %s\n", wszSrcPath, wszDstPath);
	while ((cRead = fread(wchBuffer, sizeof(WCHAR), MAX_PATH, psFileIn)) > 0)
	{
		cWritten = fwrite(wchBuffer, sizeof(WCHAR), cRead, psFileOut);
		if (cWritten != cRead)
		{
			PrintWideErrorC(L"Неустранимая ошибка при записи в файл", result);
			break;
		}
	}
	if (feof(psFileIn) == 0)
	{
		PrintWideErrorC(L"Ошибка при чтении файла", result);
	}
	wprintf(L"Копирование файла %s в %s завершено!\n", wszSrcPath, wszDstPath);

	fclose(psFileIn);
	fclose(psFileOut);
}

// Функция копирования файлов. Реализация на чистом С.
VOID
CopyFileC(_In_ PCWSTR wszSrcPath, _In_ PCWSTR wszDstPath)
{
	BYTE buffer[MAX_PATH] = { 0 };
	FILE* psFileIn = NULL;
	FILE* psFileOut = NULL;
	errno_t result = 0;

	wprintf(L"Открытие файлов %s и %s\n", wszSrcPath, wszDstPath);
	result = _wfopen_s(&psFileIn, wszSrcPath, L"rb");
	if (result != 0 || psFileIn == NULL)
	{
		PrintWideErrorC(L"Не удалось открыть исходный файл", result);
		return;
	}
	result = _wfopen_s(&psFileOut, wszDstPath, L"wb");
	if (result != 0 || psFileOut == NULL)
	{
		PrintWideErrorC(L"Не удалось создать файл назначения", result);
		fclose(psFileIn);
		return;
	}

	SIZE_T cRead = 0;
	SIZE_T cWritten = 0;
	wprintf(L"Копирование файла %s в %s\n", wszSrcPath, wszDstPath);
	while ((cRead = fread(buffer, 1, MAX_PATH, psFileIn)) > 0)
	{
		cWritten = fwrite(buffer, 1, cRead, psFileOut);
		if (cWritten != cRead)
		{
			PrintWideErrorC(L"Неустранимая ошибка при записи в файл", result);
			break;
		}
	}
	if (feof(psFileIn) == 0)
	{
		PrintWideErrorC(L"Ошибка при чтении файла", result);
	}
	wprintf(L"Копирование файла %s в %s завершено!\n", wszSrcPath, wszDstPath);

	fclose(psFileIn);
	fclose(psFileOut);
}

// Функция копирования файлов. Реализация на C++.
VOID
CopyFileCpp(_In_ const std::wstring& srcPath, _In_ const std::wstring& dstPath)
{
	std::wcout << L"Открытие файлов " << srcPath << L" и " << dstPath << std::endl;
	std::wifstream srcStream{ srcPath, std::ios::binary };
	if (!srcStream)
	{
		PrintWideErrorCpp(L"Не удалось открыть исходный файл", errno);
		return;
	}
	std::wofstream dstStream{ dstPath, std::ios::binary | std::ios::trunc };
	if (!dstStream)
	{
		PrintWideErrorCpp(L"Не удалось создать файл назначения", errno);
		return;
	} 

	std::wcout << L"Копирование файла " << srcPath << L" в " << dstPath << std::endl;
	dstStream << srcStream.rdbuf();
	std::wcout << L"Копирование файла " << srcPath << L" в " << dstPath << L" завершено!" << std::endl;
}