#include <iostream>
#include <sstream>
#include <cassert>

int main(int argc, char* argv[])
{
    std::stringstream str;

    str << "Notime";

    char a, b;
    str >> a >> b;
    std::cout << a << b << std::endl;
    str.putback(b);
    str.putback(a);

    assert(str.good());

    char c;
    str >> a >> b >> c;
    std::cout << a << b << c << std::endl;

    return 0;
}

// vim: set ts=4 sw=4 :
