#include "Token.h"

// Implementación del constructor
Token::Token(TokenType type, std::string lexeme, int line, int column) {
    this->type = type;
    this->lexeme = lexeme;
    this->line = line;
    this->column = column;
}

// Implementación de los getters
TokenType Token::getType() const { return type; }
std::string Token::getLexeme() const { return lexeme; }
int Token::getLine() const { return line; }
int Token::getColumn() const { return column; }

std::string Token::getTypeName() const {
    switch (type) {
        case TokenType::KW_TABLERO: return "KW_TABLERO";
        case TokenType::KW_COLUMNA: return "KW_COLUMNA";
        case TokenType::KW_TAREA: return "KW_TAREA";
        case TokenType::KW_PRIORIDAD: return "KW_PRIORIDAD";
        case TokenType::KW_RESPONSABLE: return "KW_RESPONSABLE";
        case TokenType::KW_FECHA_LIMITE: return "KW_FECHA_LIMITE";
        
        case TokenType::VAL_ALTA: return "VAL_ALTA";
        case TokenType::VAL_MEDIA: return "VAL_MEDIA";
        case TokenType::VAL_BAJA: return "VAL_BAJA";
        
        case TokenType::LIT_CADENA: return "LIT_CADENA";
        case TokenType::LIT_ENTERO: return "LIT_ENTERO";
        case TokenType::LIT_FECHA: return "LIT_FECHA";
        
        case TokenType::LLAVE_ABRE: return "LLAVE_ABRE";
        case TokenType::LLAVE_CIERRA: return "LLAVE_CIERRA";
        case TokenType::CORCHETE_ABRE: return "CORCHETE_ABRE";
        case TokenType::CORCHETE_CIERRA: return "CORCHETE_CIERRA";
        case TokenType::DOS_PUNTOS: return "DOS_PUNTOS";
        case TokenType::COMA: return "COMA";
        case TokenType::PUNTO_COMA: return "PUNTO_COMA";
        
        case TokenType::FIN_ARCHIVO: return "FIN_ARCHIVO";
        case TokenType::ERROR_LEXICO: return "ERROR_LEXICO";
        default: return "DESCONOCIDO";
    }
}