#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include "Parser.hpp"
#include <unordered_map>
#include <sstream>


class Generator{
    public:
        Generator(NodeProg node) : m_node(std::move(node)) {

        }

        void gen_expr(const NodeExpr &expr){
            struct ExprVisitor {
                Generator *gen;
                void operator()(const NodeExprIntLit& node) {
                    gen->m_output << "    mov rax, " << node.token.value << std::endl;
                    gen->push("rax");
                }
            };
            std::visit(ExprVisitor{this}, expr.node);
        }


        void gen_stmt(const NodeStmt& stmt) {
            struct StmtVisitor {
                Generator *gen;
                void operator()(const NodeStmtExit& node) {
                    gen->gen_expr(node.expr);
                    gen->m_output << "    mov rax, 60" << std::endl;
                    gen->pop("rdi");
                    gen->m_output << "    syscall" << std::endl;
                }
            };
            StmtVisitor visitor {.gen = this};
            std::visit(visitor, stmt.node);
        }

        std::string gen_prog(){
            m_output << "global _start\n_start:\n";
            for (auto stmt : m_node.nodes){
                gen_stmt(stmt);
            }

            m_output << "    mov rax, 60\n";
            m_output << "    mov rdi, 0\n";
            m_output << "    syscall\n";

            return m_output.str();

        }


    private:
        void push(const std::string& reg) {
            m_output << "    push " << reg << std::endl;
            stack_size++;
        }
        void pop(const std::string& reg) {
            m_output << "    pop " << reg << std::endl;
            stack_size--;
        }

        size_t stack_size = 0;
        NodeProg m_node;
        std::stringstream m_output;
};