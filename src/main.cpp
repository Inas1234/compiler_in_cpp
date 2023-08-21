#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <optional>
#include "./Tokenizer.hpp"
#include <stack>
#include <random>
#include <algorithm>
#include <map>
#include <set>
#include <iterator> // Required for std::prev

using namespace std;

int precedence(TokenType op) {
    switch (op) {
        case TokenType::mul:
            return 2;
        case TokenType::add:
        case TokenType::sub:
            return 1;
        default:
            return 0;
    }
}


string tokens_to_asm(vector<Token>& tokens) {
    stringstream output;

    map<string, string> variables;  

    output << "section .data\n";  

    vector<Token> postfix;
    stack<Token> op_stack;
    stack<string> operands;
    set<string> available_registers = {"rax", "rbx", "rcx", "rdx", "rsi", "rdi", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15"};

    for (int i = 0; i < tokens.size(); i++) {
        if (tokens[i].type == TokenType::int_lit) {
            if (tokens[i].value) {
                postfix.push_back(tokens[i]);
            }
        } else if (tokens[i].type == TokenType::add || tokens[i].type == TokenType::sub || tokens[i].type == TokenType::mul) {
            while (!op_stack.empty() && precedence(op_stack.top().type) >= precedence(tokens[i].type)) {
                postfix.push_back(op_stack.top());
                op_stack.pop();
            }
            op_stack.push(tokens[i]);
        } else if (tokens[i].type == TokenType::var_decl) {
            i++;
            if (i < tokens.size() && tokens[i].value) {
                string var_name = tokens[i].value.value();
                variables[var_name] = var_name;
                output << var_name << " dq 0\n";  
            }
        } else if (tokens[i].type == TokenType::var_assign) {
            if (i > 0 && tokens[i-1].value && i+1 < tokens.size() && tokens[i+1].value) {
                string var_name = tokens[i-1].value.value();
                string value = tokens[i+1].value.value();
                output << "    mov qword [" << variables[var_name] << "], " << value << "\n";
                i++; 
            }
        } else if (tokens[i].type == TokenType::var_ref) {
            if (tokens[i].value) {
                operands.push(variables[tokens[i].value.value()]);
            }
        }
    }

    output << "section .text\n"; 
    output << "global _start\n_start:\n";

    while (!op_stack.empty()) {
        postfix.push_back(op_stack.top());
        op_stack.pop();
    }

    for (int i = 0; i < postfix.size(); i++) {
        if (postfix[i].type == TokenType::int_lit) {
            if (postfix[i].value) {
                operands.push(postfix[i].value.value());
            }
        } 
        else if (postfix[i].type == TokenType::add || postfix[i].type == TokenType::sub || postfix[i].type == TokenType::mul) {
            string b = operands.top(); operands.pop();
            string a = operands.top(); operands.pop();
            string result_reg = *available_registers.begin();
            available_registers.erase(available_registers.begin());
            output << "    mov " << result_reg << ", " << a << "\n";
            if (postfix[i].type == TokenType::add) {
                output << "    add " << result_reg << ", " << b << "\n";
            } else if (postfix[i].type == TokenType::sub) {
                output << "    sub " << result_reg << ", " << b << "\n";
            } else if (postfix[i].type == TokenType::mul) {
                output << "    imul " << result_reg << ", " << b << "\n";
            }

            operands.push(result_reg);
        }
    }

    output << "    mov rdi, " << operands.top() << "\n";
    output << "    mov rax, 60\n";
    output << "    syscall\n";

    return output.str();
}


int main (int argc, char *argv[]) {

    string output;
    {
        stringstream ss;
        fstream input(argv[1], ios::in);
        ss << input.rdbuf();
        output = ss.str();
    }
    vector<Token> tokens = Tokenizer::tokenize(output);
    {
        fstream output_file("output.asm", ios::out);
        output_file << tokens_to_asm(tokens);
    }

    system("nasm -felf64 output.asm -o output.o");
    system("ld output.o -o output");

    return 0;
}
