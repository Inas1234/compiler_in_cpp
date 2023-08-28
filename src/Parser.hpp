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

struct NodeExpr {
    std::variant<NodeExprIntLit> node;
};


struct NodeExit {
    NodeExpr expr;
};

struct NodeStmtExit
{
    NodeExpr expr;
};


struct NodeStmt {
    std::variant<NodeStmtExit> node;
};

struct NodeProg {
    std::vector<NodeStmt> nodes;    
};

class Parser {
    public:
        Parser(std::vector<Token> tokens) {
            m_tokens = tokens;
        }

        std::optional<NodeStmt> parse_stmt(){
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
                return NodeStmt{.node = node_stmt};
            }
        }

        std::optional<NodeProg> parse_prog(){
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
        std::optional<NodeExpr> parseExpr() {
            NodeExpr node;

            if (peak().has_value() && peak().value().type == Tokentype::INTLIT) {
                node = NodeExpr{.node = NodeExprIntLit{.token = consume()}};    
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
