#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include "Tokenizer.hpp"
#include "Parser.hpp"
#include "Generator.hpp"

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
    Parser parser(tokens);
    std::optional<NodeProg> node = parser.parse_prog();
    if (!node.has_value()){
        std::cout << "Error: Invalid syntax" << std::endl;
        exit(1);
    }
    
    Generator generator(node.value());
    std::string assembly = generator.gen_prog();
    {
        std::ofstream output("out.asm");
        output << assembly;
    }

    system("nasm -felf64 out.asm -o out.o");
    system("ld out.o -o out");

    return 0;
}