#ifndef SPROUT_LEFTRECURSIONPASS_HEADER
#define SPROUT_LEFTRECURSIONPASS_HEADER

#include "Grammar.hpp"

#include <QHash>
#include <QString>

namespace sprout {

enum class RecursionState {
    Unknown,
    Pending,
    Recursive,
    Terminal
};

class LeftRecursionPass
{
    QHash<QString, RecursionState> stateMap;

    bool hasRecursions(const QString& name, GNode& node, QHash<QString, GNode>& ruleMap);

public:
    template <class Type, class Value>
    void operator()(Grammar<Type, Value>& grammar)
    {
        for (auto iter = grammar.begin(); iter != grammar.end(); ++iter) {
            GNode& node = iter.value();
            hasRecursions(iter.key(), node, grammar.parsedRules());
        }
    }
};

} // namespace sprout

#endif // SPROUT_LEFTRECURSIONPASS_HEADER

// vim: set ts=4 sw=4 :
