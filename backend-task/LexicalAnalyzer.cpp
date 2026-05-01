#include "LexicalAnalyzer.h"
#include <cctype>

LexicalAnalyzer::LexicalAnalyzer(std::string source) {
    this->source = source;
    this->current = 0;
    this->line = 1;
    this->column = 1;
}

bool LexicalAnalyzer::isAtEnd() { return current >= source.length(); }

char LexicalAnalyzer::advance() {
    column++;
    return source[current++];
}

char LexicalAnalyzer::peek() {
    if (isAtEnd()) return '\0';
    return source[current];
}

void LexicalAnalyzer::skipWhitespace() {
    while (!isAtEnd()) {
        char c = peek();
        if (c == ' ' || c == '\r' || c == '\t') {
            advance();
        } else if (c == '\n') {
            line++;
            column = 0;
            advance();
        } else {
            break;
        }
    }
}

Token LexicalAnalyzer::scanString() {
    int startCol = column - 1;
    std::string lexeme = "\"";
    bool contieneCaracterInvalido = false;

    while (peek() != '"' && !isAtEnd()) {
        char c = peek();
        if (c == '\n') {
            return Token(TokenType::ERROR_LEXICO, lexeme, line, startCol);
        }

        unsigned char uc = static_cast<unsigned char>(c);
        if (!std::isalnum(uc) && uc < 128 && c != ' ' && c != '-' && c != '_' && c != '.' && c != ',' && c != ':') {
            contieneCaracterInvalido = true;
        }

        lexeme += advance();
    }

    if (isAtEnd()) {
        return Token(TokenType::ERROR_LEXICO, lexeme, line, startCol);
    }
    lexeme += advance(); // Consumir comilla de cierre

    if (contieneCaracterInvalido) {
        return Token(TokenType::ERROR_LEXICO, lexeme, line, startCol);
    }
    return Token(TokenType::LIT_CADENA, lexeme, line, startCol);
}

Token LexicalAnalyzer::scanNumberOrDate() {
    int startCol = column;
    std::string lexeme = "";
    while (std::isdigit(peek())) {
        lexeme += advance();
    }
    if (peek() == '-') {
        lexeme += advance();
        while (std::isdigit(peek())) lexeme += advance();
        if (peek() == '-') {
            lexeme += advance();
            while (std::isdigit(peek())) lexeme += advance();
            if (lexeme.length() == 10) {
                std::string textoMes = lexeme.substr(5, 2);
                std::string textoDia = lexeme.substr(8, 2);
                int mes = std::stoi(textoMes);
                int dia = std::stoi(textoDia);
                bool todoBien = true;
                if (mes < 1 || mes > 12) todoBien = false;
                if (dia < 1 || dia > 31) todoBien = false;
                if (mes == 2 && dia > 29) todoBien = false;
                if ((mes == 4 || mes == 6 || mes == 9 || mes == 11) && dia > 30) todoBien = false;
                
                if (todoBien) return Token(TokenType::LIT_FECHA, lexeme, line, startCol);
                else return Token(TokenType::ERROR_LEXICO, lexeme, line, startCol);
            }
        }
        return Token(TokenType::ERROR_LEXICO, lexeme, line, startCol);
    }
    return Token(TokenType::LIT_ENTERO, lexeme, line, startCol);
}

Token LexicalAnalyzer::scanKeyword() {
    int startCol = column;
    std::string lexeme = "";
    while (std::isalpha(peek()) || peek() == '_') lexeme += advance();
    
    if (lexeme == "TABLERO") return Token(TokenType::KW_TABLERO, lexeme, line, startCol);
    if (lexeme == "COLUMNA") return Token(TokenType::KW_COLUMNA, lexeme, line, startCol);
    if (lexeme == "tarea") return Token(TokenType::KW_TAREA, lexeme, line, startCol);
    if (lexeme == "prioridad") return Token(TokenType::KW_PRIORIDAD, lexeme, line, startCol);
    if (lexeme == "responsable") return Token(TokenType::KW_RESPONSABLE, lexeme, line, startCol);
    if (lexeme == "fecha_limite") return Token(TokenType::KW_FECHA_LIMITE, lexeme, line, startCol);
    if (lexeme == "ALTA") return Token(TokenType::VAL_ALTA, lexeme, line, startCol);
    if (lexeme == "MEDIA") return Token(TokenType::VAL_MEDIA, lexeme, line, startCol);
    if (lexeme == "BAJA") return Token(TokenType::VAL_BAJA, lexeme, line, startCol);
    return Token(TokenType::ERROR_LEXICO, lexeme, line, startCol);
}

Token LexicalAnalyzer::nextToken() {
    skipWhitespace();
    if (isAtEnd()) return Token(TokenType::FIN_ARCHIVO, "EOF", line, column);
    int startCol = column;
    char c = advance();
    switch (c) {
        case '{': return Token(TokenType::LLAVE_ABRE, "{", line, startCol);
        case '}': return Token(TokenType::LLAVE_CIERRA, "}", line, startCol);
        case '[': return Token(TokenType::CORCHETE_ABRE, "[", line, startCol);
        case ']': return Token(TokenType::CORCHETE_CIERRA, "]", line, startCol);
        case ':': return Token(TokenType::DOS_PUNTOS, ":", line, startCol);
        case ',': return Token(TokenType::COMA, ",", line, startCol);
        case ';': return Token(TokenType::PUNTO_COMA, ";", line, startCol);
        case '"': return scanString();
    }
    if (std::isalpha(c)) { current--; column--; return scanKeyword(); }
    if (std::isdigit(c)) { current--; column--; return scanNumberOrDate(); }
    
    std::string unknown(1, c);
    return Token(TokenType::ERROR_LEXICO, unknown, line, startCol);
}

std::vector<Token> LexicalAnalyzer::analyze() {
    std::vector<Token> tokens;
    Token t = nextToken();
    while (t.getType() != TokenType::FIN_ARCHIVO) {
        tokens.push_back(t);
        t = nextToken();
    }
    tokens.push_back(t);
    return tokens;
}