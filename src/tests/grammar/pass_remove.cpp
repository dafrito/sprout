#include <grammar/pass/Remove.hpp>

#include "node.hpp"
#include "init.hpp"

using namespace sprout;
typedef grammar::Node<TType, QString> TNode;

BOOST_AUTO_TEST_CASE(testRemove)
{
    using namespace grammar;

    auto tree = TNode(TType::Add, {
        TNode(TType::Noop),
        TNode(TType::Name, "a"),
        TNode(TType::Noop),
        TNode(TType::Noop),
        TNode(TType::Name, "b"),
        TNode(TType::Noop),
    });

    pass::Remove<TType, QString> pass({
        TType::Noop,
    });
    pass(tree);

    BOOST_CHECK_EQUAL(
        TNode(TType::Add, {
            TNode(TType::Name, "a"),
            TNode(TType::Name, "b")
        }),
        tree
    );
}
