#pragma once
#include <vector>
#include <string>
#include "Token.h"
#include "ErrorManager.h"

// Estructura de arbol por punteros 
struct NodoArbol {
    std::string etiqueta;
    bool esTerminal;
    std::vector<NodoArbol*> hijos;

    NodoArbol(std::string e, bool term) : etiqueta(std::move(e)), esTerminal(term) {}
    ~NodoArbol() { for (auto* h : hijos) delete h; }
};

class SyntaxAnalyzer {
private:
    std::vector<Token> tokens;
    int current;
    ErrorManager& errorManager;

    Token peek() const;
    Token previous() const;
    bool isAtEnd() const;
    bool check(TokenType type) const;
    Token advance();
    bool match(TokenType type);
    
    // Si falla, lanza excepcion
    Token consume(TokenType type, std::string message);
    NodoArbol* hoja(const Token& t);

    // Reglas de la GLC retornando punteros
    NodoArbol* programa();
    NodoArbol* columnas();
    NodoArbol* columna();
    NodoArbol* tareas();
    NodoArbol* tarea();
    NodoArbol* atributos();
    NodoArbol* atributo();
    NodoArbol* prioridad();

public:
    SyntaxAnalyzer(const std::vector<Token>& tokens, ErrorManager& errorManager);
    NodoArbol* parse();
};