#include <rule/Operation.hpp>

#include <rule/rules.hpp>
#include <rule/Reduce.hpp>
#include <rule/Literal.hpp>

#include "node.hpp"
#include "init.hpp"

using namespace sprout;
typedef grammar::Node<TType, QString> TNode;

BOOST_AUTO_TEST_CASE(testOperation)
{
    auto name = rule::convert<TNode>(
        rule::variable(),
        [](const QString& name) {
            return TNode(TType::Name, name);
        }
    );

    auto rule = rule::operation(
        name,
        rule::tupleAlternative<QChar, TNode>(
            rule::OrderedLiteral<QChar, TNode>("+", TNode(TType::Add)),
            rule::OrderedLiteral<QChar, TNode>("-", TNode(TType::Subtract)),
            rule::OrderedLiteral<QChar, TNode>("*", TNode(TType::Multiply)),
            rule::OrderedLiteral<QChar, TNode>("/", TNode(TType::Divide)),
            rule::OrderedLiteral<QChar, TNode>("^", TNode(TType::Exponent))
        ),
        [](const TNode& left, const TNode& right) {
            return operatorPrecedence(left.type()) < operatorPrecedence(right.type());
        }
    );

    {
        auto iter = makeCursor<QChar>("a");
        Result<TNode> result;

        BOOST_CHECK(rule(iter, result));
        BOOST_REQUIRE(result);

        auto expected = TNode(TType::Name, "a");

        BOOST_CHECK_EQUAL(expected, *result);
        BOOST_CHECK(!iter);
    }

    {
        auto iter = makeCursor<QChar>("a+");
        Result<TNode> result;

        BOOST_CHECK(rule(iter, result));
        BOOST_REQUIRE(result);

        auto expected = TNode(TType::Name, "a");

        BOOST_CHECK_EQUAL(expected, *result);
        BOOST_CHECK(iter);
        BOOST_CHECK_EQUAL('+', *iter);
    }

    {
        auto iter = makeCursor<QChar>("a+b");
        Result<TNode> result;

        BOOST_CHECK(rule(iter, result));
        BOOST_REQUIRE(result);

        auto expected = TNode(TType::Add, {
            TNode(TType::Name, "a"),
            TNode(TType::Name, "b")
        });

        BOOST_CHECK_EQUAL(expected, *result);
        BOOST_CHECK(!iter);
    }

    {
        auto iter = makeCursor<QChar>("a+b*c");
        Result<TNode> result;

        BOOST_CHECK(rule(iter, result));
        BOOST_REQUIRE(result);

        auto expected = TNode(TType::Add, {
            TNode(TType::Name, "a"),
            TNode(TType::Multiply, {
                TNode(TType::Name, "b"),
                TNode(TType::Name, "c")
            })
        });

        BOOST_CHECK_EQUAL(expected, *result);
        BOOST_CHECK(!iter);
    }

    {
        auto iter = makeCursor<QChar>("a*b+c");
        Result<TNode> result;

        BOOST_CHECK(rule(iter, result));
        BOOST_REQUIRE(result);

        auto expected = TNode(TType::Add, {
            TNode(TType::Multiply, {
                TNode(TType::Name, "a"),
                TNode(TType::Name, "b")
            }),
            TNode(TType::Name, "c")
        });

        BOOST_CHECK_EQUAL(expected, *result);
        BOOST_CHECK(!iter);
    }

    {
        auto iter = makeCursor<QChar>("a+b*c^d");
        Result<TNode> result;

        BOOST_CHECK(rule(iter, result));
        BOOST_REQUIRE(result);

        auto expected = TNode(TType::Add, {
            TNode(TType::Name, "a"),
            TNode(TType::Multiply, {
                TNode(TType::Name, "b"),
                TNode(TType::Exponent, {
                    TNode(TType::Name, "c"),
                    TNode(TType::Name, "d"),
                })
            }),
        });

        BOOST_CHECK_EQUAL(expected, *result);
        BOOST_CHECK(!iter);
    }

}
