#include "init.hpp"

std::ostream& operator<<(std::ostream& stream, const QString& str)
{
    return stream << str.toUtf8().constData();
}

