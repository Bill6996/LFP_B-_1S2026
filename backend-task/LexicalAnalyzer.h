#pragma once
#include <string>
#include <vector>
#include "Token.h"

class LexicalAnalyzer {
private:
    std::string source;
    int current;
    int line;
    int column;

    // Métodos auxiliares del autómata
    char advance();
    char peek();
    bool isAtEnd();
    void skipWhitespace();

    // Transiciones de estado específicas
    Token scanString();
    Token scanNumberOrDate();
    Token scanKeyword();

public:
    LexicalAnalyzer(std::string source);
    
    // Método principal 
    Token nextToken();
    
    // Método de utilidad para el backend (ejecuta el ciclo)
    std::vector<Token> analyze(); 
};