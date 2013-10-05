#ifndef SPROUT_GRAMMAR_HEADER
#define SPROUT_GRAMMAR_HEADER

#include "tokens.hpp"
#include "rules.hpp"
#include "ProxyRule.hpp"
#include "SharedRule.hpp"
#include "ReduceRule.hpp"
#include "StreamIterator.hpp"
#include "TokenRule.hpp"
#include "DiscardRule.hpp"
#include "SharedRule.hpp"
#include "MultipleRule.hpp"
#include "OptionalRule.hpp"
#include "PredicateRule.hpp"
#include "ProxyRule.hpp"
#include "AlternativeRule.hpp"
#include "ReduceRule.hpp"
#include "JoinRule.hpp"
#include "LogRule.hpp"
#include "RecursiveRule.hpp"

#include <unordered_map>
#include <QElapsedTimer>
#include <QSet>

namespace sprout {

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

} // namespace sprout

namespace std {

template<>
struct hash<sprout::TokenType> {

    int operator()(const sprout::TokenType& type) const
    {
        return static_cast<int>(type);
    }
};

std::ostream& operator<<(std::ostream& stream, const sprout::TokenType& type);

} // namespace std

namespace sprout {

typedef Token<TokenType, QString> GToken;
typedef Node<TokenType, QString> GNode;
typedef ProxyRule<QChar, QString> GRule;

template <class Type, class Value>
class Grammar
{
public:
    typedef Node<Type, Value> PNode;
    typedef ProxyRule<QChar, PNode> PRule;

private:
    typedef QHash<QString, SharedRule<GRule>> RulesMap;

    QHash<QString, SharedRule<PRule>> _rules;
    ProxyRule<QChar, GNode> _grammarParser;
    QHash<QString, GNode> _parsedRules;

    ProxyRule<QChar, GNode>& grammarParser()
    {
        if (!_grammarParser) {
            _grammarParser = createGrammarParser();
        }
        return _grammarParser;
    }

    ProxyRule<QChar, GNode> createGrammarParser();

public:
    Grammar()
    {

        _rules["alpha"] = ProxyRule<QChar, PNode>([](Cursor<QChar>& iter, Result<PNode>& result) {
            if (iter && (*iter).isLetter()) {
                result << PNode("", *iter++);
                return true;
            }
            return false;
        });

        _rules["alnum"] = ProxyRule<QChar, PNode>([](Cursor<QChar>& iter, Result<PNode>& result) {
            if (iter && (*iter).isLetterOrNumber()) {
                result << PNode("", *iter++);
                return true;
            }
            return false;
        });

        _rules["string"] = make::convert<PNode>(
            make::rule<QChar, QString>(&rule::parseQuotedString),
            [](QString& value) {
                return PNode("string", value);
            }
        );

        _rules["number"] = make::convert<PNode>(
            make::rule<QChar, double>(&rule::parseFloating),
            [](const float& value) {
                return PNode("number", QString::number(value));
            }
        );
    }

    ProxyRule<QChar, PNode> buildRule(const GNode& node, const TokenType& ruleType)
    {
        using namespace sprout::make;

        bool excludeWhitespace = ruleType != TokenType::TokenRule;

        auto ws = discard(optional(multiple(proxyAlternative<QChar, QString>(
            rule::whitespace<QString>(),
            rule::lineComment("--")
        ))));

        switch (node.type()) {
            case TokenType::Sequence:
            {
                ProxySequenceRule<QChar, PNode> rule;
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
                ProxyRule<QChar, PNode> terminal = buildRule(node[0], ruleType);
                if (excludeWhitespace) {
                    terminal = make::proxySequence<QChar, PNode>(
                        terminal,
                        ws
                    );
                }
                return make::recursive(
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
                ProxyAlternativeRule<QChar, PNode> rule;
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
                    content = make::proxySequence<QChar, PNode>(content, ws);
                    separator = make::proxySequence<QChar, PNode>(separator, ws);
                }
                return join(content, separator);
            }
            case TokenType::ZeroOrMore:
            {
                return optional(multiple(buildRule(node[0], ruleType)));
            }
            case TokenType::Optional:
            {
                return optional(buildRule(node[0], ruleType));
            }
            case TokenType::Discard:
            {
                return discard(buildRule(node[0], ruleType));
            }
            case TokenType::OneOrMore:
            {
                return multiple(buildRule(node[0], ruleType));
            }
            case TokenType::Opaque:
            case TokenType::Name:
            {
                return _rules[node.value()];
            }
            case TokenType::Literal:
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
            _rules[node.value()] = make::reduce<PNode>(
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

    SharedRule<PRule> operator[](const char* name)
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
ProxyRule<QChar, GNode> Grammar<Type, Value>::createGrammarParser()
{
    using namespace make;

    auto whitespace = multiple(proxyAlternative<QChar, QString>(
        rule::whitespace<QString>(),
        rule::lineComment("#")
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
        [this](QString& value) {
            if (_rules.contains(value)) {
                return GNode(TokenType::Opaque, value);
            }
            return GNode(TokenType::Name, value);
        }
    );

    auto literal = convert<GNode>(
        make::rule<QChar, QString>(&rule::parseQuotedString),
        [](QString& value) {
            return GNode(TokenType::Literal, value);
        }
    );

    auto expression = shared(proxyAlternative<QChar, GNode>());

    auto singleExpression = reduce<GNode>(
        proxySequence<QChar, GNode>(
            optional(OrderedTokenRule<QChar, GNode>("-", TokenType::Discard)),
            ws,
            proxyAlternative<QChar, GNode>(
                literal,
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

    return proxySequence<QChar, GNode>(
        ws,
        multiple(rule),
        make::end<QChar, GNode>()
    );
}

} // namespace sprout

#endif // SPROUT_GRAMMAR_HEADER

// vim: set ts=4 sw=4 :
