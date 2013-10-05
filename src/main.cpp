#include "Node.hpp"
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

#include "Grammar.hpp"
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

#include "FlattenPass.hpp"
#include "LeftRecursionPass.hpp"

#include <QRegExp>
#include <QHash>
#include <QElapsedTimer>

using namespace sprout;

std::ostream& operator<<(std::ostream& stream, const QString& value)
{
    stream << value.toUtf8().constData();
    return stream;
}

template <class Node>
void parseLine(ProxyRule<QChar, Node> parser, QString& line)
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
    using namespace make;

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

    FlattenPass()(grammar);

    for (auto node : grammar) {
        std::cout << node.dump() << std::endl;
    }
    LeftRecursionPass()(grammar);
    FlattenPass()(grammar);
    grammar.build();

    auto lineComment = proxySequence<QChar, PNode>(
        OrderedTokenRule<QChar, PNode>("--"),
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
