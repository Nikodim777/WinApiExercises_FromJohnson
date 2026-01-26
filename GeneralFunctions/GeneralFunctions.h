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