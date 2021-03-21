#include "fmt/format.h"
#include <stdio.h>
#include <iostream>

int main()
{
    std::wstring mine = L"what";
    std::wstring z = fmt::format(L"{2}-{1}-{0}", mine, 2, L"hell");
    std::wcout << z << std::endl;
    return 0;
}

