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
CopyFileCWin(_In_ PCWSTR wszSrcPath, _In_ PCWSTR wszDstPath, _In_ DWORD dwFlags)
{
	HANDLE hSrcFile = INVALID_HANDLE_VALUE;
	HANDLE hDstFile = INVALID_HANDLE_VALUE;

	wprintf(L"Открытие файлов %s и %s\n", wszSrcPath, wszDstPath);
	hSrcFile = CreateFile(wszSrcPath, FILE_READ_ACCESS, 0, NULL, OPEN_EXISTING, dwFlags, NULL);
	if (hSrcFile == INVALID_HANDLE_VALUE)
	{
		wprintf(L"Ошибка при открытии файла. Код ошибки - %lu.", GetLastError());
		return;
	}
	hDstFile = CreateFile(wszDstPath, FILE_WRITE_ACCESS, 0, NULL, CREATE_ALWAYS, dwFlags | FILE_ATTRIBUTE_NORMAL, NULL);
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

// Функция копирования файлов. Реализация на С использованием winapi без системного буфера.
// Размер сектора определяется для тома конечного файла. Тома должны быть одинаковы.
VOID
CopyFileCWinNoBuf(_In_ PCWSTR wszSrcPath, _In_ PCWSTR wszDstPath, _In_ DWORD dwFlags)
{
	HANDLE hSrcFile = INVALID_HANDLE_VALUE;
	HANDLE hDstFile = INVALID_HANDLE_VALUE;

	wprintf(L"Открытие файлов %s и %s\n", wszSrcPath, wszDstPath);
	hSrcFile = CreateFile(wszSrcPath, FILE_READ_ACCESS, 0, NULL, OPEN_EXISTING, dwFlags | FILE_FLAG_NO_BUFFERING, NULL);
	if (hSrcFile == INVALID_HANDLE_VALUE)
	{
		wprintf(L"Ошибка при открытии файла. Код ошибки - %lu.", GetLastError());
		return;
	}
	hDstFile = CreateFile(wszDstPath, FILE_WRITE_ACCESS, 0, NULL, CREATE_ALWAYS, dwFlags | FILE_FLAG_NO_BUFFERING | FILE_ATTRIBUTE_NORMAL, NULL);
	if (hDstFile == INVALID_HANDLE_VALUE)
	{
		wprintf(L"Ошибка при создании файла. Код ошибки - %lu.", GetLastError());
		CloseHandle(hDstFile);
		return;
	}

	WCHAR wszDrive[16] = { 0 };
	GetVolumePathName(wszDstPath, wszDrive, 16);
	DISK_SPACE_INFORMATION diskInfo = { 0 };
	HRESULT hRes = GetDiskSpaceInformation(wszDrive, &diskInfo);
	if (hRes != S_OK)
		ReportErrorAndExit(L"Не удалось получить размер сектор диска", 1, TRUE);

	BOOL bResult = FALSE;
	DWORD cbRead = 0;
	DWORD cbWritten = 0;
	DWORD cbBuffer = diskInfo.BytesPerSector * 2;
	PBYTE pBuffer = (PBYTE)VirtualAlloc(NULL, cbBuffer, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE );
	wprintf(L"Копирование файла %s в %s\n", wszSrcPath, wszDstPath);
	while ((bResult = ReadFile(hSrcFile, pBuffer, cbBuffer, &cbRead, NULL)) && cbRead > 0)
	{
		bResult = WriteFile(hDstFile, pBuffer, cbBuffer, &cbWritten, NULL);
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

// Функция выводит в консоль или файл hOut переданные в аргументах строки.
_Success_(return)
BOOL
PrintStrs(_In_ HANDLE hOut, ...)
{
	SIZE_T cwArg = 0;
	SIZE_T cWritten = 0;
	va_list args = NULL;
	PCWSTR wszArg = NULL;
	BOOL bResult = TRUE;

	va_start(args, hOut);
	while (bResult && (wszArg = va_arg(args, PCWSTR)))
	{
		cwArg = wcslen(wszArg);
		bResult = WriteConsole(hOut, wszArg, cwArg, &cWritten, NULL) 
			|| WriteFile(hOut, wszArg, cwArg * sizeof(WCHAR), &cWritten, NULL);
	}
	va_end(args);
	return bResult;
}

/* Функция выводит запрос пользователю и считывает ответ.
	[in] wszPrompt Строка с запросом, :\n - добавляются автоматически;
	[in] cwResponse Размер буфера ответа в символах;
	[out] wszResponse Буфер ответа;
	[in] bIsNeedEcho Флаг необходимости эхо для ввода;
	Возвращает успешность выполнения. */
_Success_(return)
BOOL
Prompt(_In_ PCWSTR wszPrompt, 
	_In_ DWORD cwResponse, 
	_Out_writes_(cwResponse) PWSTR wszResponse, 
	_In_ BOOL bIsNeedEcho)
{
	HANDLE hConsoleIn = INVALID_HANDLE_VALUE;
	HANDLE hConsoleOut = INVALID_HANDLE_VALUE;
	DWORD cwRead = 0;
	DWORD dwConsoleInMode = 0;
	BOOL bResult = FALSE;

	hConsoleIn = CreateFile(L"CONIN$", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, 0, NULL);
	if (hConsoleIn == INVALID_HANDLE_VALUE)
	{
		return FALSE;
	}
	hConsoleOut = CreateFile(L"CONOUT$", GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_ALWAYS, 0, NULL);
	if (hConsoleOut == INVALID_HANDLE_VALUE)
	{
		CloseHandle(hConsoleIn);
		return FALSE;
	}

	bResult = GetConsoleMode(hConsoleIn, &dwConsoleInMode) 
		&& SetConsoleMode(hConsoleIn, (dwConsoleInMode & ~ENABLE_ECHO_INPUT) | (bIsNeedEcho ? ENABLE_ECHO_INPUT : 0))
		&& PrintStrs(hConsoleOut, wszPrompt, L":\n", NULL)
		&& ReadConsole(hConsoleIn, wszResponse, cwResponse, &cwRead, NULL);
	wszResponse[cwRead - 2] = '\0';

	CloseHandle(hConsoleOut);
	CloseHandle(hConsoleIn);
	return bResult;
}

/* Функция выводит сообщение.
	[in] wszMsg - пользовательское сообщение об ошибке в виде форматной строки;
	[in] dwExitCode - код завершение процесса, если не 0 - процесс завершается;
	[in] isNeedSysMsg - флаг необходимости получения системного сообщения по ошибке;
	[in] ... - аргументы форматной строки;
	Ничего не возвращает. */
VOID
Report(_In_ PCWSTR wszFormatMsg,
	_In_ DWORD dwExitCode,
	_In_ BOOL isNeedSysMsg,
	_In_ ...)
{
	DWORD dwError = GetLastError();
	DWORD cwSysMsg = 0;
	PWSTR wszSysMsg = NULL;
	HANDLE hStdOut = GetStdHandle(dwError == ERROR_SUCCESS ? STD_OUTPUT_HANDLE : STD_ERROR_HANDLE);
	WCHAR wszMsg[MAX_PATH] = { 0 };

	va_list args;
	va_start(args, isNeedSysMsg);
	vswprintf_s(wszMsg, MAX_PATH, wszFormatMsg, args);
	va_end(args);

	PrintMsg(hStdOut, wszMsg);
	if (isNeedSysMsg)
	{
		cwSysMsg = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, dwError, 0, (LPWSTR)&wszSysMsg, 0, NULL);
		wszSysMsg[cwSysMsg - 3] = L'\0'; // Удаляем перенос строки.
		PrintStrs(hStdOut, L": ", wszSysMsg, NULL);
		LocalFree(wszSysMsg);
	}
	PrintMsg(hStdOut, (dwError == ERROR_SUCCESS) ? L"\n" : L"!\n");

	if (dwExitCode > 0)
	{
		ExitProcess(dwExitCode);
	}
	else
	{
		return;
	}
}

/* Функция выводит указанный файл в указанный выходной поток,
	[in] hOut - ручка к выходному потоку;
	[in] hFile - ручка к файлу;
	Возвращает успешность выполнения. */
_Success_(return)
BOOL
CatFile(_In_ HANDLE hOut,
	_In_ HANDLE hFile)
{
	BOOL bResult = TRUE;
	DWORD cRead = 0;
	DWORD cWritten = 0;
	BYTE buffer[MAX_PATH];
	
	while (TRUE)
	{
		BOOL bConsoleIn = bResult = ReadConsole(hFile, buffer, MAX_PATH / 2, &cRead, NULL);
		if (!bConsoleIn)
			bResult = ReadFile(hFile, buffer, MAX_PATH, &cRead, NULL);
		if (!bResult)
		{
			ReportError(L"Ошибка чтения из файла", TRUE);
			return FALSE;
		}

		if (cRead == 0)
			return TRUE;

		bResult = WriteConsole(hOut, buffer, bConsoleIn ? cRead : cRead / 2, &cWritten, NULL) 
			|| WriteFile(hOut, buffer, bConsoleIn ? cRead * sizeof(WCHAR) : cRead, &cWritten, NULL);
		if (!bResult)
		{
			ReportError(L"Ошибка записи в файл", TRUE);
			return FALSE;
		}
	}
}

/* Функция выводит файлы в выходной поток, 
	если файлы не переданы, то выводит входной поток.
	[in] cFiles - количество файлов в переданном массиве строк;
	[in] a_FilePaths - указатель на массив строк;
	[in] bSilence - флаг режима тишины (сообщения об ошибках не выводятся);
	Возвращает успешность выполнения. */
_Success_(return)
BOOL
CatFiles(_In_ SIZE_T cFiles, 
	_In_reads_(cFiles) PCWSTR a_FilePaths[], 
	_In_ BOOL bSilence)
{
	BOOL bResult = TRUE;
	HANDLE hStdIn = GetStdHandle(STD_INPUT_HANDLE);
	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	HANDLE hFile = INVALID_HANDLE_VALUE;

	if (hStdIn == INVALID_HANDLE_VALUE || hStdIn == NULL ||
		hStdOut == INVALID_HANDLE_VALUE || hStdOut == NULL)
	{
		ReportError(L"Не удалось получить один из дескрипторов", TRUE);
		return FALSE;
	}

	if (cFiles == 0)
	{
		bResult &= CatFile(hStdOut, hStdIn);
	}

	for (SIZE_T i = 0; i < cFiles; i++)
	{
		hFile = CreateFile(a_FilePaths[i], FILE_READ_ACCESS, 0, NULL, OPEN_EXISTING, 0, NULL);
		if (hFile == INVALID_HANDLE_VALUE)
		{
			if(!bSilence)
				ReportError(L"Не удалось открыть файл", TRUE);
			continue;
		}

		bResult &= CatFile(hStdOut, hFile);
		CloseHandle(hFile);
	}

	return bResult;
}

/* Функция преобразует исходный ASCII файл в Unicode, и сохраняет в файл назначения.
	Работает только с базовыми ASCII символами.
	[in] wszSrcFile - путь к исходному файлу;
	[in] wszDstFile - путь к файлу назначения;
	[in] bRewrite - флаг форсированой перезаписи,если файл назначения уже существует;
	Ничего не возвращает.
*/
VOID
AnsiToUnicode(_In_ PCWSTR wszSrcFile, 
	_In_ PCWSTR wszDstFile, 
	_In_ BOOL bRewrite)
{
	HANDLE hSrcFile = INVALID_HANDLE_VALUE;
	HANDLE hDstFile = INVALID_HANDLE_VALUE;
	DWORD cbRead = 0;
	DWORD cbWritten = 0;
	DWORD dwDstAttr = GetFileAttributes(wszDstFile);
	CHAR chBuffer[MAX_PATH] = { 0 };
	WCHAR wchBuffer[MAX_PATH] = { 0 };

	if (dwDstAttr != INVALID_FILE_ATTRIBUTES && !bRewrite)
	{
		WCHAR wszResponse[MAX_PATH] = { 0 };
		Prompt(L"Файл уже существует! Если хотите перезаписать введите yes", MAX_PATH, wszResponse, TRUE);
		if (wcscmp(wszResponse, L"yes"))
		{
			return;
		}
	}

	hSrcFile = CreateFile(wszSrcFile, FILE_READ_ACCESS, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (hSrcFile == INVALID_HANDLE_VALUE)
	{
		ReportError(L"Не удалось открыть исходный файл", TRUE);
		return;
	}
	hDstFile = CreateFile(wszDstFile, FILE_WRITE_ACCESS, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hDstFile == INVALID_HANDLE_VALUE)
	{
		ReportError(L"Не удалось создать файл", TRUE);
		CloseHandle(hSrcFile);
		return;
	}

	BOOL bResult = TRUE;
	WCHAR wchBOM = { L'\uFEFF' };
	bResult = WriteFile(hDstFile, &wchBOM, sizeof(WCHAR), &cbWritten, NULL);
	while (bResult && (bResult = ReadFile(hSrcFile, chBuffer, MAX_PATH, &cbRead, NULL)) && cbRead > 0)
	{
		for (DWORD i = 0; i < cbRead; i++)
		{
			wchBuffer[i] = (WCHAR)chBuffer[i];
		}
		bResult = WriteFile(hDstFile, wchBuffer, cbRead * sizeof(WCHAR), &cbWritten, NULL);
	}

	bResult ?
		ReportMsg(L"Преобразование успешно завершено\n") :
		ReportError(L"Неустранимая ошибка чтения/записи", TRUE);
	CloseHandle(hDstFile);
	CloseHandle(hSrcFile);
}

VOID
PrintCurrentDir()
{
	DWORD dwResult = 0;
	WCHAR wchDir[MAX_PATH];
	
	dwResult = GetCurrentDirectory(MAX_PATH, wchDir);
	(dwResult == 0 || dwResult >= MAX_PATH) ?
		ReportError(L"Произошла ошибка", TRUE) :
		ReportMsg(wchDir);
}

/* Функция в аргументах cmd ищет флаги указанные в wszOptions,
	и заполняет соответствующие флаги, переданные как PBOOL.
	[in] argc - число аргументов cmd;
	[in] argv - аргументы cmd;
	[in] wszOptions - список флагов-символов с нулём на конце;
	[in] bIsStrict - флаг выведения сообщения об ошибке, 
	если в аргументах есть флаги не указанные в wszOptions;
	[in] ... - указатели на BOOL, их число должно соответствовать числу флагов в wszOptions;
	Возвращает номер первого аргумента - не флага. */
DWORD
GetOptions(_In_ DWORD argc,
	_In_ PCWSTR argv[],
	_In_ PCWSTR wszOptions,
	_In_ BOOL bIsStrict,
	_Out_ ...)
{
	va_list args;
	WCHAR wsFlag[2] = { L'-', L'-' };

	DWORD dwResult = 1;
	for (; dwResult < argc && argv[dwResult][0] == L'-'; dwResult++)
	{
		if (bIsStrict && !wcschr(wszOptions, argv[dwResult][1]))
			ReportErrorAndExit(L"В аргументах передан неразрешённый флаг -%c", 1, FALSE, argv[dwResult][1]);
	}

	va_start(args, bIsStrict);
	for (DWORD dwFlag = 0; wszOptions[dwFlag] != L'\0'; dwFlag++)
	{
		wsFlag[1] = wszOptions[dwFlag];
		PBOOL pbFlag = va_arg(args, PBOOL);
		if (pbFlag == NULL)
			continue;
		*pbFlag = FALSE;

		for (DWORD dwArg = 1; !(*pbFlag) && dwArg < argc; dwArg++)
		{
			if (memcmp(argv[dwArg], wsFlag, sizeof(wsFlag)))
				continue;
			
			*pbFlag = TRUE;
		}
	}
	va_end(args);

	return dwResult;
}

/* Функция сортирует переданный массив строк с учётом локали.
	[in] cStrs - число строк в массиве;
	[in] strs - массив строк;
	[in] bEnUS - флаг принудительного использования локали en-US;
	Ничего не возвращает.
*/
VOID
StringsSort(_In_ INT cStrs, 
	_In_reads_(cStrs) PCWSTR strs[], 
	_In_ BOOL bEnUS)
{
	_locale_t locale = _create_locale(LC_ALL, "en_US");
	INT iStr = 1;
	for (; iStr < cStrs; iStr++)
	{
		INT iPlace = iStr;
		for (; iPlace > 0 && 
			(bEnUS ? 
				_wcsicmp_l(strs[iPlace - 1], strs[iPlace], locale) : 
				wcscmp(strs[iPlace - 1], strs[iPlace])) > 0; 
			iPlace--)
		{
			PCWSTR wszTmp = strs[iPlace - 1];
			strs[iPlace - 1] = strs[iPlace];
			strs[iPlace] = wszTmp;
		}
	}
}