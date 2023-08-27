#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include "Tokenizer.hpp"

int main(int argc, char** argv) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <input file>" << std::endl;
        return 1;
    }

    std::string output;
    {
        std::ifstream input(argv[1]);
        std::stringstream buffer;
        buffer << input.rdbuf();
        output = buffer.str();
    }

    Tokenize tokenizer(output);
    std::vector<Token> tokens = tokenizer.tokenize();

    for (Token token : tokens) {
        std::cout << (int)token.type << " " << token.value << std::endl;
    }


    return 0;
}