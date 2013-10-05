#include <grammar/pass/Flatten.hpp>

namespace sprout {
namespace grammar {
namespace pass {

void Flatten::flatten(GNode& node)
{
    for (GNode& child : node.children()) {
        flatten(child);
    }
    switch (node.type()) {
        case TokenType::Sequence:
        case TokenType::Alternative:
            if (node.children().size() == 1) {
                node = node[0];
            }
            break;
        default:
            break;
    }
}

} // namespace pass
} // namespace grammar
} // namespace sprout

// vim: set ts=4 sw=4 :
