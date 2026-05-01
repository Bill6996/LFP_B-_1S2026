#pragma once
#include <string>

// Enumeración de todos los tipos posibles en TaskScript
enum class TokenType {
    // Palabras Reservadas
    KW_TABLERO,
    KW_COLUMNA,
    KW_TAREA,
    KW_PRIORIDAD,
    KW_RESPONSABLE,
    KW_FECHA_LIMITE,

    // Enumeraciones de Prioridad
    VAL_ALTA,
    VAL_MEDIA,
    VAL_BAJA,

    // Literales
    LIT_CADENA,
    LIT_ENTERO,
    LIT_FECHA,

    //  Símbolos
    LLAVE_ABRE,       // {
    LLAVE_CIERRA,     // }
    CORCHETE_ABRE,    // [
    CORCHETE_CIERRA,  // ]
    DOS_PUNTOS,       // :
    COMA,             // ,
    PUNTO_COMA,       // ;

    // Tokens de Control
    FIN_ARCHIVO,      // EOF
    ERROR_LEXICO      // Disparador de parada de emergencia
};

class Token {
private:
    TokenType type;
    std::string lexeme;
    int line;
    int column;

public:
    // Constructor
    Token(TokenType type, std::string lexeme, int line, int column);

    // Getters para acceder a la información
    TokenType getType() const;
    std::string getLexeme() const;
    int getLine() const;
    int getColumn() const;
    
    // Método de utilidad para facilitar la tabla en el frontend
    std::string getTypeName() const; 
};