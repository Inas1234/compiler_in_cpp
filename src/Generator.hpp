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
                    std::cout << "IntLit: " << node.token.value.value() << std::endl;
                    gen->m_output << "    mov rax, " << node.token.value.value() << std::endl;
                    gen->push("rax");
                }
                void operator()(const NodeExprIdentifier& node) {
                    std::cout << "Identifier: " << node.token.value.value() << std::endl;
                    auto& var = gen->m_vars.at(node.token.value.value());
                    std::stringstream offset;
                    offset << "QWORD [rsp + " << (gen->stack_size - var.stack_loc - 1) * 8 << "]";
                    gen->push(offset.str());
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

                void operator() (const NodeStmtLet& node){
                    gen->m_vars.insert({node.identifier.value.value(), Var{gen->stack_size}});
                    gen->gen_expr(node.expr);
                }
            };
            StmtVisitor visitor {.gen = this};
            std::visit(visitor, stmt.node);
        }

        [[nodiscard]] std::string gen_prog(){
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

        struct Var {
            size_t stack_loc;
        };

        size_t stack_size = 0;
        NodeProg m_node;
        std::stringstream m_output;

        std::unordered_map<std::string, Var> m_vars {};
};