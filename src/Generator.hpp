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
                    gen->m_output << "    mov rax, " << node.token.value.value() << std::endl;
                    gen->push("rax");
                }
                void operator()(const NodeExprIdentifier& node) {
                    auto& var = gen->m_vars.at(node.token.value.value());
                    std::stringstream offset;
                    offset << "QWORD [rsp + " << (gen->stack_size - var.stack_loc - 1) * 8 << "]";
                    gen->push(offset.str());
                }
                void operator()(const BinaryExprPlus& node){
                    gen->gen_expr(*node.lhs);
                    gen->gen_expr(*node.rhs);
                    gen->pop("rdi");
                    gen->pop("rax");
                    gen->m_output << "    add rax, rdi" << std::endl;
                    gen->push("rax");
                }
                void operator()(const BinaryExprMinus& node){
                    gen->gen_expr(*node.lhs);
                    gen->gen_expr(*node.rhs);
                    gen->pop("rdi");
                    gen->pop("rax");
                    gen->m_output << "    sub rax, rdi" << std::endl;
                    gen->push("rax");
                }
                void operator()(const BinaryExprMultiply& node){
                    gen->gen_expr(*node.lhs);
                    gen->gen_expr(*node.rhs);
                    gen->pop("rdi");
                    gen->pop("rax");
                    gen->m_output << "    imul rax, rdi" << std::endl;
                    gen->push("rax");
                }
                void operator()(const BinaryExprDivide& node){
                    gen->gen_expr(*node.lhs);
                    gen->gen_expr(*node.rhs);
                    gen->pop("rdi");
                    gen->pop("rax");
                    gen->m_output << "    cqo" << std::endl;
                    gen->m_output << "    idiv rdi" << std::endl;
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

                void operator() (const NodeStmtLet& node){
                    gen->m_vars.insert({node.identifier.value.value(), Var{gen->stack_size}});
                    gen->gen_expr(node.expr);
                }

                void operator() (const NodeStmtPrint& node) {
                    gen->gen_expr(node.expr);
                    gen->pop("rdi");
                    gen->m_output << "    call dump" << std::endl;
                }
            };
            StmtVisitor visitor {.gen = this};
            std::visit(visitor, stmt.node);
        }

        [[nodiscard]] std::string gen_prog(){
            m_output << "segment .text\n";
            m_output << "dump:\n";
            m_output << "    push    rbp\n";
            m_output << "    mov     rbp, rsp\n";
            m_output << "    sub     rsp, 64\n";
            m_output << "    mov     QWORD  [rbp-56], rdi\n";
            m_output << "    mov     QWORD  [rbp-8], 1\n";
            m_output << "    mov     eax, 32\n";
            m_output << "    sub     rax, QWORD  [rbp-8]\n";
            m_output << "    mov     BYTE  [rbp-48+rax], 10\n";
            m_output << ".L2:\n";
            m_output << "    mov     rcx, QWORD  [rbp-56]\n";
            m_output << "    mov  rdx, -3689348814741910323\n";
            m_output << "    mov     rax, rcx\n";
            m_output << "    mul     rdx\n";
            m_output << "    shr     rdx, 3\n";
            m_output << "    mov     rax, rdx\n";
            m_output << "    sal     rax, 2\n";
            m_output << "    add     rax, rdx\n";
            m_output << "    add     rax, rax\n";
            m_output << "    sub     rcx, rax\n";
            m_output << "    mov     rdx, rcx\n";
            m_output << "    mov     eax, edx\n";
            m_output << "    lea     edx, [rax+48]\n";
            m_output << "    mov     eax, 31\n";
            m_output << "    sub     rax, QWORD  [rbp-8]\n";
            m_output << "    mov     BYTE  [rbp-48+rax], dl\n";
            m_output << "    add     QWORD  [rbp-8], 1\n";
            m_output << "    mov     rax, QWORD  [rbp-56]\n";
            m_output << "    mov  rdx, -3689348814741910323\n";
            m_output << "    mul     rdx\n";
            m_output << "    mov     rax, rdx\n";
            m_output << "    shr     rax, 3\n";
            m_output << "    mov     QWORD  [rbp-56], rax\n";
            m_output << "    cmp     QWORD  [rbp-56], 0\n";
            m_output << "    jne     .L2\n";
            m_output << "    mov     eax, 32\n";
            m_output << "    sub     rax, QWORD  [rbp-8]\n";
            m_output << "    lea     rdx, [rbp-48]\n";
            m_output << "    lea     rcx, [rdx+rax]\n";
            m_output << "    mov     rax, QWORD  [rbp-8]\n";
            m_output << "    mov     rdx, rax\n";
            m_output << "    mov     rsi, rcx\n";
            m_output << "    mov     edi, 1\n";
            m_output << "    mov     rax, 1\n";
            m_output << "    syscall\n";
            m_output << "    nop\n";
            m_output << "    leave\n";
            m_output << "    ret\n";



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