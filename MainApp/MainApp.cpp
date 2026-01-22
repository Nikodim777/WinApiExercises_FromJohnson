#include "fcntl.h"
#include "io.h"
#include "stdio.h"
#include "GeneralFunctions.h"
#include <iostream>

int main()
{
    _setmode(_fileno(stdout), _O_U16TEXT);

    std::wcout << L"Hello мир!" << Func() << std::endl;
}
