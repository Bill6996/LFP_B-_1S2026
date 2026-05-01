#include "SyntaxAnalyzer.h"
#include <stdexcept>

SyntaxAnalyzer::SyntaxAnalyzer(const std::vector<Token>& tokens, ErrorManager& errorManager)
    : tokens(tokens), current(0), errorManager(errorManager) {}

Token SyntaxAnalyzer::peek() const {
    if (current >= tokens.size()) return tokens.back();
    return tokens[current];
}

Token SyntaxAnalyzer::previous() const {
    if (current == 0) return tokens[0];
    return tokens[current - 1];
}

bool SyntaxAnalyzer::isAtEnd() const { return peek().getType() == TokenType::FIN_ARCHIVO; }

bool SyntaxAnalyzer::check(TokenType type) const {
    if (isAtEnd()) return false;
    return peek().getType() == type;
}

Token SyntaxAnalyzer::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

bool SyntaxAnalyzer::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

Token SyntaxAnalyzer::consume(TokenType type, std::string message) {
    if (check(type)) return advance();
    Token errToken = peek();
    errorManager.addError(ErrorType::SINTACTICO, errToken.getLexeme(), message, errToken.getLine(), errToken.getColumn());
    throw std::runtime_error("Abortar Sintactico");
}

NodoArbol* SyntaxAnalyzer::hoja(const Token& t) {
    return new NodoArbol(t.getLexeme(), true);
}

NodoArbol* SyntaxAnalyzer::parse() {
    try {
        if (!isAtEnd()) {
            return programa();
        }
    } catch (const std::runtime_error& error) {
        return nullptr;
    }
    return nullptr;
}

NodoArbol* SyntaxAnalyzer::programa() {
    NodoArbol* node = new NodoArbol("<programa>", false);
    node->hijos.push_back(hoja(consume(TokenType::KW_TABLERO, "Se esperaba 'TABLERO'.")));
    node->hijos.push_back(hoja(consume(TokenType::LIT_CADENA, "Se esperaba nombre del tablero.")));
    node->hijos.push_back(hoja(consume(TokenType::LLAVE_ABRE, "Se esperaba '{'.")));
    
    if (match(TokenType::KW_COLUMNA)) {
        current--; 
        node->hijos.push_back(columnas());
    } else {
        Token errToken = peek();
        errorManager.addError(ErrorType::SINTACTICO, errToken.getLexeme(), "Se esperaba al menos una COLUMNA.", errToken.getLine(), errToken.getColumn());
        throw std::runtime_error("Abortar Sintactico");
    }
    
    node->hijos.push_back(hoja(consume(TokenType::LLAVE_CIERRA, "Se esperaba '}'.")));
    if (check(TokenType::PUNTO_COMA)) {
        node->hijos.push_back(hoja(consume(TokenType::PUNTO_COMA, "Se esperaba ';' al final.")));
    }
    return node;
}

NodoArbol* SyntaxAnalyzer::columnas() {
    NodoArbol* node = new NodoArbol("<columnas>", false);
    node->hijos.push_back(columna());
    while (check(TokenType::KW_COLUMNA)) {
        node->hijos.push_back(columna());
    }
    return node;
}

NodoArbol* SyntaxAnalyzer::columna() {
    NodoArbol* node = new NodoArbol("<columna>", false);
    node->hijos.push_back(hoja(consume(TokenType::KW_COLUMNA, "Se esperaba 'COLUMNA'.")));
    node->hijos.push_back(hoja(consume(TokenType::LIT_CADENA, "Se esperaba nombre de la columna.")));
    node->hijos.push_back(hoja(consume(TokenType::LLAVE_ABRE, "Se esperaba '{'.")));
    if (!check(TokenType::LLAVE_CIERRA)) {
        node->hijos.push_back(tareas());
    }
    node->hijos.push_back(hoja(consume(TokenType::LLAVE_CIERRA, "Se esperaba '}'.")));
    if (check(TokenType::PUNTO_COMA)) {
        node->hijos.push_back(hoja(consume(TokenType::PUNTO_COMA, "Se esperaba ';'.")));
    }
    return node;
}

NodoArbol* SyntaxAnalyzer::tareas() {
    NodoArbol* node = new NodoArbol("<tareas>", false);
    node->hijos.push_back(tarea());
    while (match(TokenType::COMA)) {
        node->hijos.push_back(new NodoArbol(",", true));
        if (check(TokenType::LLAVE_CIERRA)) break; 
        node->hijos.push_back(tarea());
    }
    return node;
}

NodoArbol* SyntaxAnalyzer::tarea() {
    NodoArbol* node = new NodoArbol("<tarea>", false);
    node->hijos.push_back(hoja(consume(TokenType::KW_TAREA, "Se esperaba 'tarea'.")));
    node->hijos.push_back(hoja(consume(TokenType::DOS_PUNTOS, "Se esperaba ':'.")));
    node->hijos.push_back(hoja(consume(TokenType::LIT_CADENA, "Se esperaba nombre de la tarea.")));
    node->hijos.push_back(hoja(consume(TokenType::CORCHETE_ABRE, "Se esperaba '['.")));
    if (!check(TokenType::CORCHETE_CIERRA)) {
        node->hijos.push_back(atributos());
    }
    node->hijos.push_back(hoja(consume(TokenType::CORCHETE_CIERRA, "Se esperaba ']'.")));
    return node;
}

NodoArbol* SyntaxAnalyzer::atributos() {
    NodoArbol* node = new NodoArbol("<atributos>", false);
    node->hijos.push_back(atributo());
    while (match(TokenType::COMA)) {
        node->hijos.push_back(new NodoArbol(",", true));
        if (check(TokenType::CORCHETE_CIERRA)) break; 
        node->hijos.push_back(atributo());
    }
    return node;
}

NodoArbol* SyntaxAnalyzer::atributo() {
    NodoArbol* node = new NodoArbol("<atributo>", false);
    if (match(TokenType::KW_PRIORIDAD)) {
        node->hijos.push_back(hoja(previous()));
        node->hijos.push_back(hoja(consume(TokenType::DOS_PUNTOS, "Se esperaba ':'.")));
        node->hijos.push_back(prioridad());
    } else if (match(TokenType::KW_RESPONSABLE)) {
        node->hijos.push_back(hoja(previous()));
        node->hijos.push_back(hoja(consume(TokenType::DOS_PUNTOS, "Se esperaba ':'.")));
        node->hijos.push_back(hoja(consume(TokenType::LIT_CADENA, "Se esperaba responsable.")));
    } else if (match(TokenType::KW_FECHA_LIMITE)) {
        node->hijos.push_back(hoja(previous()));
        node->hijos.push_back(hoja(consume(TokenType::DOS_PUNTOS, "Se esperaba ':'.")));
        node->hijos.push_back(hoja(consume(TokenType::LIT_FECHA, "Se esperaba fecha.")));
    } else {
        Token errToken = peek();
        errorManager.addError(ErrorType::SINTACTICO, errToken.getLexeme(), "Se esperaba atributo.", errToken.getLine(), errToken.getColumn());
        throw std::runtime_error("Abortar Sintactico");
    }
    return node;
}

NodoArbol* SyntaxAnalyzer::prioridad() {
    NodoArbol* node = new NodoArbol("<prioridad>", false);
    if (match(TokenType::VAL_ALTA) || match(TokenType::VAL_MEDIA) || match(TokenType::VAL_BAJA)) {
        node->hijos.push_back(hoja(previous()));
    } else {
        Token errToken = peek();
        errorManager.addError(ErrorType::SINTACTICO, errToken.getLexeme(), "Se esperaba ALTA, MEDIA o BAJA.", errToken.getLine(), errToken.getColumn());
        throw std::runtime_error("Abortar Sintactico");
    }
    return node;
}