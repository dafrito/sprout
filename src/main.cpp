#include "tokens.hpp"
#include "rules.hpp"

#include <iostream>
#include <cassert>
#include <unordered_map>
#include <unordered_set>

#include <QChar>
#include <QSet>
#include <QHash>
#include <QTextStream>
#include <QString>
#include <QFile>

#include "StreamIterator"
#include "TokenRule"
#include "DiscardRule"
#include "SharedRule"
#include "MultipleRule"
#include "OptionalRule"
#include "PredicateRule"
#include "ProxyRule"
#include "AlternativeRule"
#include "ReduceRule"
#include "JoinRule"
#include "LogRule"
#include "RecursiveRule"

#include <QRegExp>
#include <QHash>
#include <QElapsedTimer>

using namespace sprout;

enum class TokenType {
    Unknown,
    Name,
    Fundamental,
    StringLiteral,
    Rule,
    GroupRule,
    TokenRule,
    ZeroOrMore,
    Discard,
    OneOrMore,
    Optional,
    Alternative,
    Recursive,
    Join,
    Sequence
};

namespace std {

template<>
struct hash<TokenType> {

    int operator()(const TokenType& type) const
    {
        return static_cast<int>(type);
    }
};

} // namespace std

std::ostream& operator<<(std::ostream& stream, const TokenType& type)
{
    std::unordered_map<TokenType, const char*> names = {
        { TokenType::Unknown, "Unknown" },
        { TokenType::Name, "Name" },
        { TokenType::Fundamental, "Fundamental" },
        { TokenType::StringLiteral, "StringLiteral" },
        { TokenType::TokenRule, "TokenRule" },
        { TokenType::Rule, "Rule" },
        { TokenType::GroupRule, "GroupRule" },
        { TokenType::ZeroOrMore, "ZeroOrMore" },
        { TokenType::OneOrMore, "OneOrMore" },
        { TokenType::Discard, "Discard" },
        { TokenType::Optional, "Optional" },
        { TokenType::Alternative, "Alternative" },
        { TokenType::Join, "Join" },
        { TokenType::Sequence, "Sequence" },
        { TokenType::Recursive, "Recursive" },
    };

    stream << names.at(type);
    return stream;
}

std::ostream& operator<<(std::ostream& stream, const QString& value)
{
    stream << value.toUtf8().constData();
    return stream;
}

typedef Token<TokenType, QString> GToken;
typedef Node<TokenType, QString> GNode;
typedef ProxyRule<QChar, QString> GRule;
typedef QHash<QString, SharedRule<GRule>> RulesMap;

typedef Node<QString, QString> PNode;
typedef ProxyRule<QChar, PNode> PRule;
typedef QHash<QString, SharedRule<PRule>> PRulesMap;

void flattenRule(GNode& node)
{
    for (GNode& child : node.children()) {
        flattenRule(child);
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

enum class RecursionState {
    Unknown,
    Pending,
    Recursive,
    Terminal
};

bool hasRecursions(const QString& name, GNode& node, QHash<QString, GNode>& ruleMap, QHash<QString, RecursionState>& stateMap)
{
    switch (node.type()) {
        case TokenType::Name:
            switch (stateMap[node.value()]) {
                case RecursionState::Unknown:
                    return hasRecursions(node.value(), ruleMap[node.value()], ruleMap, stateMap);
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
                if (hasRecursions(name, child, ruleMap, stateMap)) {
                    return true;
                }
            }
            return false;
        case TokenType::Fundamental:
        case TokenType::StringLiteral:
            return false;
        case TokenType::Rule:
        case TokenType::GroupRule:
        case TokenType::TokenRule:
        {
            stateMap[node.value()] = RecursionState::Pending;
            auto result = hasRecursions(node.value(), node[0], ruleMap, stateMap);
            stateMap[node.value()] = result ? RecursionState::Recursive : RecursionState::Terminal;
            return result;
        }
        case TokenType::Sequence:
        {
            while (hasRecursions(name, node[0], ruleMap, stateMap)) {
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
                        for (unsigned int i = 0; i < first.children().size(); ++i) {
                            GNode& child = first[i];
                            if (child.type() == TokenType::Name && child.value() == name) {
                                // It's an alternative that's referring to us, so remove it
                                removable = i;
                                break;
                            }
                        }
                        if (removable >= 0) {
                            first.erase(removable);
                        } else {
                            throw std::runtime_error("I can't fix the recursion for this Alternative node");
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
        case TokenType::Recursive:
        case TokenType::ZeroOrMore:
        case TokenType::OneOrMore:
        case TokenType::Optional:
        case TokenType::Discard:
            return hasRecursions(name, node[0], ruleMap, stateMap);
        case TokenType::Unknown:
            throw std::logic_error("Unknown tokens must not be present");
        default:
            std::stringstream str;
            str << "I found an unsupported " << node.type() << " token while looking for recursions";
            throw std::logic_error(str.str());
    }
}

void optimize(QHash<QString, GNode>& ruleMap)
{
    QHash<QString, RecursionState> stateMap;
    for (auto iter = ruleMap.begin(); iter != ruleMap.end(); ++iter) {
        GNode& node = iter.value();
        flattenRule(node);
    }
    for (auto iter = ruleMap.begin(); iter != ruleMap.end(); ++iter) {
        GNode& node = iter.value();
        hasRecursions(iter.key(), node, ruleMap, stateMap);
    }
    for (auto iter = ruleMap.begin(); iter != ruleMap.end(); ++iter) {
        GNode& node = iter.value();
        flattenRule(node);
    }
}

ProxyRule<QChar, PNode> buildRule(PRulesMap& rules, const GNode& node, const TokenType& ruleType)
{
    using namespace sprout::make;

    bool excludeWhitespace = ruleType != TokenType::TokenRule;
    auto ws = discard(optional(rule::whitespace<QString>()));

    switch (node.type()) {
        case TokenType::Sequence:
        {
            ProxySequenceRule<QChar, PNode> rule;
            for (auto child : node.children()) {
                auto childRule = buildRule(rules, child, ruleType);
                if (child.type() == TokenType::StringLiteral) {
                    rule << discard(childRule);
                } else {
                    rule << childRule;
                }
                if (excludeWhitespace) {
                    rule << ws;
                }
            }
            if (ruleType == TokenType::TokenRule) {
                return reduce<PNode>(
                    rule,
                    [](Result<PNode>& dest, Result<PNode>& src) {
                        QString cumulative;
                        while (src) {
                            cumulative += src->value();
                            ++src;
                        }
                        dest.insert(PNode("", cumulative));
                    }
                );
            }
            return rule;
        }
        case TokenType::Recursive:
        {
            ProxyRule<QChar, PNode> terminal = buildRule(rules, node[0], ruleType);
            if (excludeWhitespace) {
                terminal = make::proxySequence<QChar, PNode>(
                    terminal,
                    ws
                );
            }
            return make::recursive(
                terminal,
                buildRule(rules, node[1], ruleType),
                [node](Result<PNode>& result) {
                    PNode recursiveNode(node.value());
                    while (result) {
                        recursiveNode.insert(*result++);
                    }
                    result.clear();
                    result.insert(recursiveNode);
                }
            );
        }
        case TokenType::Alternative:
        {
            ProxyAlternativeRule<QChar, PNode> rule;
            for (auto child : node.children()) {
                rule << buildRule(rules, child, ruleType);
            }
            return rule;
        }
        case TokenType::Join:
        {
            auto content = buildRule(rules, node[0], ruleType);
            auto separator = buildRule(rules, node[1], ruleType);
            if (node[1].type() == TokenType::StringLiteral) {
                separator = discard(separator);
            }
            if (excludeWhitespace) {
                content = make::proxySequence<QChar, PNode>(content, ws);
                separator = make::proxySequence<QChar, PNode>(separator, ws);
            }
            return join(content, separator);
        }
        case TokenType::ZeroOrMore:
        {
            return optional(multiple(buildRule(rules, node[0], ruleType)));
        }
        case TokenType::Optional:
        {
            return optional(buildRule(rules, node[0], ruleType));
        }
        case TokenType::Discard:
        {
            return discard(buildRule(rules, node[0], ruleType));
        }
        case TokenType::OneOrMore:
        {
            return multiple(buildRule(rules, node[0], ruleType));
        }
        case TokenType::Fundamental:
        case TokenType::Name:
        {
            return rules[node.value()];
        }
        case TokenType::StringLiteral:
        {
            return OrderedTokenRule<QChar, PNode>(node.value(), PNode("", node.value()));
        }
        default:
        {
            std::stringstream str;
            str << "I don't know how to build a " << node.type() << " rule";
            throw std::runtime_error(str.str());
        }
    }
}

void parseGrammar(PRulesMap& rules, const char* filename)
{
    using namespace sprout::make;

    QSet<QString> fundamentals;

    for (auto key : rules.keys()) {
        fundamentals << key;
    }

    auto lineComment = proxySequence<QChar, QString>(
        OrderedTokenRule<QChar, QString>("#"),
        [](Cursor<QChar>& iter, Result<QString>& result) {
            while (iter && *iter++ != '\n') {
                ;
            }
            return true;
        }
    );

    auto whitespace = multiple(proxyAlternative<QChar, QString>(
        rule::whitespace<QString>(),
        lineComment
    ));
    auto ws = discard(optional(whitespace));

    auto name = convert<GNode>(
        aggregate<QString>(
            multiple(simplePredicate<QChar>([](const QChar& input) {
                return input.isLetter() || input == '_';
            })),
            [](QString& target, const QChar& c) {
                target += c;
            }
        ),
        [&fundamentals](QString& value) {
            if (fundamentals.contains(value)) {
                return GNode(TokenType::Fundamental, value);
            }
            return GNode(TokenType::Name, value);
        }
    );

    auto stringLiteral = convert<GNode>(
        rule::quotedString,
        [](QString& value) {
            return GNode(TokenType::StringLiteral, value);
        }
    );

    auto expression = shared(proxyAlternative<QChar, GNode>());

    auto singleExpression = reduce<GNode>(
        proxySequence<QChar, GNode>(
            optional(OrderedTokenRule<QChar, GNode>("-", TokenType::Discard)),
            ws,
            proxyAlternative<QChar, GNode>(
                stringLiteral,
                name,
                reduce<GNode>(
                    proxySequence<QChar, GNode>(
                        discard(OrderedTokenRule<QChar, GNode>("{")),
                        ws,
                        expression,
                        expression,
                        ws,
                        discard(OrderedTokenRule<QChar, GNode>("}")),
                        ws
                    ),
                    [](Result<GNode>& dest, Result<GNode>& src) {
                        GNode joinNode(TokenType::Join);
                        while (src) {
                            joinNode.insert(*src++);
                        }
                        dest.insert(joinNode);
                    }
                ),
                reduce<GNode>(
                    proxySequence<QChar, GNode>(
                        discard(OrderedTokenRule<QChar, GNode>("(")),
                        ws,
                        multiple(expression),
                        ws,
                        discard(OrderedTokenRule<QChar, GNode>(")")),
                        ws
                    ),
                    [](Result<GNode>& dest, Result<GNode>& src) {
                        GNode parenNode(TokenType::Sequence);
                        while (src) {
                            parenNode.insert(*src++);
                        }
                        dest.insert(parenNode);
                    }
                )
            ),
            ws,
            optional(proxyAlternative<QChar, GNode>(
                OrderedTokenRule<QChar, GNode>("*", TokenType::ZeroOrMore),
                OrderedTokenRule<QChar, GNode>("+", TokenType::OneOrMore),
                OrderedTokenRule<QChar, GNode>("?", TokenType::Optional)
            )),
            ws
        ),
        [](Result<GNode>& results, Result<GNode>& src) {
            if (src[0].type() == TokenType::Discard) {
                GNode discardNode(TokenType::Discard);
                if (src.size() == 3) {
                    GNode node(src[2].type());
                    node.insert(src[1]);
                    discardNode.insert(node);
                } else {
                    discardNode.insert(src[1]);
                }
                results.insert(discardNode);
            } else if (src.size() == 2) {
                GNode node(src[1].type());
                node.insert(src[0]);
                results.insert(node);
            } else {
                results.insert(src);
            }
        }
    );

    expression << reduce<GNode>(
        proxySequence<QChar, GNode>(
            join<QChar, GNode>(
                proxySequence<QChar, GNode>(
                    singleExpression,
                    ws
                ),
                discard(proxySequence<QChar, GNode>(
                    discard(OrderedTokenRule<QChar, QString>("|")),
                    ws
                ))
            ),
            ws
        ),
        [](Result<GNode>& dest, Result<GNode>& src) {
            if (src.size() > 1) {
                dest.insert(GNode(TokenType::Alternative, src.data()));
            } else {
                dest.insert(*src);
            }
        }
    );

    auto rule = reduce<GNode>(
        proxySequence<QChar, GNode>(
            proxyAlternative<QChar, GNode>(
                OrderedTokenRule<QChar, GNode>("Rule", TokenType::Rule),
                OrderedTokenRule<QChar, GNode>("Token", TokenType::TokenRule),
                OrderedTokenRule<QChar, GNode>("Group", TokenType::GroupRule)
            ),
            ws,
            name,
            ws,
            discard(OrderedTokenRule<QChar, QString>("=")),
            ws,
            multiple(expression),
            ws,
            discard(OrderedTokenRule<QChar, QString>(";")),
            ws
        ),
        [](Result<GNode>& results, Result<GNode>& subresults) {
            GNode rule = *subresults++;

            rule.setValue(subresults->value());
            ++subresults;

            GNode sequence(TokenType::Sequence);
            while (subresults) {
                sequence.insert(*subresults++);
            }
            rule.insert(sequence);

            results << rule;
        }
    );

    auto parser = proxySequence<QChar, GNode>(
        ws,
        multiple(rule),
        make::end<QChar, GNode>()
    );

    QFile file(filename);
    if (!file.open(QFile::ReadOnly)) {
        std::stringstream str;
        str << "I couldn't open " << filename << " for reading";
        throw std::runtime_error(str.str());
    }
    QTextStream stream(&file);
    stream.setCodec("UTF-8");

    auto cursor = makeCursor<QChar>(&stream);
    Result<GNode> nodes;

    QElapsedTimer timer;
    timer.start();
    auto parseSuccessful = parser(cursor, nodes);
    std::cout << "Parsing completed in " << timer.nsecsElapsed() << " ns\n";

    if (!parseSuccessful) {
        throw std::runtime_error("Failed to parse. :(");
    }

    QHash<QString, GNode> nodeMap;
    for (GNode& node : nodes) {
        nodeMap[node.value()] = node;
    }

    optimize(nodeMap);

    for (GNode& node : nodeMap.values()) {
        std::cout << node.dump() << std::endl;
        rules[node.value()] = reduce<PNode>(
            buildRule(rules, node[0], node.type()),
            [node](Result<PNode>& dest, Result<PNode>& src) {
                switch (node.type()) {
                    case TokenType::GroupRule:
                        dest.insert(src);
                        break;
                    case TokenType::TokenRule:
                        if (src.size() == 1 && src[0].type() == "") {
                            src[0].setType(node.value());
                            dest << src[0];
                            break;
                        }
                        // Otherwise, fall through
                    case TokenType::Rule:
                    {
                        if (node[0].type() == TokenType::Recursive) {
                            // Recursive rules already create a group node, so don't double-nest it
                            dest.insert(src);
                            break;
                        }
                        PNode rv(node.value());
                        while (src) {
                            rv.insert(*src++);
                        }
                        dest << rv;
                        break;
                    }
                    default:
                        throw std::logic_error("Unexpected rule type");
                }
            }
        );
    }
}

int main(int argc, char* argv[])
{
    using namespace make;

    PRulesMap rules;

    rules["alpha"] = ProxyRule<QChar, PNode>([](Cursor<QChar>& iter, Result<PNode>& result) {
        if (iter && (*iter).isLetter()) {
            result << PNode("", *iter++);
            return true;
        }
        return false;
    });

    rules["alnum"] = ProxyRule<QChar, PNode>([](Cursor<QChar>& iter, Result<PNode>& result) {
        if (iter && (*iter).isLetterOrNumber()) {
            result << PNode("", *iter++);
            return true;
        }
        return false;
    });

    rules["number"] = convert<PNode>(
        rule::floating,
        [](const float& value) {
            return PNode("number", QString::number(value));
        }
    );

    if (argc <= 1) {
        throw std::logic_error("A grammar must be provided");
    } else {
        parseGrammar(rules, argv[1]);
    }

    auto parser = rules["main"];

    QTextStream stream(stdin);
    stream.setCodec("UTF-8");

    QString line;
    while (true) {
        std::cout << "sprout> ";
        line = stream.readLine();
        if (line.isNull()) {
            break;
        }

        QTextStream lineStream(&line);

        auto cursor = makeCursor<QChar>(&lineStream);
        Result<PNode> nodes;

        QElapsedTimer timer;
        timer.start();
        auto parseSuccessful = parser(cursor, nodes);
        std::cout << "Parsing completed in " << timer.nsecsElapsed() << " ns\n";

        if (parseSuccessful) {
            for (auto node : nodes) {
                std::cout << node.dump() << std::endl;
            }
        } else {
            std::cout << "Failed to parse. :(\n";
        }
    }

    std::cout << std::endl;

    return 0;
}

// vim: set ts=4 sw=4 :
