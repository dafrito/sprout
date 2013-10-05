#ifndef SPROUT_GRAMMAR_PASS_LEFTRECURSION_HEADER
#define SPROUT_GRAMMAR_PASS_LEFTRECURSION_HEADER

#include "../Grammar.hpp"

#include <QHash>
#include <QString>

namespace sprout {
namespace grammar {
namespace pass {

enum class RecursionState {
    Unknown,
    Pending,
    Recursive,
    Terminal
};

class LeftRecursion
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

} // namespace pass
} // namespace grammar
} // namespace sprout

#endif // SPROUT_GRAMMAR_PASS_LEFTRECURSION_HEADER

// vim: set ts=4 sw=4 :
