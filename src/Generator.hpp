#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include "Parser.hpp"
#include <unordered_map>
#include <sstream>
#include <assert.h>
#include <stack>

class Generator{
    public:
        Generator(NodeProg node) : m_node(std::move(node)) {

        }

        void gen_expr(const NodeExpr &expr){
            struct ExprVisitor {
                Generator *gen;
                void operator()(const NodeExprIntLit& node) {
                    gen->m_output << "    ; Pushing Integer Literal"<< std::endl;
                    gen->m_output << "    mov rax, " << node.token.value.value() << std::endl;
                    gen->push("rax");
                }
                void operator()(const NodeExprStringLit& node) {
                    gen->m_output << "    ; Pushing String Literal"<< std::endl;
                    gen->m_output << "    lea rax, [" << gen->get_string_label(node.token.value.value()) << "]" << std::endl;
                    gen->push("rax");
                }
                void operator()(const NodeExprIdentifier& node) {
                    gen->m_output << "    ; Pushing Variable"<< std::endl;
                    const auto& var = gen->m_vars.at(node.token.value.value());
                    std::string offset = var.label;
                    gen->m_output << "    mov rax, [" << offset << "]" << std::endl;
                    gen->push("rax");                
                }
                void operator()(const BinaryExprPlus& node){
                    gen->gen_expr(*node.lhs);
                    gen->gen_expr(*node.rhs);
                    gen->m_output << "    ; Adding"<< std::endl;
                    gen->pop("rdi");
                    gen->pop("rax");
                    gen->m_output << "    add rax, rdi" << std::endl;
                    gen->push("rax");
                }
                void operator()(const BinaryExprMinus& node){
                    gen->gen_expr(*node.lhs);
                    gen->gen_expr(*node.rhs);
                    gen->m_output << "    ; Subtracting"<< std::endl;
                    gen->pop("rdi");
                    gen->pop("rax");
                    gen->m_output << "    sub rax, rdi" << std::endl;
                    gen->push("rax");
                }
                void operator()(const BinaryExprMultiply& node){
                    gen->gen_expr(*node.lhs);
                    gen->gen_expr(*node.rhs);
                    gen->m_output << "    ; Multiplying"<< std::endl;
                    gen->pop("rdi");
                    gen->pop("rax");
                    gen->m_output << "    imul rax, rdi" << std::endl;
                    gen->push("rax");
                }
                void operator()(const BinaryExprDivide& node){
                    gen->gen_expr(*node.lhs);
                    gen->gen_expr(*node.rhs);
                    gen->m_output << "    ; Dividing"<< std::endl;
                    gen->pop("rdi");
                    gen->pop("rax");
                    gen->m_output << "    cqo" << std::endl;
                    gen->m_output << "    idiv rdi" << std::endl;
                    gen->push("rax");
                }
                void operator()(const NodeExprIfEqual& node){
                    gen->gen_expr(*node.lhs);
                    gen->gen_expr(*node.rhs);
                    gen->m_output << "    ; Checkign if equal"<< std::endl;
                    gen->pop("rdi");
                    gen->pop("rax");
                    
                    gen->m_output << "    cmp rax, rdi" << std::endl;
                    gen->m_output << "    sete al" << std::endl;
                    gen->push("rax");

                }
                void operator()(const NodeExprIfGreater& node){
                    gen->gen_expr(*node.lhs);
                    gen->gen_expr(*node.rhs);
                    gen->m_output << "    ; Checkign if greater"<< std::endl;
                    gen->pop("rdi");
                    gen->pop("rax");
                    gen->m_output << "    cmp rax, rdi" << std::endl;
                    gen->m_output << "    setg al" << std::endl;
                    gen->push("rax");
                }
                void operator()(const NodeExprIfNotEqual& node){
                    gen->gen_expr(*node.lhs);
                    gen->gen_expr(*node.rhs);
                    gen->m_output << "    ; Checkign if not equal"<< std::endl;
                    gen->pop("rdi");
                    gen->pop("rax");
                    gen->m_output << "    cmp rax, rdi" << std::endl;
                    gen->m_output << "    setne al" << std::endl;
                    gen->push("rax");
                }
                void operator()(const NodeExprIfLesser& node){
                    gen->gen_expr(*node.lhs);
                    gen->gen_expr(*node.rhs);
                    gen->m_output << "    ; Checkign if lesser"<< std::endl;
                    gen->pop("rdi");
                    gen->pop("rax");
                    gen->m_output << "    cmp rax, rdi" << std::endl;
                    gen->m_output << "    setl al" << std::endl;
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
                    gen->m_output << "    ; Declearing variable"<< std::endl;
                    if (gen->m_vars.find(node.identifier.value.value()) != gen->m_vars.end()){
                        std::cout << "Error: Variable already declared" << std::endl;
                        exit(1);
                    }
                    std::string varLabel = "var_" + node.identifier.value.value();
                    gen->m_dataSection << varLabel << ": dq 0" << std::endl; 

                    gen->m_vars.insert({node.identifier.value.value(), Var{gen->stack_size, varLabel}});
                    gen->gen_expr(node.expr);
                    gen->m_output << "    pop rax" << std::endl; // Pop the result into the rax register
                    gen->m_output << "    mov [" << varLabel << "], rax" << std::endl; // Move the result from rax to the variable's location

                }

                void operator()(const NodeStmtAssign& node) {
                    auto it = gen->m_vars.find(node.identifier.value.value());

                    if (it == gen->m_vars.end()) {
                        std::cout << "Error: Variable not declared" << std::endl;
                        exit(1);
                    }

                    Var var = it->second;
                    std::string offset = var.label;
                    gen->gen_expr(node.expr);
                    gen->m_output << "    ; Assigning variable" << std::endl;
                    gen->pop("rax");
                    gen->m_output << "    mov [" << var.label << "], rax" << std::endl; // Storing the value in the data section
                }

                void operator() (const NodeStmtPrint& node) {
                    gen->m_output << "    ; Printing number"<< std::endl;
                    gen->gen_expr(node.expr);
                    gen->pop("rdi");
                    gen->m_output << "    call dump" << std::endl;
                }
                void operator()(const NodeStmtIf& node) {
                    // Label creation for condition handling
                    static int labelCounter = 0;
                    int currentLabel = labelCounter++;

                    std::string trueLabel = "IF_TRUE_" + std::to_string(currentLabel);
                    std::string endLabel = "IF_END_" + std::to_string(currentLabel);

                    gen->gen_expr(node.condition);
                    gen->pop("rax");
                    gen->m_output << "    cmp rax, 1" << std::endl;
                    struct JumpVisitor {
                        Generator *gen;
                        std::string trueLabel;
                        void operator()(const NodeExprIntLit&) {
                            gen->m_output << "    jne " << trueLabel << std::endl;
                        }
                        void operator()(const NodeExprStringLit&) {
                            gen->m_output << "    jne " << trueLabel << std::endl;
                        }
                        void operator()(const NodeExprIdentifier&) {
                            gen->m_output << "    jne " << trueLabel << std::endl;
                        }
                        void operator()(const BinaryExprPlus&) {
                            gen->m_output << "    jne " << trueLabel << std::endl;
                        }
                        void operator()(const BinaryExprMinus&) {
                            gen->m_output << "    jne " << trueLabel << std::endl;
                        }
                        void operator()(const BinaryExprMultiply&) {
                            gen->m_output << "    jne " << trueLabel << std::endl;
                        }
                        void operator()(const BinaryExprDivide&) {
                            gen->m_output << "    jne " << trueLabel << std::endl;
                        }
                        void operator()(const NodeExprIfEqual&) {
                            gen->m_output << "    je " << trueLabel << std::endl;
                        }
                        void operator()(const NodeExprIfGreater&) {
                            gen->m_output << "    je " << trueLabel << std::endl;
                        }
                        void operator()(const NodeExprIfNotEqual&) {
                            gen->m_output << "    je " << trueLabel << std::endl;
                        }
                        void operator()(const NodeExprIfLesser&) {
                            gen->m_output << "    je " << trueLabel << std::endl;
                        }
                    };

                    std::visit(JumpVisitor{gen, trueLabel}, node.condition.node);

                    gen->m_output << "    jmp " << endLabel << std::endl;

                    gen->m_output << trueLabel << ":\n";
                    for (const auto& stmt : node.nodes) {
                        gen->gen_stmt(*stmt);
                    }

                    gen->m_output << endLabel << ":\n";
                }

                void operator()(const NodeStmtPrintStr& node) {
                    gen->gen_expr(node.expr);
                    gen->pop("rdi");         

                    struct ExprTypeVisitor {
                        Generator* gen;
                        void operator()(const NodeExprStringLit& node2) {
                            size_t size = gen->get_string_length(node2.token.value.value());
                            gen->m_output << "    mov rsi, rdi" << std::endl; 
                            gen->m_output << "    mov rax, 1" << std::endl;   
                            gen->m_output << "    mov rdi, 1" << std::endl;  
                            gen->m_output << "    mov rdx, " << size << std::endl; 
                            gen->m_output << "    syscall" << std::endl;
                        }
                        void operator()(const NodeExprIdentifier& node2) {
                            const auto& var = gen->m_vars.at(node2.token.value.value());

                            std::string offset = var.label;

                            // Load the string's address into rsi from its stack location
                            gen->m_output << "    mov rsi, " << offset << std::endl;

                            // Compute the string length at runtime
                            gen->m_output << "    ; Start compute length of string\n";
                            gen->m_output << "    xor rcx, rcx" << std::endl;  // Counter for string length
                            gen->m_output << ".length_loop:\n";
                            gen->m_output << "    mov al, BYTE [rsi + rcx]" << std::endl;
                            gen->m_output << "    test al, al" << std::endl;
                            gen->m_output << "    je .length_done\n";
                            gen->m_output << "    inc rcx\n";
                            gen->m_output << "    jmp .length_loop\n";
                            gen->m_output << ".length_done:\n";
                            gen->m_output << "    ; End compute length of string\n";

                            // Now that rcx contains the length of the string, we can use it for the syscall
                            gen->m_output << "    mov rdx, rcx" << std::endl; // length
                            gen->m_output << "    mov rax, 1" << std::endl;
                            gen->m_output << "    mov rdi, 1" << std::endl;
                            gen->m_output << "    syscall" << std::endl;
                        }
                        void operator()(const NodeExprIntLit&) {
                        }
                        void operator()(const BinaryExprPlus&) {
                        }
                        void operator()(const BinaryExprMinus&) {
                        }
                        void operator()(const BinaryExprMultiply&) {
                        }
                        void operator()(const BinaryExprDivide&) {
                        }
                        void operator()(const NodeExprIfEqual&) {
                        }
                        void operator()(const NodeExprIfGreater&) {
                        }
                        void operator()(const NodeExprIfNotEqual&) {
                        }
                        void operator()(const NodeExprIfLesser&) {
                        }
                    };
                    std::visit(ExprTypeVisitor{gen}, node.expr.node);
                }
                void operator()(const NodeStmtFor& node) {
                    static std::stack<std::unordered_map<std::string, Var>> varsStack;
                    static int globalLabelCounter = 0;

                    // Get a new unique label for this loop
                    int currentLabel = globalLabelCounter++;

                    std::string startLabel = "FOR_START_" + std::to_string(currentLabel);
                    std::string endLabel = "FOR_END_" + std::to_string(currentLabel);

                    // Push the current state of m_vars onto the stack, marking the start of a new scope
                    varsStack.push(gen->m_vars);

                    // Handle the initialization part of the for loop
                    gen->gen_expr(node.initialization);

                    gen->m_output << startLabel << ":\n";

                    // Evaluate the condition and check if we should exit the loop
                    gen->gen_expr(node.condition);
                    gen->pop("rax");
                    gen->m_output << "    cmp rax, 0" << std::endl;  
                    gen->m_output << "    je " << endLabel << std::endl;

                    // Generate code for the statements inside the for loop
                    for (const auto& stmt : node.nodes) {
                        gen->gen_stmt(*stmt);
                    }

                    // Handle the iteration part of the for loop
                    gen->gen_stmt(*node.iteration);

                    // Jump back to the start to check the condition again
                    gen->m_output << "    jmp " << startLabel << std::endl;

                    gen->m_output << endLabel << ":\n";

                    // Pop the top of the vars stack, returning to the previous scope
                    gen->m_vars = varsStack.top();
                    varsStack.pop();
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
            m_output << "    mov     rdx, -3689348814741910323\n";
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

            m_output << gen_data_segment();
            m_output << m_dataSection.str() << std::endl;

            return m_output.str();

        }


    private:
        std::unordered_map<std::string, std::vector<std::string>> string_literals;
        void push(const std::string& reg) {
            m_output << "    push " << reg << std::endl;
            stack_size++;
        }
        void pop(const std::string& reg) {
            m_output << "    pop " << reg << std::endl;
            stack_size--;
        }
        std::string get_string_label(const std::string& value) {
            std::string label = "str_" + std::to_string(string_literals.size()) + "_" + std::to_string(value.size());

            // If the value isn't in the map yet, create an entry for it.
            if(string_literals.find(value) == string_literals.end()) {
                string_literals[value] = std::vector<std::string>();
            }

            // Now add the label to the list of labels for that string value.
            string_literals[value].push_back(label);
            
            std::cout << "Storing string: " << value << " with label: " << label << std::endl;
            return label;
        }

        std::vector<std::string> get_all_labels_for_string(const std::string& value) {
            return string_literals[value];
        }


        std::string gen_data_segment() {
            std::stringstream ss;
            ss << "segment .data\n";
            for (const auto& pair : string_literals) {
                for (const auto& label : pair.second) {
                    ss << label << ": db \"" << pair.first << "\", 0" << std::endl;
                }
            }
            return ss.str();
        }
        
        size_t get_string_length(const std::string& value) {
            auto it = string_literals.find(value);
            if(it != string_literals.end()) {
                return it->first.size(); // this will return the size of the actual string
            }
            std::cout << "String not found in map!" << std::endl;
            return 0;  // If the string doesn't exist, return 0.
        }


        bool is_string(const NodeExpr& value) {
            struct ExprVisitor {
                bool is_string = false;
                void operator()(const NodeExprIntLit&) {
                }
                void operator()(const NodeExprStringLit&) {
                    is_string = true;
                }
                void operator()(const NodeExprIdentifier&) {
                }
                void operator()(const BinaryExprPlus&) {
                }
                void operator()(const BinaryExprMinus&) {
                }
                void operator()(const BinaryExprMultiply&) {
                }
                void operator()(const BinaryExprDivide&) {
                }
                void operator()(const NodeExprIfEqual&) {
                }
                void operator()(const NodeExprIfGreater&) {
                }
                void operator()(const NodeExprIfNotEqual&) {
                }
                void operator()(const NodeExprIfLesser&) {
                }
            };
            ExprVisitor visitor;
            std::visit(visitor, value.node);
            return visitor.is_string;
        }

        struct Var {
            size_t stack_loc;
            std::string label;
        };

        size_t stack_size = 0;
        NodeProg m_node;
        std::stringstream m_output;
        std::stringstream m_dataSection;

        std::unordered_map<std::string, Var> m_vars {}; 
        std::stack<std::unordered_map<std::string, std::string>> varScopeStack;
};