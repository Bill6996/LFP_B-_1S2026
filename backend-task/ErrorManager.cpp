#include "ErrorManager.h"

void ErrorManager::addError(ErrorType type, std::string lexeme, std::string description, int line, int column) {
    errors.push_back({type, lexeme, description, line, column});
}

bool ErrorManager::hasErrors() const {
    return !errors.empty();
}

const std::vector<ErrorInfo>& ErrorManager::getErrors() const {
    return errors;
}

void ErrorManager::clear() {
    errors.clear();
}