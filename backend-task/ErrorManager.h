#pragma once
#include <string>
#include <vector>

enum class ErrorType {
    LEXICO,
    SINTACTICO
};

struct ErrorInfo {
    ErrorType type;
    std::string lexeme;
    std::string description;
    int line;
    int column;
};

class ErrorManager {
private:
    std::vector<ErrorInfo> errors;

public:
    void addError(ErrorType type, std::string lexeme, std::string description, int line, int column);
    bool hasErrors() const;
    const std::vector<ErrorInfo>& getErrors() const;
    void clear();
};