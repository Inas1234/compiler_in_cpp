#include <vector>
#include <string>
#include <iostream>
#include <optional>

enum class Tokentype{
    EXIT,
    INTLIT,
    OPENPAREN,
    CLOSEPAREN,
};

struct Token{
    Tokentype type;
    std::string value;
};


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
                    while (isalpha(peak().value())){
                        buffer.push_back(consume());
                    }
                    if (buffer == "exit"){
                        tokens.push_back({Tokentype::EXIT});
                    }
                    buffer.clear();
                }
                else if (isdigit(peak().value())){
                    buffer.push_back(consume());
                    while (isdigit(peak().value())){
                        buffer.push_back(consume());
                    }
                    tokens.push_back({Tokentype::INTLIT, buffer});
                    buffer.clear();
                }
                else if (peak().value() == '('){
                    tokens.push_back({Tokentype::OPENPAREN});
                    consume();
                }
                else if (peak().value() == ')'){
                    tokens.push_back({Tokentype::CLOSEPAREN});
                    consume();
                }
                else if (isspace(peak().value())){
                    consume();
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