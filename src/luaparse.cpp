#include "rules.hpp"

#include <iostream>
#include <cassert>

#include <QChar>
#include <QFile>
#include <QTextStream>
#include <QString>

#include "StreamIterator"
#include "TokenRule"
#include "DiscardRule"
#include "SharedRule"
#include "MultipleRule"
#include "OptionalRule"
#include "JoinRule"
#include "PredicateRule"
#include "ProxyRule"
#include "AlternativeRule"
#include "ReduceRule"
#include "LazyRule"
#include "LogRule"

#include <QRegExp>
#include <QHash>
#include <QElapsedTimer>

using namespace sprout;

enum class TokenType {
    Unknown,
    Variable,
    Equal,
    EqualOperator,
    UnequalOperator,
    TableLiteral,
    StringLiteral,
    Assignment,
    Nil,
    True,
    False,
    Vararg,
    FunctionCall,
    IfStatement,
    ReturnStatement
};

const char* tokenTypeName(const TokenType type)
{
    switch (type) {
        case TokenType::Unknown: return "Unknown";
        case TokenType::Variable: return "Variable";
        case TokenType::Equal: return "Equal";
        case TokenType::EqualOperator: return "EqualOperator";
        case TokenType::UnequalOperator: return "UnequalOperator";
        case TokenType::TableLiteral: return "TableLiteral";
        case TokenType::StringLiteral: return "StringLiteral";
        case TokenType::Assignment: return "Assignment";
        case TokenType::ReturnStatement: return "ReturnStatement";
        case TokenType::Nil: return "Nil";
        case TokenType::True: return "True";
        case TokenType::False: return "False";
        case TokenType::Vararg: return "Vararg";
        case TokenType::IfStatement: return "IfStatement";
        case TokenType::FunctionCall: return "FunctionCall";
        default:
            std::stringstream str;
            str << "Unexpected TokenType: " << static_cast<int>(type);
            throw std::logic_error(str.str());
    }
}

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

    Node() :
        Node(Token())
    {
    }

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

    const char* typeName() const
    {
        return tokenTypeName(type());
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

    void dump(std::stringstream& str, const std::string& indent) const
    {
        str << typeName();
        if (!value().isNull()) {
            str << ":" << value().toUtf8().constData();
        }
        if (!children.empty()) {
            auto childIndent = indent + "    ";
            str << " [\n" << childIndent;
            for (unsigned int i = 0; i < children.size(); ++i) {
                if (i > 0) {
                    str << ",\n" << childIndent;
                }
                children[i].dump(str, childIndent);
            }
            str << "\n" << indent << "]";
        }
    }

    std::string dump() const
    {
        std::stringstream str;
        dump(str, "");
        return str.str();
    }
};

bool neighborhood(Cursor<QChar>& orig, Result<Node>& results)
{
    auto iter = orig;
    QString str;
    QString indicator;

    const int NEIGHBOR_SIZE = 10;

    iter -= NEIGHBOR_SIZE;
    for (int i = 0; iter && i < NEIGHBOR_SIZE * 2 + 1; ++i) {
        auto c = *iter++;

        int size = 1;
        if (c == '\n') {
            str += "\\n";
            size = 2;
        } else if (c == '\t') {
            str += "\\t";
            size = 2;
        } else {
            str += c;
        }

        for (int j = 0; j < size; ++j) {
            indicator += i == NEIGHBOR_SIZE ? '^' : ' ';
        }
    }
    std::cout << str.toUtf8().constData() << std::endl;
    std::cout << indicator.toUtf8().constData() << std::endl;
    return true;
}

int main(int argc, char* argv[])
{
    using namespace make;

    auto lineComment = proxySequence<QChar, QString>(
        OrderedTokenRule<QChar, QString>("--"),
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

    auto stringLiteral = convert<Node>(
        rule::quotedString,
        [](QString& value) {
            return Node(Token(TokenType::StringLiteral, value));
        }
    );

    auto variable = convert<Node>(
        aggregate<QString>(
            multiple(simplePredicate<QChar>([](const QChar& input) {
                return input.isLetter() || input == '_';
            })),
            [](QString& target, const QChar& c) {
                target += c;
            }
        ),
        [](QString& value) {
            return Node(Token(TokenType::Variable, value));
        }
    );

    auto singleExpression = shared(proxyAlternative<QChar, Node>(
        OrderedTokenRule<QChar, Node>("nil", Node(TokenType::Nil)),
        OrderedTokenRule<QChar, Node>("true", Node(TokenType::True)),
        OrderedTokenRule<QChar, Node>("false", Node(TokenType::False)),
        OrderedTokenRule<QChar, Node>("...", Node(TokenType::Vararg)),
        stringLiteral,
        convert<Node>(
            rule::floating,
            [](const float& value) {
                return Node(
                    Token(TokenType::StringLiteral, QString::number(value))
                );
            }
        ),
        convert<Node>(
            rule::integer,
            [](const long value) {
                return Node(
                    Token(TokenType::StringLiteral, QString::number(value))
                );
            }
        ),
        variable
    ));

    auto compoundExpression = shared(proxyAlternative<QChar, Node>());

    auto expression = alternative<QChar, Node>(
        compoundExpression,
        singleExpression
    );

    auto definition = proxySequence<QChar, Node>(
        discard(proxySequence<QChar, QString>(
            OrderedTokenRule<QChar, QString>("local"),
            whitespace
        )),
        variable
    );

    auto ws = discard(optional(whitespace));

    auto tableLiteral = reduce<Node>(
        proxySequence<QChar, Node>(
            discard(OrderedTokenRule<QChar, QString>("[")),
            optional(proxySequence<QChar, Node>(
                ws,
                expression,
                ws,
                optional(multiple(proxySequence<QChar, Node>(
                    discard(OrderedTokenRule<QChar, QString>(",")),
                    ws,
                    expression,
                    ws
                ))),
                ws,
                discard(optional(OrderedTokenRule<QChar, QString>(",")))
            )),
            ws,
            discard(OrderedTokenRule<QChar, QString>("]"))
        ),
        [](Result<Node>& target, Result<Node>& items) {
            target << Node(
                TokenType::TableLiteral,
                items.data()
            );
        }
    );

    singleExpression << tableLiteral;

    auto statement = shared(proxyAlternative<QChar, Node>());

    auto statementLine = proxySequence<QChar, Node>(
        statement,
        ws,
        discard(optional(OrderedTokenRule<QChar, Node>(";"))),
        ws
    );

    auto block = multiple(statementLine);

    auto ifStatement = reduce<Node>(
        proxySequence<QChar, Node>(
            discard(OrderedTokenRule<QChar, Node>("if")),
            ws,
            expression,
            ws,
            discard(OrderedTokenRule<QChar, Node>("then")),
            ws,
            block,
            discard(OrderedTokenRule<QChar, Node>("end")),
            ws
        ),
        [](Result<Node>& results, Result<Node>& subresults) {
            results << Node(
                TokenType::IfStatement,
                subresults.data()
            );
        }
    );
    statement << ifStatement;

    auto functionCall = reduce<Node>(
        proxySequence<QChar, Node>(
            variable,
            ws,
            proxyAlternative<QChar, Node>(
                proxySequence<QChar, Node>(
                    discard(OrderedTokenRule<QChar, Node>("(")),
                    ws,
                    optional(join<QChar, Node>(
                        proxySequence<QChar, Node>(
                            expression,
                            ws
                        ),
                        discard(proxySequence<QChar, Node>(
                            discard(OrderedTokenRule<QChar, QString>(",")),
                            ws
                        ))
                    )),
                    ws,
                    discard(OrderedTokenRule<QChar, Node>(")"))
                ),
                tableLiteral,
                stringLiteral
            ),
            ws
        ),
        [](Result<Node>& results, Result<Node>& subresults) {
            results << Node(
                TokenType::FunctionCall,
                subresults.data()
            );
        }
    );
    statement << functionCall;
    compoundExpression << functionCall;

    auto returnStatement = reduce<Node>(
        proxySequence<QChar, Node>(
            discard(OrderedTokenRule<QChar, Node>("return")),
            ws,
            expression,
            ws
        ),
        [](Result<Node>& results, Result<Node>& subresults) {
            results << Node(
                TokenType::ReturnStatement,
                subresults.data()
            );
        }
    );
    statement << returnStatement;

    auto assignment = reduce<Node>(
        proxySequence<QChar, Node>(
            variable,
            discard(proxySequence<QChar, QString>(
                optional(whitespace),
                OrderedTokenRule<QChar, QString>("="),
                optional(whitespace)
            )),
            expression,
            ws
        ),
        [](Result<Node>& results, Result<Node>& subresults) {
            results << Node(
                TokenType::Assignment,
                subresults.data()
            );
        }
    );

    auto operatorExpression = reduce<Node>(
        proxySequence<QChar, Node>(
            singleExpression,
            ws,
            alternative<QChar, Node>(
                OrderedTokenRule<QChar, Node>("==", TokenType::EqualOperator),
                OrderedTokenRule<QChar, Node>("~=", TokenType::UnequalOperator)
            ),
            ws,
            expression,
            ws
        ),
        [](Result<Node>& results, Result<Node>& subresults) {
            results << Node(
                TokenType::Assignment,
                subresults.data()
            );
        }
    );
    compoundExpression << operatorExpression;

    auto parser = proxySequence<QChar, Node>(
        ws,
        block,
        make::end<QChar, Node>()
    );

    QString content;
    if (argc > 1) {
        QFile file(argv[1]);
        file.open(QFile::ReadOnly);
        QTextStream stream(&file);
        stream.setCodec("UTF-8");
        content = stream.readAll();
    } else {
        QTextStream stream(stdin);
        stream.setCodec("UTF-8");
        content = stream.readAll();
    }
    auto cursor = makeCursor<QChar>(&content);

    Result<Node> nodes;

    QElapsedTimer timer;
    timer.start();
    auto parseSuccessful = parser(cursor, nodes);
    std::cout << "Parsing completed in " << timer.nsecsElapsed() << " ns\n";

    if (!parseSuccessful) {
        std::cout << "Failed to parse. :(\n";
        return 1;
    }

    for (auto node : nodes) {
        std::cout << node.dump() << std::endl;
    }

    return 0;
}

// vim: set ts=4 sw=4 :
