#include "LexicalAnalyzer.h"
#include <cctype>
#include <map>

LexicalAnalyzer::LexicalAnalyzer(std::string text) {
    input  = text;
    pos    = 0;
    line   = 1;
    column = 1;
}

char LexicalAnalyzer::peek() {
    if ((size_t)pos >= input.size()) return '\0';
    return input[pos];
}

char LexicalAnalyzer::advance() {
    char c = input[pos++];
    if (c == '\n') { line++; column = 1; }
    else { column++; }
    return c;
}

TokenType LexicalAnalyzer::checkKeyword(const std::string& lex) {
    static std::map<std::string, TokenType> keywords = {
                                                        {"HOSPITAL",         TokenType::HOSPITAL},
                                                        {"PACIENTES",        TokenType::PACIENTES},
                                                        {"MEDICOS",          TokenType::MEDICOS},
                                                        {"CITAS",            TokenType::CITAS},
                                                        {"DIAGNOSTICOS",     TokenType::DIAGNOSTICOS},
                                                        {"paciente",         TokenType::PACIENTE},
                                                        {"medico",           TokenType::MEDICO},
                                                        {"cita",             TokenType::CITA},
                                                        {"diagnostico",      TokenType::DIAGNOSTICO},
                                                        {"edad",             TokenType::EDAD},
                                                        {"tipo_sangre",      TokenType::TIPO_SANGRE},
                                                        {"habitacion",       TokenType::HABITACION},
                                                        {"especialidad",     TokenType::ESPECIALIDAD},
                                                        {"codigo",           TokenType::CODIGO},
                                                        {"fecha",            TokenType::FECHA_KW},
                                                        {"hora",             TokenType::HORA_KW},
                                                        {"con",              TokenType::CON},
                                                        {"condicion",        TokenType::CONDICION},
                                                        {"medicamento",      TokenType::MEDICAMENTO},
                                                        {"dosis",            TokenType::DOSIS_KW},
                                                        {"CARDIOLOGIA",      TokenType::CARDIOLOGIA},
                                                        {"NEUROLOGIA",       TokenType::NEUROLOGIA},
                                                        {"PEDIATRIA",        TokenType::PEDIATRIA},
                                                        {"CIRUGIA",          TokenType::CIRUGIA},
                                                        {"MEDICINA_GENERAL", TokenType::MEDICINA_GENERAL},
                                                        {"ONCOLOGIA",        TokenType::ONCOLOGIA},
                                                        {"DIARIA",           TokenType::DIARIA},
                                                        {"CADA_8_HORAS",     TokenType::CADA_8_HORAS},
                                                        {"CADA_12_HORAS",    TokenType::CADA_12_HORAS},
                                                        {"SEMANAL",          TokenType::SEMANAL},
                                                        };
    auto it = keywords.find(lex);
    if (it != keywords.end()) return it->second;
    return TokenType::UNKNOWN;
}

void LexicalAnalyzer::analyze() {
    int state = 0;
    std::string lexeme = "";
    int tokenLine = 1, tokenCol = 1;

    while (true) {
        char c = peek();

        switch (state) {

        case 0: // Estado inicial
            if (c == '\0') return;

            if (isspace(c)) { advance(); break; }

            // Letra: puede ser keyword, identificador o enum
            if (isalpha(c) || c == '_') {
                tokenLine = line; tokenCol = column;
                lexeme += advance();
                state = 1;
                break;
            }

            // Digito: puede ser numero, fecha o hora
            if (isdigit(c)) {
                tokenLine = line; tokenCol = column;
                lexeme += advance();
                state = 2;
                break;
            }

            // Cadena de texto
            if (c == '"') {
                tokenLine = line; tokenCol = column;
                lexeme += advance();
                state = 3;
                break;
            }

            // Simbolos validos
            if (c == ':' || c == '[' || c == ']' ||
                c == '{' || c == '}' || c == ',' || c == ';') {
                tokenLine = line; tokenCol = column;
                lexeme += advance();
                tokens.push_back(Token(TokenType::SYMBOL, lexeme, tokenLine, tokenCol));
                lexeme = "";
                break;
            }

            // Caracter no reconocido
            tokenLine = line; tokenCol = column;
            lexeme += advance();
            errors.push_back(LexicalError(
                lexeme, tokenLine, tokenCol,
                "Token invalido",
                "Caracter no reconocido: " + lexeme,
                "ERROR"
                ));
            lexeme = "";
            break;

        case 1: // Identificador o keyword
            if (isalnum(c) || c == '_') {
                lexeme += advance();
            } else {
                TokenType type = checkKeyword(lexeme);
                if (type == TokenType::UNKNOWN) {
                    tokens.push_back(Token(TokenType::UNKNOWN, lexeme, tokenLine, tokenCol));
                } else {
                    tokens.push_back(Token(type, lexeme, tokenLine, tokenCol));
                }
                lexeme = "";
                state = 0;
            }
            break;

        case 2: // Numero, fecha (AAAA-MM-DD) o hora (HH:MM)
            if (isdigit(c)) {
                lexeme += advance();
            } else if (c == '-' && lexeme.size() == 4) {
                // Puede ser fecha AAAA-MM-DD
                lexeme += advance();
                state = 4;
            } else if (c == ':' && lexeme.size() == 2) {
                // Puede ser hora HH:MM
                lexeme += advance();
                state = 7;
            } else {
                tokens.push_back(Token(TokenType::NUMBER, lexeme, tokenLine, tokenCol));
                lexeme = "";
                state = 0;
            }
            break;

        case 3: // Dentro de cadena
            if (c == '"') {
                lexeme += advance();
                tokens.push_back(Token(TokenType::STRING, lexeme, tokenLine, tokenCol));
                lexeme = "";
                state = 0;
            } else if (c == '\0' || c == '\n') {
                // Cadena sin cerrar - registrar error Y agregar token especial
                errors.push_back(LexicalError(
                    lexeme, tokenLine, tokenCol,
                    "Cadena sin cerrar",
                    "Se encontro inicio de cadena pero no se cerro",
                    "CRITICO"
                    ));
                // Agregar token con valor especial para que los reportes
                // muestren N/A en lugar de datos incorrectos
                tokens.push_back(Token(
                    TokenType::STRING,
                    "\"N/A - Error en cadena\"",
                    tokenLine, tokenCol
                    ));
                lexeme = "";
                state = 0;
                // Si es salto de linea avanzamos para continuar
                if (c == '\n') advance();
            } else {
                lexeme += advance();
            }
            break;


        case 4: // Fecha: leyendo MM despues de AAAA-
            if (isdigit(c)) {
                lexeme += advance();
                if (lexeme.size() == 7) state = 5;
            } else {
                errors.push_back(LexicalError(
                    lexeme, tokenLine, tokenCol,
                    "Fecha invalida",
                    "Formato esperado: AAAA-MM-DD",
                    "ERROR"
                    ));
                lexeme = "";
                state = 0;
            }
            break;

        case 5: // Fecha: esperando segundo guion
            if (c == '-') {
                lexeme += advance();
                state = 6;
            } else {
                errors.push_back(LexicalError(
                    lexeme, tokenLine, tokenCol,
                    "Fecha invalida",
                    "Formato esperado: AAAA-MM-DD",
                    "ERROR"
                    ));
                lexeme = "";
                state = 0;
            }
            break;

        case 6: // Fecha: leyendo DD
            if (isdigit(c)) {
                lexeme += advance();
                if (lexeme.size() == 10) {
                    // Validar mes y dia
                    int mes = stoi(lexeme.substr(5, 2));
                    int dia = stoi(lexeme.substr(8, 2));
                    if (mes < 1 || mes > 12) {
                        errors.push_back(LexicalError(
                            lexeme, tokenLine, tokenCol,
                            "Fecha con mes invalido",
                            "El mes '" + std::to_string(mes) + "' esta fuera del rango (01-12)",
                            "ERROR"
                            ));
                    } else if (dia < 1 || dia > 31) {
                        errors.push_back(LexicalError(
                            lexeme, tokenLine, tokenCol,
                            "Fecha con dia invalido",
                            "El dia '" + std::to_string(dia) + "' esta fuera del rango (01-31)",
                            "ERROR"
                            ));
                    } else {
                        tokens.push_back(Token(TokenType::DATE, lexeme, tokenLine, tokenCol));
                    }
                    lexeme = "";
                    state = 0;
                }
            } else {
                errors.push_back(LexicalError(
                    lexeme, tokenLine, tokenCol,
                    "Fecha invalida",
                    "Formato esperado: AAAA-MM-DD",
                    "ERROR"
                    ));
                lexeme = "";
                state = 0;
            }
            break;

        case 7: // Hora: leyendo MM despues de HH:
            if (isdigit(c)) {
                lexeme += advance();
                if (lexeme.size() == 5) {
                    // Validar hora y minutos
                    int hora = stoi(lexeme.substr(0, 2));
                    int min  = stoi(lexeme.substr(3, 2));
                    if (hora > 23) {
                        errors.push_back(LexicalError(
                            lexeme, tokenLine, tokenCol,
                            "Hora fuera de rango",
                            "La hora '" + std::to_string(hora) + "' esta fuera del rango (00-23)",
                            "ERROR"
                            ));
                    } else if (min > 59) {
                        errors.push_back(LexicalError(
                            lexeme, tokenLine, tokenCol,
                            "Hora fuera de rango",
                            "Los minutos '" + std::to_string(min) + "' estan fuera del rango (00-59)",
                            "ERROR"
                            ));
                    } else {
                        tokens.push_back(Token(TokenType::TIME, lexeme, tokenLine, tokenCol));
                    }
                    lexeme = "";
                    state = 0;
                }
            } else {
                errors.push_back(LexicalError(
                    lexeme, tokenLine, tokenCol,
                    "Hora invalida",
                    "Formato esperado: HH:MM en 24 horas",
                    "ERROR"
                    ));
                lexeme = "";
                state = 0;
            }
            break;
        }
    }
}