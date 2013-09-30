#include "FlattenPass.hpp"

namespace sprout {

void FlattenPass::flatten(GNode& node)
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

} // namespace sprout

// vim: set ts=4 sw=4 :
