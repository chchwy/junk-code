// json-minify.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <streambuf>
#include "json.hpp"

int main()
{
    std::cout << "Hello World!\n";

    std::ifstream t("file.txt");
    std::string str((std::istreambuf_iterator<char>(t)),
        std::istreambuf_iterator<char>());
}
