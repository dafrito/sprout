#include "tokens.hpp"
#include "rules.hpp"

#include <iostream>
#include <cassert>
#include <unordered_map>

#include <QChar>
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

#include <QRegExp>
#include <QHash>
#include <QElapsedTimer>

using namespace sprout;

enum class TokenType {
    Unknown,
    Name,
    StringLiteral,
    Rule,
    TokenRule,
    ZeroOrMore,
    OneOrMore,
    Optional,
    Alternative,
    Sequence
};

namespace std {

template<>
struct hash<TokenType> {

    int operator()(const TokenType& type) const
    {
        return (int)type;
    }
};

} // namespace std

std::ostream& operator<<(std::ostream& stream, const TokenType& type)
{
    std::unordered_map<TokenType, const char*> names = {
        { TokenType::Unknown, "Unknown" },
        { TokenType::Name, "Name" },
        { TokenType::StringLiteral, "StringLiteral" },
        { TokenType::TokenRule, "TokenRule" },
        { TokenType::Rule, "Rule" },
        { TokenType::ZeroOrMore, "ZeroOrMore" },
        { TokenType::OneOrMore, "OneOrMore" },
        { TokenType::Optional, "Optional" },
        { TokenType::Alternative, "Alternative" },
        { TokenType::Sequence, "Sequence" },
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

static GNode NOOP(TokenType::Unknown);

void flattenRule(GNode& node)
{
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

void optimize(GNode& node)
{
    for (GNode& child : node.children()) {
        optimize(child);
    }
    flattenRule(node);
}

ProxyRule<QChar, QString> buildRule(RulesMap& rules, const GNode& node, const TokenType& ruleType)
{
    using namespace sprout::make;

    bool excludeWhitespace = ruleType == TokenType::Rule;
    auto ws = discard(optional(rule::whitespace<QString>()));

    switch (node.type()) {
        case TokenType::Sequence:
        {
            ProxySequenceRule<QChar, QString> rule;
            for (auto child : node.children()) {
                rule << buildRule(rules, child, ruleType);
                if (excludeWhitespace) {
                    rule << ws;
                }
            }
            if (ruleType == TokenType::TokenRule) {
                return reduce<QString>(
                    rule,
                    [](Result<QString>& dest, Result<QString>& src) {
                        QString cumulative;
                        while (src) {
                            cumulative += *src++;
                        }
                        dest.insert(cumulative);
                    }
                );
            }
            return rule;
        }
        case TokenType::Alternative:
        {
            ProxyAlternativeRule<QChar, QString> rule;
            for (auto child : node.children()) {
                rule << buildRule(rules, child, ruleType);
            }
            return rule;
        }
        case TokenType::ZeroOrMore:
        {
            return optional(multiple(buildRule(rules, node[0], ruleType)));
        }
        case TokenType::Optional:
        {
            return optional(buildRule(rules, node[0], ruleType));
        }
        case TokenType::OneOrMore:
        {
            return multiple(buildRule(rules, node[0], ruleType));
        }
        case TokenType::Name:
        {
            return rules[node.value()];
        }
        case TokenType::StringLiteral:
        {
            return discard(OrderedTokenRule<QChar, QString>(node.value()));
        }
        default:
        {
            std::stringstream str;
            str << "I don't know how to build a " << node.type() << " rule";
            throw std::runtime_error(str.str());
        }
    }
}

void parseGrammar(RulesMap& rules, const char* filename)
{
    using namespace sprout::make;

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
        [](QString& value) {
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
            proxyAlternative<QChar, GNode>(
                stringLiteral,
                name,
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
            if (src.size() == 2) {
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
                OrderedTokenRule<QChar, GNode>("Token", TokenType::TokenRule)
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

    for (GNode& node : nodes) {
        optimize(node);
        std::cout << node.dump() << std::endl;
        rules[node.value()] = buildRule(rules, node[0], node.type());
    }
}

int main(int argc, char* argv[])
{
    using namespace make;

    QHash<QString, SharedRule<ProxyRule<QChar, QString>>> rules;

    rules["alpha"] = ProxyRule<QChar, QString>([](Cursor<QChar>& iter, Result<QString>& result) {
        if (iter && (*iter).isLetter()) {
            result << *iter++;
            return true;
        }
        return false;
    });

    rules["alnum"] = ProxyRule<QChar, QString>([](Cursor<QChar>& iter, Result<QString>& result) {
        if (iter && (*iter).isLetterOrNumber()) {
            result << *iter++;
            return true;
        }
        return false;
    });

    rules["number"] = convert<QString>(
        rule::floating,
        [](const float& value) {
            return QString::number(value);
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
        Result<QString> nodes;

        QElapsedTimer timer;
        timer.start();
        auto parseSuccessful = parser(cursor, nodes);
        std::cout << "Parsing completed in " << timer.nsecsElapsed() << " ns\n";

        if (parseSuccessful) {
            for (auto node : nodes) {
                std::cout << node.toUtf8().constData() << std::endl;
            }
        } else {
            std::cout << "Failed to parse. :(\n";
        }
    }

    std::cout << std::endl;

    return 0;
}

// vim: set ts=4 sw=4 :
