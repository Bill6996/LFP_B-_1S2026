#ifndef LEXICAL_ANALYZER_H
#define LEXICAL_ANALYZER_H

#include <string>
#include <vector>
#include <map>
#include "../model/Token.h"
#include "../errors/ErrorManager.h"

class LexicalAnalyzer {
private:
    std::string input;
    int pos;
    int line;
    int column;

    char peek();
    char advance();
    void skipWhitespace();
    TokenType checkKeyword(const std::string& lexeme);

public:
    std::vector<Token> tokens;
    std::vector<LexicalError> errors;

    LexicalAnalyzer(std::string text);
    void analyze();
};

#endif