#pragma once
#include <vector>
#include <string>
#include "Token.h"
#include "ErrorManager.h"
#include "SyntaxAnalyzer.h"

class ReportGenerator {
public:
    void generarReporteTokens(const std::vector<Token>& tokens);
    void generarReporteErrores(const std::vector<ErrorInfo>& errores);
    void generarReporteKanban(const NodoArbol* raiz);
    void generarReporteResponsables(const NodoArbol* raiz);
};