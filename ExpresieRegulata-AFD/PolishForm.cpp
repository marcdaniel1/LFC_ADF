#include "PolishForm.h"
#include <iostream>
#include <cctype> 

bool isOperator(char character)
{
    return character == '+' || character == '-' || character == '*' || character == '/';
}

bool isBinaryOperator(char character)
{
    return character == '+' || character == '/';
}

bool isHigherPrecedence(char op1, char op2)
{
    if ((op1 == '*' || op1 == '/') && (op2 == '+' || op2 == '-'))
    {
        return true;
    }
    return false;
}

void handleOperator(char op, std::stack<char>& stack, std::string& polishNotation)
{
    while (!stack.empty() && stack.top() != '(' && isHigherPrecedence(stack.top(), op))
    {
        polishNotation += stack.top();
        stack.pop();
    }
    stack.push(op);
}

std::string toPolishNotation(const std::string& expression)
{
    std::string polishNotation;
    std::stack<char> operators;

    for (int i = 0; i < expression.size(); ++i)
    {
        char current = expression[i];

        if (current == ' ')
        {
            continue;
        }

        if (std::isalnum(current))
        {
            polishNotation += current;
        }
        else if (current == '(')
        {
            operators.push(current);
        }
        else if (current == ')')
        {
            while (!operators.empty() && operators.top() != '(')
            {
                polishNotation += operators.top();
                operators.pop();
            }
            if (!operators.empty())
            {
                operators.pop();
            }
        }
        else if (isOperator(current))
        {
            handleOperator(current, operators, polishNotation);
        }
        else
        {
            std::cerr << "Caracter invalid: " << current << std::endl;
            return "";
        }
    }

    while (!operators.empty())
    {
        polishNotation += operators.top();
        operators.pop();
    }

    return polishNotation;
}

std::string addPlusBetweenChars(const std::string& expression)
{
    std::string expressionWithPluses;
    bool lastCharacterNeedsPlus = false;
    for (int i = 0; i < expression.size(); i++)
    {
        if (lastCharacterNeedsPlus and (std::isalpha(expression[i]) or expression[i] == '('))
        {
            expressionWithPluses += '+';
        }

        if (std::isalpha(expression[i]) or expression[i] == ')' or expression[i] == '*')
        {
            lastCharacterNeedsPlus = true;
        }
        else
        {
            lastCharacterNeedsPlus = false;
        }

        expressionWithPluses += expression[i];
    }
    return expressionWithPluses;
}