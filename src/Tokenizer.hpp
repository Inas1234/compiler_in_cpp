#ifndef TOKENIZER_H
#define TOKENIZER_H

#include <string>
#include <vector>
#include <optional>
#include <iostream>

enum class TokenType {
    _return,
    int_lit,
    add,
    sub,
    mul,
    var_decl,
    var_ref,
    var_assign,
};

struct Token {
    TokenType type;
    std::optional<std::string> value {};
    std::optional<std::string> var_name {};
};

class Tokenizer {
public:
    static std::vector<Token> tokenize(const std::string& str) {
        std::string buff;
        std::vector<Token> tokens;

        for (size_t i = 0; i < str.length(); ++i) {
            char c = str.at(i);
            if (isalpha(c)) {
                buff.push_back(c);
                while (i + 1 < str.length() && isalpha(str.at(i + 1))) {
                    i++;
                    buff.push_back(str.at(i));
                }
                
                if (i + 1 == str.length() || !isalpha(str.at(i + 1))) {
                if (buff == "exit") {
                    tokens.push_back(Token{TokenType::_return});
                } else if (buff == "var") {
                    tokens.push_back(Token{TokenType::var_decl});
                } else {
                    tokens.push_back(Token{TokenType::var_ref, {}, buff});
                }
                buff.clear();
                }

                buff.clear();
            } else if (isdigit(c)) {
                buff.push_back(c);
                i++;
                while (i < str.length() && isdigit(str.at(i))) {
                    buff.push_back(str.at(i));
                    i++;
                }
                i--;
                tokens.push_back(Token{TokenType::int_lit, buff});
                buff.clear();
            } else if (c == '+') {
                tokens.push_back(Token{TokenType::add});
            } else if (c == '-') {
                tokens.push_back(Token{TokenType::sub});
            } else if (c == '*') {
                tokens.push_back(Token{TokenType::mul});
            } else if (isspace(c)) {
                continue;
            } else if (c == '=') {
                tokens.push_back(Token{TokenType::var_assign});
            }
        }

        return tokens;
    }
};

#endif // TOKENIZER_H
