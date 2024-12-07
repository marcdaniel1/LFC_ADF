#include <string>
#include <stack>

// Verif - un carac este un op
bool isOperator(char character);

// Verif - un carac este un op binar
bool isBinaryOperator(char character);

// Verif - primul op are pr mai mare decât al doilea
bool isHigherPrecedence(char op1, char op2);

void handleOperator(char op, std::stack<char>& stack, std::string& polishNotation);

std::string toPolishNotation(const std::string& expression);

std::string addPlusBetweenChars(const std::string& expression);

