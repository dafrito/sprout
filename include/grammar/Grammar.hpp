#ifndef SPROUT_GRAMMAR_HEADER
#define SPROUT_GRAMMAR_HEADER

#include "Node.hpp"

#include <rule/rules.hpp>
#include <rule/Proxy.hpp>
#include <rule/Shared.hpp>
#include <rule/Reduce.hpp>
#include <rule/Literal.hpp>
#include <rule/Discard.hpp>
#include <rule/Shared.hpp>
#include <rule/Multiple.hpp>
#include <rule/Optional.hpp>
#include <rule/Predicate.hpp>
#include <rule/Alternative.hpp>
#include <rule/Reduce.hpp>
#include <rule/Join.hpp>
#include <rule/Recursive.hpp>

#include <unordered_map>
#include <QElapsedTimer>
#include <QSet>
#include <QString>
#include <QChar>

namespace sprout {
namespace grammar {

enum class TokenType {
    Unknown,

    /**
     * A named reference to some other rule.
     */
    Name,

    /**
     * An ordered sequence of subrules.
     */
    Sequence,

    /**
     * A rule to match a literal string.
     */
    Literal,

    /**
     * An opaque rule, usually one implmeented in C++ for primitive matching.
     */
    Opaque,

    /**
     * A rule that matches repeatedly. Analagous to the + in regular expressions.
     */
    OneOrMore,

    /**
     * A rule that will attempt to match its subrule, but will pass regardless.
     */
    Optional,

    /**
     * A rule that allows selection from a set of possible choices.
     */
    Alternative,

    /**
     * A rule that implements left-recursion. This cannot be created directly in a grammar.
     */
    Recursive,

    /**
     * A rule that discards the output, but forwards its subrule's result. In other words,
     * discard does not imply optional.
     */
    Discard,

    // Convenience rules. These could be implemented using our primitives.
    Join,
    ZeroOrMore,

    // Rule types
    Rule,
    GroupRule,
    TokenRule
};

} // namespace grammar
} // namespace sprout

namespace std {

template<>
struct hash<sprout::grammar::TokenType> {

    int operator()(const sprout::grammar::TokenType& type) const
    {
        return static_cast<int>(type);
    }
};

std::ostream& operator<<(std::ostream& stream, const sprout::grammar::TokenType& type);

} // namespace std

namespace sprout {
namespace grammar {

typedef Token<TokenType, QString> GToken;
typedef Node<TokenType, QString> GNode;
typedef rule::Proxy<QChar, QString> GRule;

template <class Type, class Value>
class Grammar
{
public:
    typedef Node<Type, Value> PNode;
    typedef rule::Proxy<QChar, PNode> PRule;

private:
    typedef QHash<QString, rule::Shared<GRule>> RulesMap;

    QHash<QString, rule::Shared<PRule>> _rules;
    rule::Proxy<QChar, GNode> _grammarParser;
    QHash<QString, GNode> _parsedRules;

    rule::Proxy<QChar, GNode>& grammarParser()
    {
        return _grammarParser;
    }

    rule::Proxy<QChar, GNode> createGrammarParser();

public:
    Grammar()
    {

        _rules["alpha"] = rule::Proxy<QChar, PNode>([](Cursor<QChar>& iter, Result<PNode>& result) {
            if (iter && (*iter).isLetter()) {
                result << PNode("", *iter++);
                return true;
            }
            return false;
        });

        _rules["alnum"] = rule::Proxy<QChar, PNode>([](Cursor<QChar>& iter, Result<PNode>& result) {
            if (iter && (*iter).isLetterOrNumber()) {
                result << PNode("", *iter++);
                return true;
            }
            return false;
        });

        _rules["string"] = rule::convert<PNode>(
            rule::wrap<QChar, QString>(&rule::parseQuotedString),
            [](QString& value) {
                return PNode("string", value);
            }
        );

        _rules["number"] = rule::convert<PNode>(
            rule::wrap<QChar, double>(&rule::parseFloating),
            [](const float& value) {
                return PNode("number", QString::number(value));
            }
        );

        _grammarParser = createGrammarParser();
    }

    rule::Proxy<QChar, PNode> buildRule(const GNode& node, const TokenType& ruleType)
    {
        bool excludeWhitespace = ruleType != TokenType::TokenRule;

        auto ws = rule::discard(rule::optional(rule::multiple(rule::tupleAlternative<QChar, QString>(
            rule::whitespace<QString>(),
            rule::lineComment("--")
        ))));

        switch (node.type()) {
            case TokenType::Sequence:
            {
                rule::ProxySequence<QChar, PNode> rule;
                for (auto child : node.children()) {
                    auto childRule = buildRule(child, ruleType);
                    if (child.type() == TokenType::Literal) {
                        rule << discard(childRule);
                    } else {
                        rule << childRule;
                    }
                    if (excludeWhitespace) {
                        rule << ws;
                    }
                }
                if (ruleType == TokenType::TokenRule) {
                    return rule::reduce<PNode>(
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
                rule::Proxy<QChar, PNode> terminal = buildRule(node[0], ruleType);
                if (excludeWhitespace) {
                    terminal = rule::tupleSequence<QChar, PNode>(
                        terminal,
                        ws
                    );
                }
                return rule::recursive(
                    terminal,
                    buildRule(node[1], ruleType),
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
                rule::ProxyAlternative<QChar, PNode> rule;
                for (auto child : node.children()) {
                    rule << buildRule(child, ruleType);
                }
                return rule;
            }
            case TokenType::Join:
            {
                auto content = buildRule(node[0], ruleType);
                auto separator = buildRule(node[1], ruleType);
                if (node[1].type() == TokenType::Literal) {
                    separator = discard(separator);
                }
                if (excludeWhitespace) {
                    content = rule::tupleSequence<QChar, PNode>(content, ws);
                    separator = rule::tupleSequence<QChar, PNode>(separator, ws);
                }
                return rule::join(content, separator);
            }
            case TokenType::ZeroOrMore:
            {
                return rule::optional(rule::multiple(buildRule(node[0], ruleType)));
            }
            case TokenType::Optional:
            {
                return rule::optional(buildRule(node[0], ruleType));
            }
            case TokenType::Discard:
            {
                return rule::discard(buildRule(node[0], ruleType));
            }
            case TokenType::OneOrMore:
            {
                return rule::multiple(buildRule(node[0], ruleType));
            }
            case TokenType::Opaque:
            case TokenType::Name:
            {
                return _rules[node.value()];
            }
            case TokenType::Literal:
            {
                return rule::OrderedLiteral<QChar, PNode>(node.value(), PNode("", node.value()));
            }
            default:
            {
                std::stringstream str;
                str << "I don't know how to build a " << node.type() << " rule";
                throw std::runtime_error(str.str());
            }
        }
    }

    void readGrammar(Cursor<QChar>& cursor)
    {
        Result<GNode> nodes;

        QElapsedTimer timer;
        timer.start();
        auto parseSuccessful = grammarParser()(cursor, nodes);
        std::cout << "Parsing completed in " << timer.nsecsElapsed() << " ns\n";

        if (!parseSuccessful) {
            throw std::runtime_error("Failed to parse grammar. :(");
        }

        for (GNode& node : nodes) {
            _parsedRules[node.value()] = node;
        }
    }

    void build()
    {
        for (GNode& node : _parsedRules.values()) {
            _rules[node.value()] = rule::reduce<PNode>(
                buildRule(node[0], node.type()),
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

    rule::Shared<PRule> operator[](const char* name)
    {
        return _rules[name];
    }

    decltype(_parsedRules)& parsedRules()
    {
        return _parsedRules;
    }

    typename decltype(_parsedRules)::iterator begin()
    {
        return _parsedRules.begin();
    }

    typename decltype(_parsedRules)::iterator end()
    {
        return _parsedRules.end();
    }
};

template <class Type, class Value>
rule::Proxy<QChar, GNode> Grammar<Type, Value>::createGrammarParser()
{

    auto whitespace = rule::multiple(rule::tupleAlternative<QChar, QString>(
        rule::whitespace<QString>(),
        rule::lineComment("#")
    ));
    auto ws = rule::discard(rule::optional(whitespace));

    auto name = rule::convert<GNode>(
        rule::aggregate<QString>(
            rule::multiple(rule::simplePredicate<QChar>([](const QChar& input) {
                return input.isLetter() || input == '_';
            })),
            [](QString& target, const QChar& c) {
                target += c;
            }
        ),
        [this](QString& value) {
            if (_rules.contains(value)) {
                return GNode(TokenType::Opaque, value);
            }
            return GNode(TokenType::Name, value);
        }
    );

    auto literal = rule::convert<GNode>(
        rule::wrap<QChar, QString>(&rule::parseQuotedString),
        [](QString& value) {
            return GNode(TokenType::Literal, value);
        }
    );

    auto expression = rule::shared(rule::proxyAlternative<QChar, GNode>());

    auto singleExpression = rule::reduce<GNode>(
        rule::tupleSequence<QChar, GNode>(
            rule::optional(rule::OrderedLiteral<QChar, GNode>("-", TokenType::Discard)),
            ws,
            rule::tupleAlternative<QChar, GNode>(
                literal,
                name,
                rule::reduce<GNode>(
                    rule::tupleSequence<QChar, GNode>(
                        rule::discard(rule::OrderedLiteral<QChar, GNode>("{")),
                        ws,
                        expression,
                        expression,
                        ws,
                        rule::discard(rule::OrderedLiteral<QChar, GNode>("}")),
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
                rule::reduce<GNode>(
                    rule::tupleSequence<QChar, GNode>(
                        rule::discard(rule::OrderedLiteral<QChar, GNode>("(")),
                        ws,
                        rule::multiple(expression),
                        ws,
                        rule::discard(rule::OrderedLiteral<QChar, GNode>(")")),
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
            rule::optional(rule::tupleAlternative<QChar, GNode>(
                rule::OrderedLiteral<QChar, GNode>("*", TokenType::ZeroOrMore),
                rule::OrderedLiteral<QChar, GNode>("+", TokenType::OneOrMore),
                rule::OrderedLiteral<QChar, GNode>("?", TokenType::Optional)
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

    expression << rule::reduce<GNode>(
        rule::tupleSequence<QChar, GNode>(
            rule::join<QChar, GNode>(
                rule::tupleSequence<QChar, GNode>(
                    singleExpression,
                    ws
                ),
                rule::discard(rule::tupleSequence<QChar, GNode>(
                    rule::discard(rule::OrderedLiteral<QChar, QString>("|")),
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

    auto rule = rule::reduce<GNode>(
        rule::tupleSequence<QChar, GNode>(
            rule::tupleAlternative<QChar, GNode>(
                rule::OrderedLiteral<QChar, GNode>("Rule", TokenType::Rule),
                rule::OrderedLiteral<QChar, GNode>("Token", TokenType::TokenRule),
                rule::OrderedLiteral<QChar, GNode>("Group", TokenType::GroupRule)
            ),
            ws,
            name,
            ws,
            rule::discard(rule::OrderedLiteral<QChar, QString>("=")),
            ws,
            rule::multiple(expression),
            ws,
            rule::discard(rule::OrderedLiteral<QChar, QString>(";")),
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

    return rule::tupleSequence<QChar, GNode>(
        ws,
        rule::multiple(rule),
        rule::end<QChar, GNode>()
    );
}

} // namespace grammar
} // namespace sprout

#endif // SPROUT_GRAMMAR_HEADER

// vim: set ts=4 sw=4 :
