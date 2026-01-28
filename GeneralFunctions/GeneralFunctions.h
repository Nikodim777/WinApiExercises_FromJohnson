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
CopyFileCWin(_In_ PCWSTR wszSrcPath, _In_ PCWSTR wszDstPath);

// Функция копирования текстовых файлов в разных режимах. Реализация на чистом С.
// [in] isTb - если true, файл-источник открывается 
// как текстовый, а целевой файл - как бинарный, иначе - наоборот. 
VOID
CopyFileCDiff(_In_ PCWSTR wszSrcPath, _In_ PCWSTR wszDstPath, _In_ BOOL isTb);

// Функция копирования файло, использующая вспомогательную функцию winapi.
VOID
CopyFileWinAux(_In_ PCWSTR wszSrcPath, _In_ PCWSTR wszDstPath);

// Функция выводит в консоль или файл hOut переданные в аргументах строки.
BOOL
PrintStrs(_In_ HANDLE hOut, ...);

// Функция выводит в консоль или файл hOut переданное сообщение.
inline BOOL
PrintMsg(_In_ HANDLE hOut, PCWSTR wszMsg)
{
	return PrintStrs(hOut, wszMsg, NULL);
}

/* Функция выводит запрос пользователю и считывает ответ.
	[in] Строка с запросом, :\n - добавляются автоматически;
	[in] Размер буфера ответа в символах;
	[out] Буфер ответа;
	[in] Флаг необходимости эхо для ввода;
	Возвращает успешность выполнения. */
_Success_(return)
BOOL
Prompt(_In_ PCWSTR wszPrompt,
	_In_ DWORD cwResponse,
	_Out_writes_(cwResponse) PWSTR wszResponse,
	BOOL bIsNeedEcho);