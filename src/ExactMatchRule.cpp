#include "ExactMatchRule"

bool ExactMatchRule::match(const std::string& candidate)
{
    return candidate == constant;
}
