#pragma once
#include <iostream>
#include <vector>
#include <string>
#include "Tokenizer.hpp"
#include <optional>
#include <variant>
#include <memory>
#include <fstream>

struct NodeExprIntLit {
    Token token;
};

struct NodeExprStringLit {
    Token token;
};

struct NodeExprIdentifier {
    Token token;
};

struct NodeExpr;

struct BinaryExprPlus {
    Token token;
    std::shared_ptr<NodeExpr> lhs;
    std::shared_ptr<NodeExpr> rhs;
};

struct BinaryExprMinus {
    Token token;
    std::shared_ptr<NodeExpr> lhs;
    std::shared_ptr<NodeExpr> rhs;
};

struct BinaryExprMultiply {
    Token token;
    std::shared_ptr<NodeExpr> lhs;
    std::shared_ptr<NodeExpr> rhs;
};

struct BinaryExprDivide {
    Token token;
    std::shared_ptr<NodeExpr> lhs;
    std::shared_ptr<NodeExpr> rhs;
};

struct NodeExprIfEqual {
    Token token;
    std::shared_ptr<NodeExpr> lhs;
    std::shared_ptr<NodeExpr> rhs;
};

struct NodeExprIfNotEqual {
    Token token;
    std::shared_ptr<NodeExpr> lhs;
    std::shared_ptr<NodeExpr> rhs;
};

struct NodeExprIfGreater {
    Token token;
    std::shared_ptr<NodeExpr> lhs;
    std::shared_ptr<NodeExpr> rhs;
};

struct NodeExprIfLesser {
    Token token;
    std::shared_ptr<NodeExpr> lhs;
    std::shared_ptr<NodeExpr> rhs;
};

struct NodeExprIfGreaterEqual {
    Token token;
    std::shared_ptr<NodeExpr> lhs;
    std::shared_ptr<NodeExpr> rhs;
};

struct NodeExprIfLesserEqual {
    Token token;
    std::shared_ptr<NodeExpr> lhs;
    std::shared_ptr<NodeExpr> rhs;
};


struct NodeExpr {
    std::variant<NodeExprIntLit, NodeExprIdentifier, BinaryExprPlus, 
    BinaryExprMinus, BinaryExprMultiply, BinaryExprDivide, 
    NodeExprIfEqual, NodeExprIfGreater, NodeExprIfNotEqual,
    NodeExprIfLesser, NodeExprStringLit, NodeExprIfGreaterEqual,
    NodeExprIfLesserEqual> node;
};

struct NodeStmtExit
{
    NodeExpr expr;
};

struct NodeStmtPrint {
    NodeExpr expr;
};

struct NodeStmtPrintStr {
    NodeExpr expr;
};

struct NodeStmtPrintLn {
    NodeExpr expr;
};


struct NodeStmtLet {
    Token identifier;
    NodeExpr expr;
};

struct NodeStmt;

struct NodeStmtElse {
    std::vector<std::shared_ptr<NodeStmt>> nodes;
};
struct NodeStmtIf {
    NodeExpr condition;
    std::vector<std::shared_ptr<NodeStmt>> nodes;
    std::shared_ptr<NodeStmtElse> else_node;       // Optional 'else' block.
};



struct NodeStmtFor {
    NodeExpr initialization;
    NodeExpr condition;
    std::shared_ptr<NodeStmt> iteration;
    std::vector<std::shared_ptr<NodeStmt>> nodes;
};


struct NodeStmtAssign{
    Token identifier;
    NodeExpr expr;
};

struct NodeStmtFunction {
    Token functionName;
    std::vector<Token> parameters;
    std::vector<std::shared_ptr<NodeStmt>> body;
};


struct NodeFunctions
{
    NodeStmtFunction function;
};


struct NodeStmtFunctionCall {
    Token functionName;
    std::vector<NodeExpr> arguments;
};


struct NodeStmt {
    std::variant<NodeStmtExit, NodeStmtLet, NodeStmtPrint, NodeStmtIf, NodeStmtPrintStr, NodeStmtFor, NodeStmtAssign, NodeStmtPrintLn, NodeStmtFunctionCall> node;
};

struct NodeProg {
    std::vector<NodeStmt> main;
    std::vector<NodeStmtFunction> functions;
};

class Parser {
    public:
        Parser(std::vector<Token> tokens) {
            m_tokens = tokens;
        }

        [[nodiscard]] std::optional<NodeExpr> parse_expr(){
            return parseBinOpExpr(0);
        }

        [[nodiscard]] std::optional<NodeExpr> parseBinOpExpr(int prec){
            std::optional<NodeExpr> left = parsePrimaryExpr();
            if (!left.has_value()){
                return {};
            }
            
            while (true) {
                if (!peak().has_value()) {
                    return left;
                }
                
                std::optional<int> nextPrec = bin_precidance(peak().value().type);
                if (!nextPrec.has_value() || nextPrec.value() < prec) {
                    return left;
                }
                
                Tokentype opType = peak().value().type;
                Token token = consume();
                std::optional<NodeExpr> right = parseBinOpExpr(nextPrec.value() + 1);
                if (!right.has_value()){
                    return {};
                }

                switch (opType) {
                    case Tokentype::IFEQUAL:
                        left = NodeExpr{.node = NodeExprIfEqual{.token = token, .lhs = std::make_shared<NodeExpr>(left.value()), .rhs = std::make_shared<NodeExpr>(right.value())}};
                        break;
                    case Tokentype::IFNOTEQUAL:
                        left = NodeExpr{.node = NodeExprIfNotEqual{.token = token, .lhs = std::make_shared<NodeExpr>(left.value()), .rhs = std::make_shared<NodeExpr>(right.value())}};
                        break;
                    case Tokentype::IFGREATER:
                        left = NodeExpr{.node = NodeExprIfGreater{.token = token, .lhs = std::make_shared<NodeExpr>(left.value()), .rhs = std::make_shared<NodeExpr>(right.value())}};
                        break;
                    case Tokentype::IFLESSER:
                        left = NodeExpr{.node = NodeExprIfLesser{.token = token, .lhs = std::make_shared<NodeExpr>(left.value()), .rhs = std::make_shared<NodeExpr>(right.value())}};
                        break;
                    case Tokentype::IFEQUALGREATER:
                        left = NodeExpr{.node = NodeExprIfGreaterEqual{.token = token, .lhs = std::make_shared<NodeExpr>(left.value()), .rhs = std::make_shared<NodeExpr>(right.value())}};
                        break;
                    case Tokentype::IFEQUALLESSER:  
                        left = NodeExpr{.node = NodeExprIfLesserEqual{.token = token, .lhs = std::make_shared<NodeExpr>(left.value()), .rhs = std::make_shared<NodeExpr>(right.value())}};  
                        break;
                    case Tokentype::PLUS:
                        left = NodeExpr{.node = BinaryExprPlus{.token = token, .lhs = std::make_shared<NodeExpr>(left.value()), .rhs = std::make_shared<NodeExpr>(right.value())}};
                        break;
                    case Tokentype::MINUS:
                        left = NodeExpr{.node = BinaryExprMinus{.token = token, .lhs = std::make_shared<NodeExpr>(left.value()), .rhs = std::make_shared<NodeExpr>(right.value())}};
                        break;
                    case Tokentype::STAR:
                        left = NodeExpr{.node = BinaryExprMultiply{.token = token, .lhs = std::make_shared<NodeExpr>(left.value()), .rhs = std::make_shared<NodeExpr>(right.value())}};
                        break;
                    case Tokentype::DIVIDE:
                        left = NodeExpr{.node = BinaryExprDivide{.token = token, .lhs = std::make_shared<NodeExpr>(left.value()), .rhs = std::make_shared<NodeExpr>(right.value())}};
                        break;
                    default:
                        return {};  // error
                }
            }
        }



        [[nodiscard]] std::optional<NodeStmt> parse_stmt(){
            if (peak().value().type == Tokentype::EXIT && peak(1).value().type == Tokentype::OPENPAREN) {
                consume();
                consume();
                NodeStmtExit node_stmt;
                if (auto node = parse_expr()) {
                    node_stmt = NodeStmtExit({node.value()});
                }
                else {
                    std::cout << "Error: Invalid syntax exit" << std::endl;
                    exit(1);
                }

                if (peak().value().type == Tokentype::CLOSEPAREN) {
                    consume();
                }
                else {
                    std::cout << "Error: Invalid syntax exit" << std::endl;
                    exit(1);
                }

                if (peak().value().type == Tokentype::SEMI) {
                    consume();
                }
                else {
                    std::cout << "Error: Invalid syntax exit" << std::endl;
                    exit(1);
                }
                return NodeStmt{.node = node_stmt};
            }
            else if (peak().value().type == Tokentype::LET && peak(1).value().type == Tokentype::IDENTIFIER && peak(2).value().type == Tokentype::EQUALS) {
                consume();
                auto stmt_let = NodeStmtLet{.identifier = consume()};
                consume();
                if (auto node = parse_expr()) {
                    stmt_let.expr = node.value();
                }
                else {
                    std::cout << "Error: Invalid syntax let" << std::endl;
                    exit(1);
                }

                if (peak().value().type == Tokentype::SEMI) {
                    consume();
                }
                else {
                    std::cout << "Error: Invalid syntax let" << std::endl;
                    exit(1);
                }

                return NodeStmt{.node = stmt_let};
            }
            else if (peak().value().type == Tokentype::PRINT && peak(1).value().type == Tokentype::OPENPAREN){
                consume();
                consume();

                NodeStmtPrint node_stmt;
                if (auto node = parse_expr()) {
                    node_stmt = NodeStmtPrint({node.value()});
                }
                else {
                    std::cout << "Error: Invalid syntax print()" << std::endl;
                    exit(1);
                }
                if (peak().value().type == Tokentype::CLOSEPAREN) {
                    consume();
                }
                else {
                    std::cout << "Error: Invalid syntax print()" << std::endl;
                    exit(1);
                }

                if (peak().value().type == Tokentype::SEMI) {
                    consume();
                }
                else {
                    std::cout << "Error: Invalid syntax print()" << std::endl;
                    exit(1);
                }

                return NodeStmt{.node = node_stmt};
            }
            else if (peak().value().type == Tokentype::IF && peak(1).value().type == Tokentype::OPENPAREN) {
                consume();
                consume();

                NodeStmtIf node_stmt;
                if (auto node = parse_expr()) {
                    node_stmt = NodeStmtIf({node.value()});
                }
                else {
                    std::cout << "Error: Invalid syntax if()" << std::endl;
                    exit(1);
                }

                if (peak().value().type == Tokentype::CLOSEPAREN) {
                    consume();
                }
                else {
                    std::cout << "Error: Invalid syntax if()" << std::endl;
                    exit(1);
                }

                if (peak().value().type == Tokentype::BRACKET_OPEN) {
                    consume();
                }
                else {
                    std::cout << "Error: Invalid syntax if()" << std::endl;
                    exit(1);
                }

                while (peak().has_value() && peak().value().type != Tokentype::BRACKET_CLOSE) {
                    if (auto node = parse_stmt()) {
                        node_stmt.nodes.push_back(std::make_shared<NodeStmt>(node.value()));
                    }
                    else {
                        std::cout << "Error: Invalid syntax if()" << std::endl;
                        exit(1);
                    }
                }

                if (peak().value().type == Tokentype::BRACKET_CLOSE) {
                    consume();
                }
                else {
                    std::cout << "Error: Invalid syntax if()" << std::endl;
                    exit(1);
                }

                if (peak().value().type == Tokentype::ELSE) {
                    consume(); // Consume the ELSE token

                    NodeStmtElse node_else;

                    // Check for a block of statements for the 'else' part
                    if (peak().value().type != Tokentype::BRACKET_OPEN) {
                        std::cout << "Error: Invalid syntax else" << std::endl;
                        exit(1);
                    }
                    consume(); // Consume the opening bracket

                    while (peak().has_value() && peak().value().type != Tokentype::BRACKET_CLOSE) {
                        if (auto node = parse_stmt()) {
                            node_else.nodes.push_back(std::make_shared<NodeStmt>(node.value()));
                        }
                        else {
                            std::cout << "Error: Invalid syntax else" << std::endl;
                            exit(1);
                        }
                    }

                    if (peak().value().type != Tokentype::BRACKET_CLOSE) {
                        std::cout << "Error: Invalid syntax else" << std::endl;
                        exit(1);
                    }
                    consume(); // Consume the closing bracket

                    node_stmt.else_node = std::make_shared<NodeStmtElse>(node_else); 
                }

                return NodeStmt{.node = node_stmt};
            }
            else if (peak().value().type == Tokentype::PRINTSTR && peak(1).value().type == Tokentype::OPENPAREN){
                consume();
                consume();

                NodeStmtPrintStr node_stmt;
                if (auto node = parse_expr()) {
                    node_stmt = NodeStmtPrintStr({node.value()});
                }
                else {
                    std::cout << "Error: Invalid syntax print()" << std::endl;
                    exit(1);
                }
                if (peak().value().type == Tokentype::CLOSEPAREN) {
                    consume();
                }
                else {
                    std::cout << "Error: Invalid syntax print()" << std::endl;
                    exit(1);
                }

                if (peak().value().type == Tokentype::SEMI) {
                    consume();
                }
                else {
                    std::cout << "Error: Invalid syntax print()" << std::endl;
                    exit(1);
                }

                return NodeStmt{.node = node_stmt};
            }
            else if (peak().value().type == Tokentype::PRINTLN){
                consume();

                NodeStmtPrintLn node_stmt;

                if (peak().value().type == Tokentype::SEMI) {
                    consume();
                }
                else {
                    std::cout << "Error: Invalid syntax println missing ;" << std::endl;
                    exit(1);
                }

                return NodeStmt{.node = node_stmt};
            }
            else if (peak().value().type == Tokentype::FOR && peak(1).value().type == Tokentype::OPENPAREN) {
                consume();
                consume();

                NodeStmtFor node_stmt;
                if (auto node = parse_expr()) {  // Assuming the initialization is an expression for simplicity
                    node_stmt.initialization = node.value();
                }
                else {
                    std::cout << "Error: Invalid syntax in for loop initialization" << std::endl;
                    exit(1);
                }

                if (peak().value().type != Tokentype::SEMI) {
                    std::cout << "Error: Expected ; after initialization in for loop" << std::endl;
                    exit(1);
                }
                consume();

                if (auto node = parse_expr()) {  // The loop condition
                    node_stmt.condition = node.value();
                }
                else {
                    std::cout << "Error: Invalid syntax in for loop condition" << std::endl;
                    exit(1);
                }

                if (peak().value().type != Tokentype::SEMI) {
                    std::cout << "Error: Expected ; after condition in for loop" << std::endl;
                    exit(1);
                }
                consume();

                if (auto node = parse_stmt()) {  // The iteration step
                    node_stmt.iteration =  std::make_shared<NodeStmt>(node.value());
                }
                else {
                    std::cout << "Error: Invalid syntax in for loop iteration " << std::endl;
                    exit(1);
                }

                if (peak().value().type != Tokentype::CLOSEPAREN) {
                    std::cout << "Error: Expected ) after for loop header" << std::endl;
                    exit(1);
                }
                consume();

                if (peak().value().type == Tokentype::BRACKET_OPEN) {
                    consume();
                }
                else {
                    std::cout << "Error: Invalid syntax in for loop body" << std::endl;
                    exit(1);
                }

                while (peak().has_value() && peak().value().type != Tokentype::BRACKET_CLOSE) {
                    if (auto node = parse_stmt()) {
                        node_stmt.nodes.push_back(std::make_shared<NodeStmt>(node.value()));
                    }
                    else {
                        std::cout << "Error: Invalid syntax in for loop body" << std::endl;
                        exit(1);
                    }
                }

                if (peak().value().type == Tokentype::BRACKET_CLOSE) {
                    consume();
                }
                else {
                    std::cout << "Error: Expected } after for loop body" << std::endl;
                    exit(1);
                }

                return NodeStmt{.node = node_stmt};
            }
            else if (peak().value().type == Tokentype::IDENTIFIER && peak(1).value().type == Tokentype::EQUALS) {
                auto stmt_assign = NodeStmtAssign{.identifier = consume()};
                consume();
                if (auto node = parse_expr()) {
                    stmt_assign.expr = node.value();
                }
                else {
                    std::cout << "Error: Invalid syntax in assignment" << std::endl;
                    exit(1);
                }

                if (peak().value().type == Tokentype::SEMI) {
                    consume();
                }
                else {
                    std::cout << "Error: Expected ; after assignment" << std::endl;
                    exit(1);
                }

                return NodeStmt{.node = stmt_assign};
            }
            else if (peak().value().type == Tokentype::IDENTIFIER && peak(1).value().type == Tokentype::OPENPAREN) {
                NodeStmtFunctionCall node_stmt;
                node_stmt.functionName = consume();  // Store the function name
                consume();  // Consume the open parenthesis

                // Parse the function arguments
                while (peak().value().type != Tokentype::CLOSEPAREN) {
                    if (auto arg = parse_expr()) {
                        node_stmt.arguments.push_back(arg.value());
                    } else {
                        std::cout << "Error: Invalid syntax in function call argument" << std::endl;
                        exit(1);
                    }

                    // If there's a comma, we expect another argument
                    if (peak().value().type == Tokentype::COMMA) {
                        consume();  // Consume the comma
                    } else if (peak().value().type != Tokentype::CLOSEPAREN) {
                        std::cout << "Error: Expected , or ) in function call" << std::endl;
                        exit(1);
                    }
                }

                consume();  // Consume the close parenthesis

                if (peak().value().type != Tokentype::SEMI) {
                    std::cout << "Error: Expected ; after function call" << std::endl;
                    exit(1);
                }
                consume();  // Consume the semicolon

                return NodeStmt{.node = node_stmt};
            }

            else {
                return {};
            }

        }

        [[nodiscard]] std::optional<NodeFunctions> parse_func_def() {
            if (peak().value().type == Tokentype::FUNCTION && peak(1).value().type == Tokentype::IDENTIFIER && peak(2).value().type == Tokentype::OPENPAREN) {
                NodeStmtFunction func_def;

                // Consume "fn" keyword
                consume();

                // Get function name
                func_def.functionName = consume();

                // Consume open parenthesis
                consume();

                // Handle arguments
                while (peak().has_value() && peak().value().type != Tokentype::CLOSEPAREN) {
                    if (peak().value().type == Tokentype::LET) {
                        // Consume 'let' keyword
                        consume();

                        if (peak().value().type == Tokentype::IDENTIFIER) {
                            // Append argument to function definition
                            func_def.parameters.push_back(consume());

                            if (peak().value().type == Tokentype::COMMA) {
                                consume(); // Consume comma
                            } else if (peak().value().type != Tokentype::CLOSEPAREN) {
                                std::cout << "Error: Expected ',' or ')' in function arguments list." << std::endl;
                                exit(1);
                            }
                        } else {
                            std::cout << "Error: Expected argument identifier after 'let' in function definition." << std::endl;
                            exit(1);
                        }
                    } else {
                        std::cout << "Error: Expected 'let' keyword in function parameter." << std::endl;
                        exit(1);
                    }
                }

                if (peak().value().type != Tokentype::CLOSEPAREN) {
                    std::cout << "Error: Expected ')' in function definition." << std::endl;
                    exit(1);
                }

                // Consume close parenthesis
                consume();

                // Handle the body of the function
                if (peak().value().type != Tokentype::BRACKET_OPEN) {
                    std::cout << "Error: Expected '{' at start of function body." << std::endl;
                    exit(1);
                }

                // Consume open bracket
                consume();

                // Parse statements inside function body
                while (peak().has_value() && peak().value().type != Tokentype::BRACKET_CLOSE) {
                    if (auto node = parse_stmt()) {
                        func_def.body.push_back(std::make_shared<NodeStmt>(node.value()));
                    } else {
                        std::cout << "Error: Invalid statement in function body." << std::endl;
                        exit(1);
                    }
                }

                if (peak().value().type != Tokentype::BRACKET_CLOSE) {
                    std::cout << "Error: Expected '}' at end of function body." << std::endl;
                    exit(1);
                }

                // Consume close bracket
                consume();

                return NodeFunctions{.function = func_def};
            }
            return {};
        }



        [[nodiscard]] std::optional<NodeProg> parse_prog(){
            NodeProg node_prog;

            while (peak().has_value()) {
                if (peak().value().type == Tokentype::INCLUDE){
                    handleInclude();
                }
                else if (auto node = parse_func_def()) {
                    node_prog.functions.push_back(node.value().function);
                }
                else if (auto node = parse_stmt()) {
                    node_prog.main.push_back(node.value());
                }
                else {
                    std::cout << "Error: Invalid syntax" << std::endl;
                    exit(1);
                }
            }

            return node_prog;
        }

        void handleInclude(){
            consume();
            Token token;
            if (peak().value().type == Tokentype::STRINGLIT) {
                token = consume();
            }
            else {
                std::cout << "Error: Invalid syntax expected string" << std::endl;
                exit(1);
            }
            std::optional<std::string> filename = token.value;
            if (!filename.has_value()) {
                std::cout << "Error: Invalid syntax expected file path" << std::endl;
                exit(1);
            }
            std::string output;
            {
                std::ifstream input("./src/include/" + filename.value());
                if (!input.is_open()) {
                    std::cout << "Error: Could not open file" << std::endl;
                    exit(1);
                }
                std::stringstream buffer;
                buffer << input.rdbuf();
                output = buffer.str();
            }
            Tokenize tokenizer(output);
            std::vector<Token> tokens = tokenizer.tokenize();
            m_tokens.insert(m_tokens.begin() + m_index, tokens.begin(), tokens.end());
        }

        [[nodiscard]] std::optional<NodeExpr> parsePrimaryExpr() {
            NodeExpr node;

            if (peak().has_value() && peak().value().type == Tokentype::INTLIT) {
                node = NodeExpr{.node = NodeExprIntLit{.token = consume()}};    
                return node;
            }
            else if (peak().has_value() && peak().value().type == Tokentype::STRINGLIT) {
                node = NodeExpr{.node = NodeExprStringLit{.token = consume()}};
                return node;
            }
            else if (peak().has_value() && peak().value().type == Tokentype::IDENTIFIER) {
                node = NodeExpr{.node = NodeExprIdentifier{.token = consume()}};
                return node;
            }
            else {
                return {};
            }
        }

    private:
        std::vector<Token> m_tokens;
        int m_index = 0;
        std::optional<Token> peak(int ahead = 0){
            if (m_index + ahead >= m_tokens.size()){
                return {};
            }
            return m_tokens[m_index + ahead];
        }
        
        Token consume(){
            return m_tokens[m_index++];
        }
};
