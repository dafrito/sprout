#include <rule/Recursive.hpp>

#include "node.hpp"

#include <rule/Multiple.hpp>
#include <rule/Alternative.hpp>
#include <rule/Discard.hpp>
#include <rule/Literal.hpp>
#include <rule/Reduce.hpp>
#include <rule/rules.hpp>

#include <grammar/Node.hpp>

#include "init.hpp"

#include <functional>
#include <unordered_map>

using namespace sprout;
typedef grammar::Node<TType, QString> TNode;

BOOST_AUTO_TEST_CASE(testRecursive)
{

    auto name = rule::convert<TNode>(
        rule::variable(),
        [](const QString& name) {
            return TNode(TType::Name, name);
        }
    );

    // foo.bar.baz
    // Access:baz [
    //     Access:bar [
    //         Name:foo
    //     ]
    // ]

    auto rule = rule::recursive(
        name,
        rule::tupleSequence<QChar, TNode>(
            discard(rule::OrderedLiteral<QChar, TNode>(".")),
            rule::convert<TNode>(
                name,
                [](TNode node) {
                    node.setType(TType::Access);
                    return node;
                }
            )
        ),
        [](Result<TNode>& result) {
            auto inner = *result++;
            auto access = *result++;
            access << inner;
            result.clear();
            result << access;
        }
    );

    auto iter = makeCursor<QChar>("foo.bar.baz");
    Result<TNode> result;

    BOOST_CHECK(rule(iter, result));
    BOOST_REQUIRE(result);

    auto expected = TNode(TType::Access, "baz", {
        TNode(TType::Access, "bar", {
            TNode(TType::Name, "foo")
        })
    });

    BOOST_CHECK_EQUAL(expected, *result);
    BOOST_CHECK(!iter);
}
