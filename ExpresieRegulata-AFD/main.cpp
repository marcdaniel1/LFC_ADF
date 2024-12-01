#include <iostream>
#include <string>
#include "PolishForm.h"
#include "DeterministicFiniteAutomaton.h"

int main()
{
    std::string expression;

    std::cout << "Expresia: ";
    std::getline(std::cin, expression);

    std::string modifiedExpression = addPlusBetweenChars(expression);

    std::string polishNotation = toPolishNotation(modifiedExpression);

    std::cout << "Forma poloneza: " << polishNotation << std::endl;

    return 0;
}
