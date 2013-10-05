#include <grammar/pass/LeftRecursion.hpp>

namespace sprout {
namespace grammar {
namespace pass {

/*

Left-recursive:
Rule expression = binop | number;
Rule binop = expression ('>' | '~=') expression;

Fixed:
Rule expression = binop | number;
Rule binop = number ('>' | '~=') expression;

AltFixed:
Rule expression = number (('>' | '~=') expression)?;

Left-recursive:
Rule prefixExpression = functionCall | variable;
Rule functionCall = prefixExpression arguments;


Incorrect:
Rule functionCall = variable arguments;

Fixed:


 */

bool LeftRecursion::hasRecursions(const QString& name, GNode& node, QHash<QString, GNode>& ruleMap)
{
    switch (node.type()) {
        case TokenType::Name:
            switch (stateMap[node.value()]) {
                case RecursionState::Unknown:
                {
                    GNode& resolved = ruleMap[node.value()];
                    if (resolved.type() == TokenType::Unknown) {
                        std::stringstream str;
                        str << "The named rule '" << node.value().toUtf8().constData() << "' could not be resolved\n";
                        throw std::runtime_error(str.str());
                    }
                    return hasRecursions(node.value(), resolved, ruleMap);
                }
                case RecursionState::Pending:
                case RecursionState::Recursive:
                    return true;
                case RecursionState::Terminal:
                    return false;
                default:
                    throw std::logic_error("Impossible");
            }
        case TokenType::Alternative:
            for (auto child : node.children()) {
                if (hasRecursions(name, child, ruleMap)) {
                    return true;
                }
            }
            return false;
        case TokenType::Opaque:
        case TokenType::Literal:
            return false;
        case TokenType::Rule:
        case TokenType::GroupRule:
        case TokenType::TokenRule:
        {
            stateMap[node.value()] = RecursionState::Pending;
            auto result = hasRecursions(node.value(), node[0], ruleMap);
            stateMap[node.value()] = result ? RecursionState::Recursive : RecursionState::Terminal;
            return result;
        }
        case TokenType::Sequence:
        {
            while (hasRecursions(name, node[0], ruleMap)) {
                GNode& first = node[0];
                switch (first.type()) {
                    case TokenType::GroupRule:
                    case TokenType::Rule:
                        node[0] = first[0];
                        break;
                    case TokenType::Name:
                        if (first.value() == name) {
                            node.erase(0);
                        } else {
                            node[0] = ruleMap[first.value()];
                        }
                        break;
                    case TokenType::Alternative:
                    {
                        int removable = -1;
                        int firstNamed = -1;
                        for (unsigned int i = 0; i < first.children().size(); ++i) {
                            GNode& child = first[i];
                            if (child.type() == TokenType::Name) {
                                if (child.value() == name) {
                                    // It's an alternative that's referring to us, so remove it
                                    removable = i;
                                    break;
                                } else {
                                    firstNamed = i;
                                }
                            }
                        }
                        if (removable >= 0) {
                            first.erase(removable);
                        } else if (firstNamed >= 0) {
                            first[firstNamed] = ruleMap[first[firstNamed].value()][0];
                            continue;
                        } else {
                            std::stringstream str;
                            str << "I can't fix the recursion for this Alternative node:\n";
                            throw std::runtime_error(str.str());
                        }

                        GNode recursor(TokenType::Recursive, name);
                        recursor.insert(first);
                        node.erase(0);
                        recursor.insert(node);

                        node = recursor;
                        break;
                    }
                    default:
                        std::stringstream str;
                        str << "I don't know how to fix a recursion for a " << first.type() << " node";
                        throw std::logic_error(str.str());
                }
            }
            return false;
        }
        case TokenType::Join:
        case TokenType::Recursive:
        case TokenType::ZeroOrMore:
        case TokenType::OneOrMore:
        case TokenType::Optional:
        case TokenType::Discard:
            return hasRecursions(name, node[0], ruleMap);
        case TokenType::Unknown:
            throw std::logic_error("Unknown tokens must not be present");
        default:
            std::stringstream str;
            str << "I found an unsupported " << node.type() << " token while looking for recursions";
            throw std::logic_error(str.str());
    }
}

} // namespace pass
} // namespace grammar
} // namespace sprout

// vim: set ts=4 sw=4 :
