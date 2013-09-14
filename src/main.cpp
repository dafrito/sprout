#include "rules.hpp"

#include <iostream>
#include <cassert>

#include <QChar>
#include <QTextStream>
#include <QString>

#include "StreamIterator"
#include "TokenRule"
#include "DiscardRule"
#include "MultipleRule"
#include "OptionalRule"
#include "PredicateRule"
#include "ProxyRule"
#include "AlternativeRule"
#include "ReduceRule"

#include <QRegExp>
#include <QHash>
#include <QElapsedTimer>

using namespace sprout;

enum class TokenType {
    Unknown,
    Name,
    Equal,
    StringConstant,
    Assignment
};

struct Token {
    TokenType type;
    QString value;

    Token() :
        type(TokenType::Unknown)
    {
    }

    Token(const TokenType& type) :
        type(type)
    {
    }

    Token(const TokenType& type, const QString& value) :
        type(type),
        value(value)
    {
    }
};

struct Node {
    Token token;
    std::vector<Node> children;

    Node(const Token& token) :
        token(token)
    {
    }

    Node(const Token& token, std::vector<Node> children) :
        token(token),
        children(children)
    {
    }

    TokenType type() const
    {
        return token.type;
    }

    QString value() const
    {
        return token.value;
    }

    void add(const Node& child)
    {
        children.push_back(child);
    }

    Node operator[](const int index) const
    {
        return children[index];
    }
};

int main(int argc, char* argv[])
{
    using namespace make;

    auto whitespace = rule::whitespace<QString>();

    auto name = convert<Node>(
        aggregate<QString>(
            multiple(simplePredicate<QChar>([](const QChar& input) {
                return input.isLetter() || input == '_';
            })),
            [](QString& target, const QChar& c) {
                target += c;
            }
        ),
        [](QString& value) {
            return Node(Token(TokenType::Name, value));
        }
    );

    auto definition = proxySequence<QChar, Node>(
        discard(proxySequence<QChar, QString>(
            OrderedTokenRule<QChar, QString>("var"),
            whitespace
        )),
        name
    );

    auto stringConstant = convert<Node>(
        rule::quotedString,
        [](QString& value) {
            return Node(Token(TokenType::StringConstant, value));
        }
    );

    auto rvalue = proxyAlternative<QChar, Node>(
        name,
        stringConstant
    );

    auto assignment = reduce<Node>(
        proxySequence<QChar, Node>(
            name,
            discard(proxySequence<QChar, QString>(
                optional(whitespace),
                OrderedTokenRule<QChar, QString>("="),
                optional(whitespace)
            )),
            rvalue,
            discard(optional(whitespace)),
            discard(optional(OrderedTokenRule<QChar, QString>(";")))
        ),
        [](Result<Node>& results, Result<Node>& subresults) {
            results << Node(
                TokenType::Assignment,
                subresults.data()
            );
        }
    );

    auto parser = proxySequence<QChar, Node>(
        proxyAlternative<QChar, Node>(
            assignment,
            name
        ),
        make::end<QChar, Node>()
    );

    QTextStream stream(stdin);
    stream.setCodec("UTF-8");

    QHash<QString, QString> globals;

    QString line;
    while (true) {
        std::cout << "sprout> ";
        line = stream.readLine();
        if (line.isNull()) {
            break;
        }
        QTextStream lineStream(&line);
        auto cursor = makeCursor<QChar>(&lineStream);

        Result<Node> nodes;
        if (parser(cursor, nodes)) {
            for (auto node : nodes) {
                switch (node.type()) {
                    case TokenType::Name:
                        {
                            auto name = node.value();
                            std::cout << globals[name].toUtf8().constData() << std::endl;
                            break;
                        }
                    case TokenType::Assignment:
                        {
                            auto name = node[0].value();
                            auto result = node[1];
                            switch (result.type()) {
                                case TokenType::Name:
                                    globals[name] = globals[result.value()];
                                    break;
                                case TokenType::StringConstant:
                                    globals[name] = result.value();
                                    break;
                                default:
                                    std::cout << "Unhandled type: " << static_cast<int>(node.type()) << std::endl;
                                    break;
                            }
                            break;
                        }
                    default:
                        std::cout << "Unhandled type: " << static_cast<int>(node.type()) << std::endl;
                        break;
                }
            }
        }
    }

    return 0;
}

// vim: set ts=4 sw=4 :
