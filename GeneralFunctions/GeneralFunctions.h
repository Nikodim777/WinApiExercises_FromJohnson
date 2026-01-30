#pragma once

// Функция копирования текстовых Unicode-файлов. Реализация на чистом С.
VOID 
CopyTextFileC(_In_ PCWSTR wszSrcPath, _In_ PCWSTR wszDstPath);

// Функция копирования файлов. Реализация на чистом С.
VOID
CopyFileC(_In_ PCWSTR wszSrcPath, _In_ PCWSTR wszDstPath);

// Функция копирования файлов. Реализация на C++.
VOID
CopyFileCpp(_In_ const std::wstring& srcPath, _In_ const std::wstring& dstPath);

// Функция копирования файлов. Реализация на С использованием winapi.
VOID
CopyFileCWin(_In_ PCWSTR wszSrcPath, _In_ PCWSTR wszDstPath, _In_ DWORD dwFlags);

// Функция копирования файлов. Реализация на С использованием winapi без системного буфера.
// Размер сектора определяется для тома конечного файла. Тома должны быть одинаковы.
VOID
CopyFileCWinNoBuf(_In_ PCWSTR wszSrcPath, _In_ PCWSTR wszDstPath, _In_ DWORD dwFlags);

// Функция копирования текстовых файлов в разных режимах. Реализация на чистом С.
// [in] isTb - если true, файл-источник открывается 
// как текстовый, а целевой файл - как бинарный, иначе - наоборот. 
VOID
CopyFileCDiff(_In_ PCWSTR wszSrcPath, _In_ PCWSTR wszDstPath, _In_ BOOL isTb);

// Функция копирования файло, использующая вспомогательную функцию winapi.
VOID
CopyFileWinAux(_In_ PCWSTR wszSrcPath, _In_ PCWSTR wszDstPath);

// Функция выводит в консоль или файл hOut переданные в аргументах строки.
_Success_(return)
BOOL
PrintStrs(_In_ HANDLE hOut, ...);

// Функция выводит в консоль или файл hOut переданное сообщение.
inline BOOL
PrintMsg(_In_ HANDLE hOut, _In_ PCWSTR wszMsg)
{
	return PrintStrs(hOut, wszMsg, NULL);
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
	_In_ BOOL bIsNeedEcho);

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
	_In_ ...);

// Функция выводит в консоль форматированное сообщение.
#define ReportMsg(wszFormatMsg, ...) Report((wszFormatMsg), 0, FALSE, __VA_ARGS__)
// Функция выводит в консоль форматированное сообщение об ошибке.
#define ReportError(wszFormatMsg, isNeedSysMsg, ...) Report((wszFormatMsg), 0, (isNeedSysMsg), __VA_ARGS__)
// Функция выводит в консоль форматированное сообщение об ошибке и завершает процесс с ошибкой dwExitCode.
#define ReportErrorAndExit(wszFormatMsg, dwExitCode, isNeedSysMsg, ...) Report((wszFormatMsg), (dwExitCode), (isNeedSysMsg), __VA_ARGS__)

/* Функция выводит указанный файл в указанный выходной поток,
	[in] hOut - ручка к выходному потоку;
	[in] hFile - ручка к файлу;
	Возвращает успешность выполнения. */
_Success_(return)
BOOL
CatFile(_In_ HANDLE hOut,
	_In_ HANDLE hFile);

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
	_In_ BOOL bSilence);

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
	_In_ BOOL bRewrite);

VOID
PrintCurrentDir();

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
	_Out_ ...);

/* Функция сортирует переданный массив строк с учётом локали.
	[in] cStrs - число строк в массиве;
	[in] strs - массив строк;
	[in] bEnUS - флаг принудительного использования локали en-US;
	Ничего не возвращает.
*/
VOID
StringsSort(_In_ INT cStrs,
	_In_reads_(cStrs) PCWSTR strs[],
	_In_ BOOL bEnUS);