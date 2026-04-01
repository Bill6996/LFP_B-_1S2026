#pragma once
#include <string>

enum class TokenType {
    HOSPITAL, PACIENTES, MEDICOS, CITAS, DIAGNOSTICOS,
    PACIENTE, MEDICO, CITA, DIAGNOSTICO,
    EDAD, TIPO_SANGRE, HABITACION, ESPECIALIDAD,
    CODIGO, FECHA_KW, HORA_KW, CON,
    CONDICION, MEDICAMENTO, DOSIS_KW,
    CARDIOLOGIA, NEUROLOGIA, PEDIATRIA,
    CIRUGIA, MEDICINA_GENERAL, ONCOLOGIA,
    DIARIA, CADA_8_HORAS, CADA_12_HORAS, SEMANAL,
    NUMBER, STRING, DATE, TIME, ID_CODE,
    SYMBOL, UNKNOWN
};

class Token {
public:
    TokenType type;
    std::string lexeme;
    int line;
    int column;
    Token(TokenType type, std::string lexeme, int line, int column);
    std::string getTypeString() const;
};