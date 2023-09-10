#pragma once
#include <iostream>
#include <vector>
#include <string>
#include "Tokenizer.hpp"
#include <optional>
#include <variant>
#include <memory>


struct NodeExprIntLit {
    Token token;
};

struct NodeExprIdentifier {
    Token token;
};

struct NodeExprStringLit {
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

struct NodeExpr {
    std::variant<NodeExprIntLit, NodeExprIdentifier, BinaryExprPlus, 
    BinaryExprMinus, BinaryExprMultiply, BinaryExprDivide, 
    NodeExprIfEqual, NodeExprIfGreater, NodeExprIfNotEqual,
    NodeExprIfLesser> node;
};

struct NodeStmtExit
{
    NodeExpr expr;
};

struct NodeStmtPrint {
    NodeExpr expr;
};

struct NodeStmtLet {
    Token identifier;
    NodeExpr expr;
};

struct NodeStmt;

struct NodeStmtIf {
    NodeExpr condition;
    std::vector<std::shared_ptr<NodeStmt>> nodes;
};

struct NodeStmt {
    std::variant<NodeStmtExit, NodeStmtLet, NodeStmtPrint, NodeStmtIf> node;
};

struct NodeProg {
    std::vector<NodeStmt> nodes;    
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

                return NodeStmt{.node = node_stmt};
            }
            else {
                return {};
            }

        }

        [[nodiscard]] std::optional<NodeProg> parse_prog(){
            NodeProg node_prog;

            while (peak().has_value()) {
                if (auto node = parse_stmt()) {
                    node_prog.nodes.push_back(node.value());
                }
                else {
                    std::cout << "Error: Invalid syntax" << std::endl;
                    exit(1);
                }
            }

            return node_prog;
        }
        [[nodiscard]] std::optional<NodeExpr> parsePrimaryExpr() {
            NodeExpr node;

            if (peak().has_value() && peak().value().type == Tokentype::INTLIT) {
                node = NodeExpr{.node = NodeExprIntLit{.token = consume()}};    
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
