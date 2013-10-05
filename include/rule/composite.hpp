#ifndef SPROUT_RULE_COMPOSITE_HEADER
#define SPROUT_RULE_COMPOSITE_HEADER

namespace sprout {
namespace rule {

template <class Target>
void populate(Target& target)
{
    // Exit condition
}

template <class Target, class T, typename... Values>
void populate(Target& target, const T& value, Values... rest)
{
    target << value;
    populate(target, rest...);
}

} // namespace rule
} // namespace sprout

#endif // SPROUT_RULE_COMPOSITE_HEADER

// vim: set ft=cpp ts=4 sw=4 :
