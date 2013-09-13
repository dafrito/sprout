#ifndef SPROUT_COMPOSITERULE_HEADER
#define SPROUT_COMPOSITERULE_HEADER

namespace sprout {

template <class Target>
void populate(Target& target)
{
    // Exit condition
}

template <class Target, class T, typename... Values>
void populate(Target& target, const T& value, Values... rest)
{
    target.insert(value);
    populate(target, rest...);
}

} // namespace sprout

#endif // SPROUT_COMPOSITERULE_HEADER

// vim: set ft=cpp ts=4 sw=4 :
