#ifndef LEXICAL_ERROR_H
#define LEXICAL_ERROR_H
#include <string>
using namespace std;

class LexicalError {
public:
    string lexeme;
    int line;
    int column;
    string errorType;
    string description;
    string severity;

    LexicalError(string lex, int l, int c, string errType, string desc, string sev) {
        lexeme      = lex;
        line        = l;
        column      = c;
        errorType   = errType;
        description = desc;
        severity    = sev;
    }
};

#endif