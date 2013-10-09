#include <grammar/Grammar.hpp>
#include <grammar/Node.hpp>
#include <grammar/pass/Flatten.hpp>
#include <grammar/pass/LeftRecursion.hpp>

#include <rule/rules.hpp>
#include <rule/Literal.hpp>
#include <rule/Discard.hpp>
#include <rule/Shared.hpp>
#include <rule/Multiple.hpp>
#include <rule/Optional.hpp>
#include <rule/Predicate.hpp>
#include <rule/Proxy.hpp>
#include <rule/Alternative.hpp>
#include <rule/Reduce.hpp>
#include <rule/Join.hpp>
#include <rule/Log.hpp>
#include <rule/Recursive.hpp>

#include <StreamIterator.hpp>

#include <QChar>
#include <QSet>
#include <QHash>
#include <QTextStream>
#include <QString>
#include <QFile>
#include <QRegExp>
#include <QHash>
#include <QElapsedTimer>

#include <iostream>
#include <cassert>
#include <unordered_map>
#include <unordered_set>

using namespace sprout;

std::ostream& operator<<(std::ostream& stream, const QString& value)
{
    stream << value.toUtf8().constData();
    return stream;
}

template <class Node>
void parseLine(rule::Proxy<QChar, Node> parser, QString& line)
{
    QTextStream lineStream(&line);

    auto cursor = makeCursor<QChar>(&lineStream);
    Result<Node> nodes;

    QElapsedTimer timer;
    timer.start();
    auto parseSuccessful = parser(cursor, nodes);
    std::cout << "Parsing completed in " << timer.nsecsElapsed() << " ns\n";

    if (parseSuccessful) {
        for (auto node : nodes) {
            std::cout << node.dump() << std::endl;
        }
    } else {
        std::cout << "Failed to parse provided line. :(\n";
    }
}

int main(int argc, char* argv[])
{
    using namespace rule;
    using namespace grammar;

    Grammar<QString, QString> grammar;
    typedef Node<QString, QString> PNode;

    if (argc <= 1) {
        throw std::logic_error("A grammar must be provided");
    } else {
        auto filename = argv[1];
        QFile file(filename);
        if (!file.open(QFile::ReadOnly)) {
            std::stringstream str;
            str << "I couldn't open " << filename << " for reading";
            throw std::runtime_error(str.str());
        }
        QTextStream stream(&file);
        stream.setCodec("UTF-8");

        auto cursor = makeCursor<QChar>(&stream);
        grammar.readGrammar(cursor);
    }

    auto flattenPass = pass::Flatten<TokenType, QString>({
        TokenType::Alternative,
        TokenType::Sequence
    });
    flattenPass(grammar);

    for (auto node : grammar) {
        std::cout << node.dump() << std::endl;
    }
    pass::LeftRecursion()(grammar);
    flattenPass(grammar);
    grammar.build();

    auto lineComment = proxySequence<QChar, PNode>(
        rule::qLiteral<PNode>("--"),
        [](Cursor<QChar>& iter, Result<PNode>& result) {
            while (iter && *iter++ != '\n') {
                ;
            }
            return true;
        }
    );

    auto ws = discard(optional(multiple(proxyAlternative<QChar, PNode>(
        rule::whitespace<PNode>(),
        lineComment
    ))));

    auto parser = proxySequence<QChar, PNode>(
        ws,
        grammar["main"]
    );

    if (argc > 2) {
        for (int i = 2; i < argc; ++i) {
            QFile file(argv[i]);
            if (file.open(QFile::ReadOnly)) {
                QTextStream stream(&file);
                stream.setCodec("UTF-8");

                std::cout << argv[i] << std::endl;
                auto text = stream.readAll();
                parseLine<PNode>(parser, text);
            } else {
                std::cout << "Failed to open file: " << argv[i] << std::endl;
            }
        }
    }

    QTextStream stream(stdin);
    stream.setCodec("UTF-8");

    QString line;
    while (true) {
        std::cout << "sprout> ";
        line = stream.readLine();
        if (line.isNull()) {
            break;
        }

        parseLine<PNode>(parser, line);
    }

    std::cout << std::endl;

    return 0;
}

// vim: set ts=4 sw=4 :
