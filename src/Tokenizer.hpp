#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <optional>

enum class Tokentype{
    EXIT,
    INTLIT,
    OPENPAREN,
    CLOSEPAREN,
    LET,
    IDENTIFIER,
    EQUALS,
    SEMI,
    PRINT,
    PLUS,
    MINUS,
    STAR,
    DIVIDE,
    IFEQUAL,
    IFNOTEQUAL,
    IFGREATER,
    IFLESSER,
    IF,
    THEN,
    ELSE,
    BRACKET_OPEN,
    BRACKET_CLOSE,
    STRINGLIT,
    PRINTSTR
};

struct Token{
    Tokentype type;
    std::optional<std::string> value;
};


std::optional<int> bin_precidance(Tokentype type){
    switch (type)
    {
    case Tokentype::IFNOTEQUAL:
    case Tokentype::IFLESSER:
    case Tokentype::IFEQUAL:
    case Tokentype::IFGREATER:
    case Tokentype::MINUS:
    case Tokentype::PLUS:
        return 0;
    case Tokentype::DIVIDE:
    case Tokentype::STAR:
        return 1;
    default:
        return {};
    }
}


class Tokenize{
    public:
        Tokenize(std::string str){
            m_string = str;
        }


        std::vector<Token> tokenize(){
            std::vector<Token> tokens;
            std::string buffer = "";
            while (peak().has_value())
            {
                if (isalpha(peak().value())){
                    buffer.push_back(consume());
                    while (isalnum(peak().value())){
                        buffer.push_back(consume());
                    }
                    if (buffer == "exit"){
                        tokens.push_back({Tokentype::EXIT});
                        buffer.clear();
                        continue;
                    }
                    else if (buffer == "let"){
                        tokens.push_back({Tokentype::LET});
                        buffer.clear();
                        continue;
                    }
                    else if (buffer == "if"){
                        tokens.push_back({Tokentype::IF});
                        buffer.clear();
                        continue;
                    }
                    else if (buffer == "then"){
                        tokens.push_back({Tokentype::THEN});
                        buffer.clear();
                        continue;
                    }
                    else if (buffer == "else"){
                        tokens.push_back({Tokentype::ELSE});
                        buffer.clear();
                        continue;
                    }
                    else if (buffer == "print"){
                        tokens.push_back({Tokentype::PRINT});
                        buffer.clear();
                        continue;
                    }
                    else if (buffer == "prints"){
                        tokens.push_back({Tokentype::PRINTSTR});
                        buffer.clear();
                        continue;
                    }
                    else{
                        tokens.push_back({Tokentype::IDENTIFIER, buffer});
                        buffer.clear();
                        continue;
                    }

                }
                else if (isdigit(peak().value())){
                    buffer.push_back(consume());
                    while (isdigit(peak().value())){
                        buffer.push_back(consume());
                    }
                    tokens.push_back({Tokentype::INTLIT, buffer});
                    buffer.clear();
                    continue;
                }
                else if (peak().value() == '"'){
                    consume();
                    while (peak().value() != '"'){
                        buffer.push_back(consume());
                    }
                    consume();
                    tokens.push_back({Tokentype::STRINGLIT, buffer});
                    buffer.clear();
                    continue;
                }
                else if (peak().value() == '?' && peak(1).value() == '='){
                    consume();
                    consume();
                    tokens.push_back({Tokentype::IFEQUAL});
                    continue;
                }
                else if (peak().value() == '!' && peak(1).value() == '='){
                    consume();
                    consume();
                    tokens.push_back({Tokentype::IFNOTEQUAL});
                    continue;
                }
                else if (peak().value() == '>'){
                    consume();
                    tokens.push_back({Tokentype::IFGREATER});
                    continue;
                }
                else if (peak().value() == '<'){
                    consume();
                    tokens.push_back({Tokentype::IFLESSER});
                    continue;
                }
                else if (peak().value() == '('){
                    consume();
                    tokens.push_back({Tokentype::OPENPAREN});
                    continue;
                }
                else if (peak().value() == ')'){
                    consume();
                    tokens.push_back({Tokentype::CLOSEPAREN});
                    continue;
                }
                else if (peak().value() == '='){
                    consume();
                    tokens.push_back({Tokentype::EQUALS});
                    continue;
                }
                else if (peak().value() == ';'){
                    consume();
                    tokens.push_back({Tokentype::SEMI});
                    continue;   
                }
                else if (peak().value() == '+'){
                    consume();
                    tokens.push_back({Tokentype::PLUS});
                    continue;
                }
                else if (peak().value() == '-'){
                    consume();
                    tokens.push_back({Tokentype::MINUS});
                    continue;
                }
                else if (peak().value() == '*'){
                    consume();
                    tokens.push_back({Tokentype::STAR});
                    continue;
                }
                else if (peak().value() == '/'){
                    consume();
                    tokens.push_back({Tokentype::DIVIDE});
                    continue;
                }
                else if (peak().value() == '{'){
                    consume();
                    tokens.push_back({Tokentype::BRACKET_OPEN});
                    continue;
                }
                else if (peak().value() == '}'){
                    consume();
                    tokens.push_back({Tokentype::BRACKET_CLOSE});
                    continue;
                }
                else if (isspace(peak().value())){
                    consume();
                    continue;
                }
                else{
                    std::cout << "Error: Invalid character" << std::endl;
                    exit(1);
                }
            }

            m_index = 0;

            return tokens;
        }
    private:
        std::string m_string;
        int m_index = 0;

        std::optional<char> peak(int ahead = 0){
            if (m_index + ahead >= m_string.length()) {
                return {};
            }
            else {
                return m_string.at(m_index + ahead);
            }
        }

        char consume(){
            return m_string.at(m_index++);
        }

};