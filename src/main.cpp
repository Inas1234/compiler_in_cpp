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

    output << "global _start\n_start:\n";

    vector<Token> postfix;
    stack<Token> op_stack;

    for (int i = 0; i < tokens.size(); i++) {
        if (tokens[i].type == TokenType::int_lit) {
            postfix.push_back(tokens[i]);
        } else if (tokens[i].type == TokenType::add || tokens[i].type == TokenType::sub || tokens[i].type == TokenType::mul) {
            while (!op_stack.empty() && precedence(op_stack.top().type) >= precedence(tokens[i].type)) {
                postfix.push_back(op_stack.top());
                op_stack.pop();
            }
            op_stack.push(tokens[i]);
        }
    }

    while (!op_stack.empty()) {
        postfix.push_back(op_stack.top());
        op_stack.pop();
    }

    stack<string> operands;

    for (int i = 0; i < postfix.size(); i++) {
        if (postfix[i].type == TokenType::int_lit) {
            operands.push(postfix[i].value.value());
        } else if (postfix[i].type == TokenType::add || postfix[i].type == TokenType::sub || postfix[i].type == TokenType::mul) {
            string b = operands.top(); operands.pop();
            string a = operands.top(); operands.pop();
            string result_reg = "r" + to_string(i + 8);  

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
