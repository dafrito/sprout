#ifndef SPROUT_RULE_OPERATION_HEADER
#define SPROUT_RULE_OPERATION_HEADER

#include "rule/RuleTraits.hpp"
#include "rule/Join.hpp"

#include <grammar/Node.hpp>

#include "../Cursor.hpp"
#include "../Result.hpp"

namespace sprout {
namespace rule {

template <
    class Expression,
    class Operator,
    class LessThan,
    class Type = typename Expression::token_type::type_type,
    class Value = typename Expression::token_type::value_type,
    class Input = typename Expression::input_type
>
class Operation : public RuleTraits<Input, grammar::Node<Type, Value>>
{
    typedef grammar::Node<Type, Value> ENode;

    const LessThan _lessThan;

    const rule::Join<
        Expression,
        Operator,
        Input,
        typename Operation::token_type
    > _joiner;

public:
    Operation(
            const Expression& expression,
            const Operator& operatorRule,
            const LessThan& lessThan
        ) :
        _lessThan(lessThan),
        _joiner(expression, operatorRule)
    {

    }

    bool operator()(Cursor<Input>& iter, Result<ENode>& dest) const
    {
        Result<ENode> src;
        if (!_joiner(iter, src)) {
            return false;
        }

        ENode left = *src++;
        if (!src) {
            dest << left;
            return true;
        }

        ENode tmp = *src++;
        tmp << left;
        left = tmp;
        left << *src++;

        ENode* pos = &left;
        while (src) {
            ENode rightOp(*src++);

            if (_lessThan(left, rightOp)) {
                // e.g. 2 + 3 / 4
                // (+ 2 3) -> (+ 2 (/ 3 4))
                rightOp << pos->at(1) << *src++;
                pos->erase(1);
                *pos << rightOp;
                pos = &pos->at(1);
            } else {
                // e.g. 2 / 3 + 4
                // (/ 2 3) -> (+ (/ 2 3) 4)
                rightOp << left << *src++;
                left = rightOp;
                pos = &left;
            }
        }
        dest << left;
        return true;
    }
};

template <
    class Expression,
    class Operator,
    class LessThan
>
rule::Operation<Expression, Operator, LessThan>
operation(
    const Expression& expression,
    const Operator& operatorRule,
    const LessThan& lessThan
)
{
    return rule::Operation<Expression, Operator, LessThan>(
        expression,
        operatorRule,
        lessThan
    );
}

} // namespace rule
} // namespace sprout

#endif // SPROUT_RULE_OPERATION_HEADER

// vim: set ts=4 sw=4 :
