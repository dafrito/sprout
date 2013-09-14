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
    std::vector<Node*> children;

    Node(const Token& token) :
        token(token)
    {
    }

    TokenType tokenType() const
    {
        return token.type;
    }

    QString value() const
    {
        return token.value;
    }

    void add(Node* const child)
    {
        children.push_back(child);
    }

    Node* operator[](const int index) const
    {
        return children[index];
    }

    ~Node()
    {
        for (auto child : children) {
            delete child;
        }
    }
};

int main(int argc, char* argv[])
{
    using namespace make;

    auto whitespace = rule::whitespace<QString>();

    auto name = aggregate<QString>(
        multiple(simplePredicate<QChar>([](const QChar& input) {
            return input.isLetter() || input == '_';
        })),
        [](QString& target, const QChar& c) {
            target += c;
        }
    );

    auto definition = proxySequence<QChar, QString>(
        discard(proxySequence<QChar, QString>(
            OrderedTokenRule<QChar, QString>("var"),
            whitespace
        )),
        name
    );

    auto assignment = proxySequence<QChar, QString>(
        definition,
        discard(proxySequence<QChar, QString>(
            optional(whitespace),
            OrderedTokenRule<QChar, QString>("="),
            optional(whitespace)
)),
        rule::quotedString,
        optional(whitespace),
        optional(discard(OrderedTokenRule<QChar, QString>(";")))
    );

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
    }

    return 0;
}

// vim: set ts=4 sw=4 :
