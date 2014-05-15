#include <grammar/pass/Flatten.hpp>

#include "node.hpp"
#include "init.hpp"

using namespace sprout;
typedef grammar::Node<TType, QString> TNode;

BOOST_AUTO_TEST_CASE(testFlatten)
{
    using namespace grammar;

    auto tree = TNode(TType::Add, {
        TNode(TType::Name, "a")
    });

    pass::Flatten<TType, QString> flattenPass({
        TType::Add,
        TType::Subtract,
        TType::Multiply,
        TType::Divide,
    });
    flattenPass(tree);

    BOOST_CHECK_EQUAL(
        TNode(TType::Name, "a"),
        tree
    );
}

BOOST_AUTO_TEST_CASE(testFlattenWithNonFlattenable)
{
    using namespace grammar;

    auto tree = TNode(TType::Access, "foo", {
        TNode(TType::Name, "a")
    });
    TNode orig = tree;

    pass::Flatten<TType, QString> flattenPass({
        TType::Add,
        TType::Subtract,
        TType::Multiply,
        TType::Divide,
    });
    flattenPass(tree);

    BOOST_CHECK_EQUAL(
        orig,
        tree
    );
}
