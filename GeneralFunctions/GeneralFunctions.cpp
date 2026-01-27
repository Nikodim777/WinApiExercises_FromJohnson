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
	result = _wfopen_s(&psFileOut, wszDstPath, L"w,ccs=UTF-16LE");
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

// Функция копирования файлов. Реализация на С использованием winapi.
VOID
CopyFileCWin(_In_ PCWSTR wszSrcPath, _In_ PCWSTR wszDstPath)
{
	HANDLE hSrcFile = INVALID_HANDLE_VALUE;
	HANDLE hDstFile = INVALID_HANDLE_VALUE;

	wprintf(L"Открытие файлов %s и %s\n", wszSrcPath, wszDstPath);
	hSrcFile = CreateFile(wszSrcPath, FILE_READ_ACCESS, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (hSrcFile == INVALID_HANDLE_VALUE)
	{
		wprintf(L"Ошибка при открытии файла. Код ошибки - %lu.", GetLastError());
		return;
	}
	hDstFile = CreateFile(wszDstPath, FILE_WRITE_ACCESS, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hDstFile == INVALID_HANDLE_VALUE)
	{
		wprintf(L"Ошибка при создании файла. Код ошибки - %lu.", GetLastError());
		CloseHandle(hDstFile);
		return;
	}

	BOOL bResult = FALSE;
	DWORD cbRead = 0;
	DWORD cbWritten = 0;
	BYTE buffer[MAX_PATH] = { 0 };
	wprintf(L"Копирование файла %s в %s\n", wszSrcPath, wszDstPath);
	while ((bResult = ReadFile(hSrcFile, buffer, MAX_PATH, &cbRead, NULL)) && cbRead > 0)
	{
		bResult = WriteFile(hDstFile, buffer, cbRead, &cbWritten, NULL);
		if (!bResult || cbRead != cbWritten)
		{
			break;
		}
	}
	bResult ?
		wprintf(L"Копирование файла %s в %s завершено!\n", wszSrcPath, wszDstPath) :
		wprintf(L"Неустранимая ошибка чтения/записи. Код ошибки - %lu.", GetLastError());
	CloseHandle(hSrcFile);
	CloseHandle(hDstFile);
	return;
}

// Функция копирования текстовых файлов в разных режимах. Реализация на чистом С.
// [in] isTb - если true, файл-источник открывается 
// как текстовый, а целевой файл - как бинарный, иначе - наоборот. 
VOID
CopyFileCDiff(_In_ PCWSTR wszSrcPath, _In_ PCWSTR wszDstPath, _In_ BOOL isTb)
{
	WCHAR wchBuffer[MAX_PATH] = { 0 };
	FILE* psFileIn = NULL;
	FILE* psFileOut = NULL;
	errno_t result = 0;

	wprintf(L"Открытие файлов %s и %s\n", wszSrcPath, wszDstPath);
	result = _wfopen_s(&psFileIn, wszSrcPath, isTb ? L"r,ccs=UTF-16LE" : L"rb");
	if (result != 0 || psFileIn == NULL)
	{
		PrintWideErrorC(L"Не удалось открыть исходный файл", result);
		return;
	}
	result = _wfopen_s(&psFileOut, wszDstPath, isTb ? L"wb" : L"w,ccs=UTF-16LE");
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

// Функция копирования файло, использующая вспомогательную функцию winapi.
VOID
CopyFileWinAux(_In_ PCWSTR wszSrcPath, _In_ PCWSTR wszDstPath)
{
	BOOL bResult = FALSE;
	bResult = CopyFile(wszSrcPath, wszDstPath, FALSE);
	bResult ?
		wprintf(L"Копирование успешно завершено!\n") :
		wprintf(L"При копировании произошла ошибка! Код ошибки - %lu.\n", GetLastError());
}