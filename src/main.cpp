#include <iostream>
#include <iterator>
#include <sstream>
#include <cassert>

int main(int argc, char* argv[])
{
    std::stringstream str;
    str << "Cat";

    std::istream_iterator<char> eos;
    std::istream_iterator<char> i(str);

    char a, b;
    a = *i++;
    b = *i++;
    std::cout << a << b << std::endl;

    return 0;
}

// vim: set ts=4 sw=4 :
