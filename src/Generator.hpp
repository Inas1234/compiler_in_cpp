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
                    if (gen->m_vars.find(node.token.value.value()) == gen->m_vars.end()){
                        std::string varLabel = "var_" + node.token.value.value();
                        std::cout << "Error: Variable not declared " << varLabel << std::endl;
                        exit(1);
                    }
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
                void operator()(const NodeExprIfGreaterEqual& node){
                    gen->gen_expr(*node.lhs);
                    gen->gen_expr(*node.rhs);
                    gen->m_output << "    ; Checkign if greater or equal"<< std::endl;
                    gen->pop("rdi");
                    gen->pop("rax");
                    gen->m_output << "    cmp rax, rdi" << std::endl;
                    gen->m_output << "    setge al" << std::endl;
                    gen->push("rax");
                }
                void operator()(const NodeExprIfLesserEqual& node){
                    gen->gen_expr(*node.lhs);
                    gen->gen_expr(*node.rhs);
                    gen->m_output << "    ; Checkign if lesser or equal"<< std::endl;
                    gen->pop("rdi");
                    gen->pop("rax");
                    gen->m_output << "    cmp rax, rdi" << std::endl;
                    gen->m_output << "    setle al" << std::endl;
                    gen->push("rax");
                }
                void operator()(const NodeExprFunctionCall& node) {
                    std::vector<std::string> regs = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};

                    // Push arguments onto the stack or move to registers
                    int numArgs = node.arguments.size();

                    if (numArgs > 6) {
                        for (int i = numArgs - 1; i >= 6; --i) {
                            gen->gen_expr(*node.arguments[i]);
                            gen->m_output << "    push rax" << std::endl;
                        }
                    }

                    for (int i = 0; i < std::min(numArgs, 6); ++i) {
                        gen->gen_expr(*node.arguments[i]);
                        gen->m_output << "    mov " << regs[i] << ", rax" << std::endl;
                    }

                    // Ensure the stack is 16-byte aligned before the call. If the number of arguments pushed onto 
                    // the stack is odd, push an extra value to align the stack.
                    if ((numArgs - 6) % 2 == 1) {
                        gen->m_output << "    sub rsp, 8 ; Aligning stack to 16 bytes" << std::endl;
                    }

                    // Call function
                    gen->m_output << "    call " << node.functionName.value.value() << std::endl;

                    // Cleanup if arguments were pushed onto the stack. 
                    // Also consider the potential extra value pushed for stack alignment.
                    if (numArgs > 6) {
                        int stack_adjust = 8 * (numArgs - 6);
                        if ((numArgs - 6) % 2 == 1) {
                            stack_adjust += 8;
                        }
                        gen->m_output << "    add rsp, " << stack_adjust << std::endl;
                    }

                    // Push the result of the function call onto the stack
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
                        std::cout << "Error: Variable already declared " << std::endl;
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
                void operator()(const NodeStmtPrintLn& node){
                    gen->m_output << "    ; Printing newline" << std::endl;
                    gen->m_output << "    mov rax, 1" << std::endl;  // syscall number for sys_write
                    gen->m_output << "    mov rdi, 1" << std::endl;  // file descriptor: STDOUT
                    gen->m_output << "    mov rsi, newline_str" << std::endl; // address of the newline string
                    gen->m_output << "    mov rdx, 1" << std::endl;  // number of bytes
                    gen->m_output << "    syscall" << std::endl;

                }
                void operator()(const NodeStmtIf& node) {
                    // Label creation for condition handling
                    static int labelCounter = 0;
                    int currentLabel = labelCounter++;

                    std::string trueLabel = "IF_TRUE_" + std::to_string(currentLabel);
                    std::string falseLabel = "IF_FALSE_" + std::to_string(currentLabel);
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
                        void operator()(const NodeExprIfGreaterEqual&) {
                            gen->m_output << "    je " << trueLabel << std::endl;
                        }
                        void operator()(const NodeExprIfLesserEqual&) {
                            gen->m_output << "    je " << trueLabel << std::endl;
                        }
                        void operator()(const NodeExprFunctionCall&) {
                            gen->m_output << "    je " << trueLabel << std::endl;
                        }
                    };

                    std::visit(JumpVisitor{gen, trueLabel}, node.condition.node);

                    gen->m_output << "    jmp " << falseLabel << std::endl;  // Go to the ELSE block if condition is not met.

                    gen->m_output << trueLabel << ":\n";
                    for (const auto& stmt : node.nodes) {
                        gen->gen_stmt(*stmt);
                    }

                    gen->m_output << "    jmp " << endLabel << std::endl;  // Jump to end after executing IF block

                    gen->m_output << falseLabel << ":\n";
                    if (node.else_node) {  // If there's an ELSE block, generate its code.
                        for (const auto& stmt : node.else_node->nodes) {
                            gen->gen_stmt(*stmt);
                        }
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
                            gen->m_output << "    mov rsi, [" << offset <<"]" << std::endl;

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
                        void operator()(const NodeExprIfGreaterEqual&) {
                        }
                        void operator()(const NodeExprIfLesserEqual&) {
                        }
                        void operator()(const NodeExprFunctionCall& node) {
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

                    gen->gen_expr(node.condition);
                    gen->pop("rax");
                    gen->m_output << "    test al, al" << std::endl;                    
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

                void operator()(const NodeStmtFunctionCall& node){
                    std::vector<std::string> regs = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};

                    // Push arguments onto the stack or move to registers
                    for (int i = node.arguments.size() - 1; i >= 0; i--) {
                        if (i >= 6) {
                            // If more than 6 arguments, push onto the stack
                            // For simplicity, assume every argument generates a value in rax (modify as per your design)
                            gen->gen_expr(node.arguments[i]);
                            gen->m_output << "    push rax" << std::endl;
                        }
                    }

                    for (size_t i = 0; i < std::min(node.arguments.size(), size_t(6)); i++) {
                        // For the first 6 arguments, move to the appropriate registers
                         gen->gen_expr(node.arguments[i]);
                         gen->m_output << "    mov " << regs[i] << ", rax" << std::endl; // Assuming rax holds the result
                    }

                    // Call function
                     gen->m_output << "    call " << node.functionName.value.value() << std::endl;

                    // Cleanup if arguments were pushed onto the stack
                    if (node.arguments.size() > 6) {
                        int stack_adjust = 8 * (node.arguments.size() - 6);
                         gen->m_output << "    add rsp, " << stack_adjust << std::endl;
                    }                
                }

            };
            StmtVisitor visitor {.gen = this};
            std::visit(visitor, stmt.node);
        }


        void gen_function(const NodeFunctions& node) {
            struct FunctionVisitor {
                Generator *gen;
                void operator()(const NodeStmtFunction& node){
                    gen->m_output << "    ; Function declaration for " << node.functionName.value.value() << std::endl;
                    gen->m_output << "    global " << node.functionName.value.value() << std::endl;
                    gen->m_output << node.functionName.value.value() << ":" << std::endl;

                    // Function prologue
                    gen->m_output << "    push rbp" << std::endl;
                    gen->m_output << "    mov rbp, rsp" << std::endl;

                    // Allocate space and save arguments
                    std::vector<std::string> regs = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
                    int stack_offset = 0; // Used if you have more than 6 parameters

                    for (size_t i = 0; i < node.parameters.size(); i++) {
                        std::string paramName = node.parameters[i].value.value(); // extract the name of the parameter
                        if (i < 6) {
                            gen->m_output << "    push " << regs[i] << std::endl;
                            gen->m_vars.emplace(paramName, Var{gen->stack_size, "rbp - " + std::to_string((i+1) * 8)});
                        } else {
                            stack_offset += 8;
                            gen->m_output << "    mov rax, [rbp + " << stack_offset << "]" << std::endl; // Move next argument from stack to rax
                            gen->m_output << "    push rax" << std::endl;
                            gen->m_vars.emplace(paramName, Var{gen->stack_size, "rbp - " + std::to_string(8 * (i + 7))});
                        }
                    }

                    // Generate the code for the body of the function
                    for (const auto& stmt : node.body) {
                        gen->gen_stmt(*stmt);
                    }

                    // Function epilogue
                    gen->m_output << "    mov rsp, rbp" << std::endl;
                    gen->m_output << "    pop rbp" << std::endl;
                    gen->m_output << "    ret" << std::endl;
                }
                void operator()(const NodeStmtFunctionReturn& node) {
                    gen->m_output << "    ; Function declaration with return for " << node.functionName.value.value() << std::endl;
                    gen->m_output << "    global " << node.functionName.value.value() << std::endl;
                    gen->m_output << node.functionName.value.value() << ":" << std::endl;

                    // Function prologue
                    gen->m_output << "    push rbp" << std::endl;
                    gen->m_output << "    mov rbp, rsp" << std::endl;

                    // Saving arguments
                    std::vector<std::string> regs = {"rdi", "rsi", "rdx", "rcx", "r8", "r9"};
                    
                    for (size_t i = 0; i < std::min(node.parameters.size(), size_t(6)); i++) {
                        std::string paramName = node.parameters[i].value.value();
                        gen->m_output << "    push " << regs[i] << std::endl;
                        gen->m_vars.emplace(paramName, Var{gen->stack_size, "rbp - " + std::to_string((i+1) * 8)});
                    }

                    // Process function body
                    for (const auto& stmt : node.body) {
                        gen->gen_stmt(*stmt);
                    }

                    // Handle return expression
                    if(node.returnExpr.has_value()) {
                        gen->gen_expr(node.returnExpr.value());
                    }

                    // Function epilogue
                    gen->m_output << "    leave" << std::endl;  // Recommended over the combination of `mov rsp, rbp` and `pop rbp`
                    gen->m_output << "    ret" << std::endl;
                }

            };

            std::visit(FunctionVisitor{this}, node.function);

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
            m_output << std::endl;


            m_output << "global _start\n_start:\n";
            for (auto stmt : m_node.main){
                gen_stmt(stmt);
            }

            m_output << "    mov rax, 60\n";
            m_output << "    mov rdi, 0\n";
            m_output << "    syscall\n";


            for (const auto& pair : m_node.functions) {
                gen_function(pair);
            }


            m_output << gen_data_segment();
            m_output << m_dataSection.str() << std::endl;
            m_output << "newline_str: db 10" << std::endl;
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
                void operator()(const NodeExprIfGreaterEqual&) {
                }
                void operator()(const NodeExprIfLesserEqual&) {
                }
                void operator()(const NodeExprFunctionCall& node) {
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