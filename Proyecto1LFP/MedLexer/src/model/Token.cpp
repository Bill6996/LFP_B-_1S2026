#include "Token.h"

Token::Token(TokenType type, std::string lexeme, int line, int column) {
    this->type = type;
    this->lexeme = lexeme;
    this->line = line;
    this->column = column;
}

std::string Token::getTypeString() const {
    switch (type) {
    case TokenType::HOSPITAL:         return "HOSPITAL";
    case TokenType::PACIENTES:        return "PACIENTES";
    case TokenType::MEDICOS:          return "MEDICOS";
    case TokenType::CITAS:            return "CITAS";
    case TokenType::DIAGNOSTICOS:     return "DIAGNOSTICOS";
    case TokenType::PACIENTE:         return "PACIENTE";
    case TokenType::MEDICO:           return "MEDICO";
    case TokenType::CITA:             return "CITA";
    case TokenType::DIAGNOSTICO:      return "DIAGNOSTICO";
    case TokenType::EDAD:             return "EDAD";
    case TokenType::TIPO_SANGRE:      return "TIPO_SANGRE";
    case TokenType::HABITACION:       return "HABITACION";
    case TokenType::ESPECIALIDAD:     return "ESPECIALIDAD";
    case TokenType::CODIGO:           return "CODIGO";
    case TokenType::FECHA_KW:         return "FECHA";
    case TokenType::HORA_KW:          return "HORA";
    case TokenType::CON:              return "CON";
    case TokenType::CONDICION:        return "CONDICION";
    case TokenType::MEDICAMENTO:      return "MEDICAMENTO";
    case TokenType::DOSIS_KW:         return "DOSIS";
    case TokenType::CARDIOLOGIA:      return "CARDIOLOGIA";
    case TokenType::NEUROLOGIA:       return "NEUROLOGIA";
    case TokenType::PEDIATRIA:        return "PEDIATRIA";
    case TokenType::CIRUGIA:          return "CIRUGIA";
    case TokenType::MEDICINA_GENERAL: return "MEDICINA_GENERAL";
    case TokenType::ONCOLOGIA:        return "ONCOLOGIA";
    case TokenType::DIARIA:           return "DIARIA";
    case TokenType::CADA_8_HORAS:     return "CADA_8_HORAS";
    case TokenType::CADA_12_HORAS:    return "CADA_12_HORAS";
    case TokenType::SEMANAL:          return "SEMANAL";
    case TokenType::NUMBER:           return "NUMBER";
    case TokenType::STRING:           return "STRING";
    case TokenType::DATE:             return "DATE";
    case TokenType::TIME:             return "TIME";
    case TokenType::ID_CODE:          return "ID_CODE";
    case TokenType::SYMBOL:           return "SYMBOL";
    default:                          return "UNKNOWN";
    }
}