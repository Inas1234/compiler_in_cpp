#pragma once
#include <iostream>
#include <vector>
#include <string>
#include "Tokenizer.hpp"
#include <optional>
#include <variant>

struct NodeExprIntLit {
    Token token;
};

struct NodeExprIdentifier {
    Token token;
};

struct NodeExpr {
    std::variant<NodeExprIntLit, NodeExprIdentifier> node;
};


struct NodeStmtExit
{
    NodeExpr expr;
};

struct NodeStmtLet {
    Token identifier;
    NodeExpr expr;
};

struct NodeStmt {
    std::variant<NodeStmtExit, NodeStmtLet> node;
};

struct NodeProg {
    std::vector<NodeStmt> nodes;    
};

class Parser {
    public:
        Parser(std::vector<Token> tokens) {
            m_tokens = tokens;
        }

        [[nodiscard]] std::optional<NodeStmt> parse_stmt(){
            if (peak().value().type == Tokentype::EXIT && peak(1).value().type == Tokentype::OPENPAREN) {
                consume();
                consume();
                NodeStmtExit node_stmt;
                if (auto node = parseExpr()) {
                    node_stmt = NodeStmtExit({node.value()});
                }
                else {
                    std::cout << "Error: Invalid syntax" << std::endl;
                    exit(1);
                }

                if (peak().value().type == Tokentype::CLOSEPAREN) {
                    consume();
                }
                else {
                    std::cout << "Error: Invalid syntax" << std::endl;
                    exit(1);
                }

                if (peak().value().type == Tokentype::SEMI) {
                    consume();
                }
                else {
                    std::cout << "Error: Invalid syntax" << std::endl;
                    exit(1);
                }
                return NodeStmt{.node = node_stmt};
            }
            else if (peak().value().type == Tokentype::LET && peak(1).value().type == Tokentype::IDENTIFIER && peak(2).value().type == Tokentype::EQUALS) {
                consume();
                auto stmt_let = NodeStmtLet{.identifier = consume()};
                consume();
                if (auto node = parseExpr()) {
                    stmt_let.expr = node.value();
                }
                else {
                    std::cout << "Error: Invalid syntax" << std::endl;
                    exit(1);
                }

                if (peak().value().type == Tokentype::SEMI) {
                    consume();
                }
                else {
                    std::cout << "Error: Invalid syntax" << std::endl;
                    exit(1);
                }

                return NodeStmt{.node = stmt_let};
            }else {
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
        [[nodiscard]] std::optional<NodeExpr> parseExpr() {
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
