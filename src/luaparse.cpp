#include "rules.hpp"
#include "tokens.hpp"

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

std::ostream& operator<<(std::ostream& stream, const TokenType& type)
{
    switch (type) {
        case TokenType::Unknown: stream << "Unknown";
        case TokenType::Variable: stream << "Variable";
        case TokenType::Equal: stream << "Equal";
        case TokenType::EqualOperator: stream << "EqualOperator";
        case TokenType::UnequalOperator: stream << "UnequalOperator";
        case TokenType::TableLiteral: stream << "TableLiteral";
        case TokenType::StringLiteral: stream << "StringLiteral";
        case TokenType::Assignment: stream << "Assignment";
        case TokenType::ReturnStatement: stream << "ReturnStatement";
        case TokenType::Nil: stream << "Nil";
        case TokenType::True: stream << "True";
        case TokenType::False: stream << "False";
        case TokenType::Vararg: stream << "Vararg";
        case TokenType::IfStatement: stream << "IfStatement";
        case TokenType::FunctionCall: stream << "FunctionCall";
        default:
            std::stringstream str;
            str << "Unexpected TokenType: " << static_cast<int>(type);
            throw std::logic_error(str.str());
    }

    return stream;
}

std::ostream& operator<<(std::ostream& stream, const QString& value)
{
    stream << value.toUtf8().constData();
    return stream;
}

typedef Token<TokenType, QString> LuaToken;
typedef Node<TokenType, QString> LuaNode;

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

    auto stringLiteral = convert<LuaNode>(
        rule::quotedString,
        [](QString& value) {
            return LuaNode(LuaToken(TokenType::StringLiteral, value));
        }
    );

    auto variable = convert<LuaNode>(
        aggregate<QString>(
            multiple(simplePredicate<QChar>([](const QChar& input) {
                return input.isLetter() || input == '_';
            })),
            [](QString& target, const QChar& c) {
                target += c;
            }
        ),
        [](QString& value) {
            return LuaNode(LuaToken(TokenType::Variable, value));
        }
    );

    auto singleExpression = shared(proxyAlternative<QChar, LuaNode>(
        OrderedTokenRule<QChar, LuaNode>("nil", LuaNode(TokenType::Nil)),
        OrderedTokenRule<QChar, LuaNode>("true", LuaNode(TokenType::True)),
        OrderedTokenRule<QChar, LuaNode>("false", LuaNode(TokenType::False)),
        OrderedTokenRule<QChar, LuaNode>("...", LuaNode(TokenType::Vararg)),
        stringLiteral,
        convert<LuaNode>(
            rule::floating,
            [](const float& value) {
                return LuaNode(
                    LuaToken(TokenType::StringLiteral, QString::number(value))
                );
            }
        ),
        convert<LuaNode>(
            rule::integer,
            [](const long value) {
                return LuaNode(
                    LuaToken(TokenType::StringLiteral, QString::number(value))
                );
            }
        ),
        variable
    ));

    auto compoundExpression = shared(proxyAlternative<QChar, LuaNode>());

    auto expression = alternative<QChar, LuaNode>(
        compoundExpression,
        singleExpression
    );

    auto definition = proxySequence<QChar, LuaNode>(
        discard(proxySequence<QChar, QString>(
            OrderedTokenRule<QChar, QString>("local"),
            whitespace
        )),
        variable
    );

    auto ws = discard(optional(whitespace));

    auto tableLiteral = reduce<LuaNode>(
        proxySequence<QChar, LuaNode>(
            discard(OrderedTokenRule<QChar, QString>("[")),
            optional(proxySequence<QChar, LuaNode>(
                ws,
                expression,
                ws,
                optional(multiple(proxySequence<QChar, LuaNode>(
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
        [](Result<LuaNode>& target, Result<LuaNode>& items) {
            target << LuaNode(
                TokenType::TableLiteral,
                items.data()
            );
        }
    );

    singleExpression << tableLiteral;

    auto statement = shared(proxyAlternative<QChar, LuaNode>());

    auto statementLine = proxySequence<QChar, LuaNode>(
        statement,
        ws,
        discard(optional(OrderedTokenRule<QChar, LuaNode>(";"))),
        ws
    );

    auto block = multiple(statementLine);

    auto ifStatement = reduce<LuaNode>(
        proxySequence<QChar, LuaNode>(
            discard(OrderedTokenRule<QChar, LuaNode>("if")),
            ws,
            expression,
            ws,
            discard(OrderedTokenRule<QChar, LuaNode>("then")),
            ws,
            block,
            discard(OrderedTokenRule<QChar, LuaNode>("end")),
            ws
        ),
        [](Result<LuaNode>& results, Result<LuaNode>& subresults) {
            results << LuaNode(
                TokenType::IfStatement,
                subresults.data()
            );
        }
    );
    statement << ifStatement;

    auto functionCall = reduce<LuaNode>(
        proxySequence<QChar, LuaNode>(
            variable,
            ws,
            proxyAlternative<QChar, LuaNode>(
                proxySequence<QChar, LuaNode>(
                    discard(OrderedTokenRule<QChar, LuaNode>("(")),
                    ws,
                    optional(join<QChar, LuaNode>(
                        proxySequence<QChar, LuaNode>(
                            expression,
                            ws
                        ),
                        discard(proxySequence<QChar, LuaNode>(
                            discard(OrderedTokenRule<QChar, QString>(",")),
                            ws
                        ))
                    )),
                    ws,
                    discard(OrderedTokenRule<QChar, LuaNode>(")"))
                ),
                tableLiteral,
                stringLiteral
            ),
            ws
        ),
        [](Result<LuaNode>& results, Result<LuaNode>& subresults) {
            results << LuaNode(
                TokenType::FunctionCall,
                subresults.data()
            );
        }
    );
    statement << functionCall;
    compoundExpression << functionCall;

    auto returnStatement = reduce<LuaNode>(
        proxySequence<QChar, LuaNode>(
            discard(OrderedTokenRule<QChar, LuaNode>("return")),
            ws,
            expression,
            ws
        ),
        [](Result<LuaNode>& results, Result<LuaNode>& subresults) {
            results << LuaNode(
                TokenType::ReturnStatement,
                subresults.data()
            );
        }
    );
    statement << returnStatement;

    auto assignment = reduce<LuaNode>(
        proxySequence<QChar, LuaNode>(
            variable,
            discard(proxySequence<QChar, QString>(
                optional(whitespace),
                OrderedTokenRule<QChar, QString>("="),
                optional(whitespace)
            )),
            expression,
            ws
        ),
        [](Result<LuaNode>& results, Result<LuaNode>& subresults) {
            results << LuaNode(
                TokenType::Assignment,
                subresults.data()
            );
        }
    );

    auto operatorExpression = reduce<LuaNode>(
        proxySequence<QChar, LuaNode>(
            singleExpression,
            ws,
            alternative<QChar, LuaNode>(
                OrderedTokenRule<QChar, LuaNode>("==", TokenType::EqualOperator),
                OrderedTokenRule<QChar, LuaNode>("~=", TokenType::UnequalOperator)
            ),
            ws,
            expression,
            ws
        ),
        [](Result<LuaNode>& results, Result<LuaNode>& subresults) {
            results << LuaNode(
                TokenType::Assignment,
                subresults.data()
            );
        }
    );
    compoundExpression << operatorExpression;

    auto parser = proxySequence<QChar, LuaNode>(
        ws,
        block,
        make::end<QChar, LuaNode>()
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
    Result<LuaNode> nodes;

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
