#include "llvm.hpp"

#include <grammar/Node.hpp>

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
#include <rule/Recursive.hpp>

#include "StreamIterator.hpp"

#include <QChar>
#include <QSet>
#include <QHash>
#include <QTextStream>
#include <QString>
#include <QHash>
#include <QElapsedTimer>

#include <llvm/Analysis/Passes.h>
#include <llvm/Analysis/Verifier.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/JIT.h>
#include <llvm/IR/DataLayout.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/PassManager.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Transforms/Scalar.h>

#include <iostream>
#include <cassert>
#include <unordered_map>
#include <unordered_set>
#include <cstdio>
#include <map>
#include <string>
#include <vector>

using namespace sprout;

std::ostream& operator<<(std::ostream& stream, const QString& value)
{
    stream << value.toUtf8().constData();
    return stream;
}

std::unordered_map<ASTType, const char*> names = {
    { ASTType::Unknown,        "Unknown" },
    { ASTType::NumberLiteral,  "NumberLiteral" },
    { ASTType::Add,            "Add" },
    { ASTType::Subtract,       "Subtract" },
    { ASTType::Multiply,       "Multiply" },
    { ASTType::Divide,         "Divide" },
    { ASTType::Exponent,       "Exponent" },
    { ASTType::Or,             "Or" },
    { ASTType::And,            "And" },
    { ASTType::Function,       "Function" },
    { ASTType::Variable,       "Variable" },
    { ASTType::ArgumentList,   "ArgumentList" },
    { ASTType::Call,           "Call" },
    { ASTType::If,             "If" },
};

std::unordered_map<ASTType, int> operatorPrecedence = {
    { ASTType::Or,       1 },
    { ASTType::And,      1 },
    { ASTType::Add,      2 },
    { ASTType::Subtract, 2 },
    { ASTType::Multiply, 3 },
    { ASTType::Divide,   3 },
    { ASTType::Exponent, 4 },
};

int sprout::compare(const ASTType& left, const ASTType& right)
{
    return operatorPrecedence[left] - operatorPrecedence[right];
}

typedef grammar::Node<ASTType, QString> ASTNode;

int compare(const ASTNode& left, const ASTNode& right)
{
    return sprout::compare(left.type(), right.type());
}

rule::Proxy<QChar, ASTNode> buildParser()
{
    using namespace rule;

    auto ws = discard(multiple(proxyAlternative<QChar, ASTNode>(
        rule::whitespace<ASTNode>(),
        discard(rule::lineComment("#"))
    )));

    auto name = convert<ASTNode>(
        rule::variable(),
        [](const QString& str) {
            return ASTNode(ASTType::Variable, str);
        }
    );

    auto single = shared(proxyAlternative<QChar, ASTNode>(
        convert<ASTNode>(
            rule::wrap<QChar, double>(&rule::parseFloating),
            [](const double& value) {
                return ASTNode(ASTType::NumberLiteral, QString::number(value));
            }
        ),
        name
    ));

    auto expression = reduce<ASTNode>(join(
        single,
        proxySequence<QChar, ASTNode>(
            optional(ws),
            proxyAlternative<QChar, ASTNode>(
                rule::OrderedLiteral<QChar, ASTNode>("||", ASTType::Or),
                rule::OrderedLiteral<QChar, ASTNode>("&&", ASTType::And),
                rule::OrderedLiteral<QChar, ASTNode>("+", ASTType::Add),
                rule::OrderedLiteral<QChar, ASTNode>("-", ASTType::Subtract),
                rule::OrderedLiteral<QChar, ASTNode>("/", ASTType::Divide),
                rule::OrderedLiteral<QChar, ASTNode>("*", ASTType::Multiply),
                rule::OrderedLiteral<QChar, ASTNode>("^", ASTType::Exponent)
            ),
            optional(ws)
        )),
        [](Result<ASTNode>& dest, Result<ASTNode>& src) {
            ASTNode left = *src++;
            if (!src) {
                dest << left;
                return;
            }

            ASTNode tmp = *src++;
            tmp << left;
            left = tmp;
            left << *src++;

            ASTNode* pos = &left;
            while (src) {
                ASTNode rightOp(*src++);

                if (compare(left, rightOp) < 0) {
                    // e.g. 2 + 3 / 4
                    // (+ 2 3) -> (+ 2 (/ 3 4))
                    rightOp << pos->at(1) << *src++;
                    pos->erase(1);
                    *pos << rightOp;
                    pos = &pos->at(1);
                } else {
                    // e.g. 2 / 3 + 4
                    // (/ 2 3) -> (+ (/ 2 3) 4)
                    rightOp << left << *src++;
                    left = rightOp;
                    pos = &left;
                }
            }
            dest << left;
        }
    );

    single << proxySequence<QChar, ASTNode>(
        discard(rule::OrderedLiteral<QChar, ASTNode>("(")),
        optional(ws),
        expression,
        optional(ws),
        discard(rule::OrderedLiteral<QChar, ASTNode>(")"))
    );

    auto statement = shared(proxyAlternative<QChar, ASTNode>());

    auto functionCall = reduce<ASTNode>(
        proxySequence<QChar, ASTNode>(
            name,
            optional(ws),
            discard(rule::OrderedLiteral<QChar, QChar>("(")),
            optional(ws),
            optional(join(
                proxySequence<QChar, ASTNode>(
                    optional(ws),
                    expression,
                    optional(ws)
                ),
                discard(rule::OrderedLiteral<QChar, ASTNode>(","))
            )),
            discard(rule::OrderedLiteral<QChar, QChar>(")"))
        ),
        [](Result<ASTNode>& dest, Result<ASTNode>& src) {
            ASTNode funcNode(ASTType::Call, src[0].value());
            ++src;
            while (src) {
                funcNode << *src++;
            }
            dest << funcNode;
        }
    );
    statement << functionCall;

    auto ifStatement = reduce<ASTNode>(
        proxySequence<QChar, ASTNode>(
            discard(rule::OrderedLiteral<QChar, QChar>("if")),
            optional(ws),
            expression,
            optional(ws),
            discard(rule::OrderedLiteral<QChar, QChar>("then")),
            optional(ws),
            expression,
            optional(ws),
            optional(proxySequence<QChar, ASTNode>(
                optional(ws),
                discard(rule::OrderedLiteral<QChar, QChar>("else")),
                optional(ws),
                expression
            )),
            optional(ws)
        ),
        [](Result<ASTNode>& dest, Result<ASTNode>& src) {
            ASTNode ifNode(ASTType::If);
            ifNode << src;
            dest << ifNode;
        }
    );
    statement << ifStatement;

    auto functionDeclaration = reduce<ASTNode>(
        proxySequence<QChar, ASTNode>(
            discard(rule::OrderedLiteral<QChar, QChar>("def")),
            ws,
            name,
            optional(ws),
            discard(rule::OrderedLiteral<QChar, QChar>("(")),
            optional(ws),
            reduce<ASTNode>(
                optional(join(
                    proxySequence<QChar, ASTNode>(
                        optional(ws),
                        name,
                        optional(ws)
                    ),
                    discard(rule::OrderedLiteral<QChar, ASTNode>(","))
                )),
                [](Result<ASTNode>& dest, Result<ASTNode>& src) {
                    ASTNode args(ASTType::ArgumentList);
                    args << src;
                    dest << args;
                }
            ),
            discard(rule::OrderedLiteral<QChar, QChar>(")")),
            optional(ws),
            proxyAlternative<QChar, ASTNode>(
                statement,
                expression
            )
        ),
        [](Result<ASTNode>& dest, Result<ASTNode>& src) {
            ASTNode funcNode(ASTType::Function, src[0].value());
            ++src;
            while (src) {
                funcNode << *src++;
            }
            dest << funcNode;
        }
    );

    return proxySequence<QChar, ASTNode>(
        proxyAlternative<QChar, ASTNode>(
            functionDeclaration,
            statement,
            expression
        ),
        rule::end<QChar, ASTNode>()
    );
}

class LLVMBuilder
{
    llvm::LLVMContext& context;
    llvm::Module* module;
    llvm::IRBuilder<> builder;
    llvm::ExecutionEngine* engine;
    llvm::FunctionPassManager* fpm;

    QHash<QString, llvm::Value*> scope;

public:
    LLVMBuilder(
            llvm::LLVMContext& context,
            llvm::Module* module,
            llvm::ExecutionEngine* engine,
            llvm::FunctionPassManager* fpm) :
        context(context),
        module(module),
        builder(context),
        engine(engine),
        fpm(fpm)
    {
    }

    llvm::Value* build(const ASTNode& node);
    std::function<double()> buildRunnable(const ASTNode& node);
};

std::function<double()> LLVMBuilder::buildRunnable(const ASTNode& node)
{
    switch (node.type()) {
        case ASTType::Function:
        case ASTType::Unknown:
            return nullptr;
        default:
            // Some top-level expression that we should evaluate, so continue
            break;
    }

    // Synthesize an anonymous function to invoke directly
    ASTNode evalNode(ASTType::Function);
    evalNode << ASTNode(ASTType::ArgumentList);
    evalNode << node;

    auto built = build(evalNode);
    built->dump();

    auto func = static_cast<llvm::Function*>(built);

    void *compiledFunc = engine->getPointerToFunction(func);
    return (double(*)())((intptr_t)compiledFunc);
}

llvm::Value* LLVMBuilder::build(const ASTNode& node)
{
    using namespace llvm;
    switch (node.type()) {
        case ASTType::NumberLiteral:
            return ConstantFP::get(context, APFloat(node.value().toDouble()));
        case ASTType::Add:
            return builder.CreateFAdd(
                build(node[0]),
                build(node[1]),
                "addtmp"
            );
        case ASTType::Subtract:
            return builder.CreateFAdd(
                build(node[0]),
                build(node[1]),
                "subtmp"
            );
        case ASTType::Multiply:
            return builder.CreateFMul(
                build(node[0]),
                build(node[1]),
                "multmp"
            );
        case ASTType::Divide:
            return builder.CreateFDiv(
                build(node[0]),
                build(node[1]),
                "addtmp"
            );
        case ASTType::Variable:
        {
            auto resolved = scope[node.value()];
            if (!resolved) {
                std::stringstream str;
                str << "The variable name '" << node.value() << "' must be defined before use";
                throw std::runtime_error(str.str());
            }
            return resolved;
        }
        case ASTType::Function:
        {
            assert(node.size() == 3);
            auto name = node.value();
            auto args = node[0];

            auto doubleType = llvm::Type::getDoubleTy(context);
            llvm::FunctionType* funcType = FunctionType::get(
                doubleType,
                std::vector<llvm::Type*>(args.size(), doubleType),
                false // No varargs
            );
            auto func = llvm::Function::Create(
                funcType,
                llvm::Function::ExternalLinkage,
                name.toUtf8().constData(),
                module
            );

            scope.clear();

            // Set names for all arguments.
            unsigned int i = 0;
            for (auto iter = func->arg_begin(); i != args.size(); ++iter, ++i) {
                auto argName = args[i].value();
                iter->setName(argName.toUtf8().constData());
                scope[argName] = iter;
            }

            auto block = llvm::BasicBlock::Create(context, "entry", func);
            builder.SetInsertPoint(block);
            builder.CreateRet(build(node[1]));
            llvm::verifyFunction(*func);
            fpm->run(*func);

            return func;
        }
        case ASTType::Call:
        {
            llvm::Function* callee = module->getFunction(node.value().toUtf8().constData());
            if (!callee) {
                throw std::runtime_error("Function must be defined or declared before it is invoked");
            }

            std::vector<llvm::Value*> argValues;
            for (auto child : node.children()) {
                argValues.push_back(build(child));
            }

            return builder.CreateCall(callee, argValues, "calltmp");
        }
        case ASTType::If:
        {
            int index = 0;
            auto condition = builder.CreateFCmpONE(
                build(node[index++]),
                llvm::ConstantFP::get(context, llvm::APFloat(0.0)),
                "ifCond"
            );

            auto func = builder.GetInsertBlock()->getParent();

            auto ifThenBB = llvm::BasicBlock::Create(context, "ifThen");
            auto ifElseBB = llvm::BasicBlock::Create(context, "ifElse");
            auto ifMergeBB = llvm::BasicBlock::Create(context, "ifMerge");

            builder.CreateCondBr(condition, ifThenBB, ifElseBB);

            auto buildBlock = [&](llvm::BasicBlock*& block) {
                func->getBasicBlockList().push_back(block);
                builder.SetInsertPoint(block);

                auto blockV = build(node[index++]);
                builder.CreateBr(ifMergeBB);

                block = builder.GetInsertBlock();

                return blockV;
            };
            auto ifThenV = buildBlock(ifThenBB);
            auto ifElseV = buildBlock(ifElseBB);

            func->getBasicBlockList().push_back(ifMergeBB);
            builder.SetInsertPoint(ifMergeBB);

            auto phi = builder.CreatePHI(
                llvm::Type::getDoubleTy(context),
                2,
                "iftmp"
            );
            phi->addIncoming(ifThenV, ifThenBB);
            phi->addIncoming(ifElseV, ifElseBB);
            return phi;
        }
        case ASTType::Unknown:
        default:
            std::stringstream str;
            str << "I don't know how to build a " << node.type() << " rule";
            throw std::runtime_error(str.str());
    }
}

void parseLine(LLVMBuilder& builder, rule::Proxy<QChar, ASTNode> parser, QString& line)
{
    QTextStream lineStream(&line);

    auto cursor = makeCursor<QChar>(&lineStream);
    Result<ASTNode> nodes;
    auto parseSuccessful = parser(cursor, nodes);

    if (parseSuccessful) {
        while (nodes) {
            auto expr = *nodes++;

            std::cout << expr.dump() << std::endl;
            auto runnable = builder.buildRunnable(expr);
            if (runnable) {
                auto rv = runnable();
                std::cout << "Evaluated to " << rv << std::endl;
            } else {
                auto built = builder.build(expr);
                built->dump();
            }
        }
    } else {
        std::cout << "Failed to parse provided line. :(\n";
    }
}

int main()
{
    llvm::InitializeNativeTarget();

    auto module = new llvm::Module("Sprout", llvm::getGlobalContext());

    // Create the JIT.  This takes ownership of the module.
    std::string err;
    auto engine = llvm::EngineBuilder(module).setErrorStr(&err).create();
    if (!engine) {
        std::stringstream str;
        str << "I failed to create the ExecutionEngine. " << err;
        throw std::runtime_error(str.str());
    }

    llvm::FunctionPassManager fpm(module);
    fpm.add(new llvm::DataLayout(*engine->getDataLayout()));
    fpm.add(llvm::createBasicAliasAnalysisPass());
    fpm.add(llvm::createInstructionCombiningPass());
    fpm.add(llvm::createReassociatePass());
    fpm.add(llvm::createGVNPass());
    fpm.add(llvm::createCFGSimplificationPass());
    fpm.doInitialization();

    QTextStream stream(stdin);
    stream.setCodec("UTF-8");

    auto parser = buildParser();
    LLVMBuilder builder(
        llvm::getGlobalContext(),
        module,
        engine,
        &fpm
    );

    QString line;
    while (true) {
        std::cout << "sprout> ";
        line = stream.readLine();
        if (line.isNull()) {
            std::cout << std::endl;
            break;
        }

        parseLine(builder, parser, line);
    }

    return 0;
}

std::ostream& std::operator<<(std::ostream& stream, const sprout::ASTType& type)
{
    stream << names.at(type);
    return stream;
}
