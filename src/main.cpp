#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <optional>
#include "./Tokenizer.hpp"
using namespace std;

string tokens_to_asm(vector<Token>& tokens){
    stringstream output;
    output << "global _start\n_start:\n";
    for (auto token : tokens){
        if (token.type == TokenType::_return){
            output << "    mov rax, 60\n";
        }
        else if (token.type == TokenType::int_lit){
            output << "    mov rdi, " << token.value.value() << "\n";
        }

    }
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
