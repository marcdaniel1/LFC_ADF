#include <iostream>
#include <fstream>
#include <string>
#include "PolishForm.h"
#include "DeterministicFiniteAutomaton.h"

void meniu(const std::string& inputFilePath) {
    // Citirea expresiei regulate din fișier
    std::ifstream file(inputFilePath);
    if (!file.is_open()) {
        std::cerr << "Fisierul nu a putut fi deschis: " << inputFilePath << "\n";
        return;
    }

    std::string expression;
    std::getline(file, expression);
    file.close();

    if (expression.empty()) {
        std::cerr << "Fisierul nu contine o expresie regulata valida.\n";
        return;
    }

    std::cout << "Expresia regulata citita: " << expression << "\n";

    // Adăugarea "+" între caractere, dacă este necesar
    std::string modifiedExpression = addPlusBetweenChars(expression);

    // Transformarea în forma poloneză
    std::string polishNotation = toPolishNotation(modifiedExpression);

    if (polishNotation.empty()) {
        std::cerr << "Expresia regulata citita este invalida.\n";
        return;
    }

    std::cout << "Forma poloneza: " << polishNotation << std::endl;

    //// Construirea automatului finit determinist
    //auto lambdaNFA = buildLambdaNFA(polishNotation);
    //auto dfa = lambdaNFA.convertToDFA();
    DeterministicFiniteAutomaton lambdaNFA = buildLambdaNFA(const polishNotation);

    // Verificăm dacă automatul este valid
    if (!lambdaNFA.VerifyAutomaton()) {
        std::cerr << "Automatul finit nedeterminist (AFN-λ) nu este valid.\n";
        return;
    }

    // Transformarea AFN-λ într-un AFD
    DeterministicFiniteAutomaton dfa = lambdaNFA.convertToDFA();

    // Meniul interactiv
    int optiune = -1;
    while (optiune != 0) {
        std::cout << "\n<+++++++++++++++++++++>Meniu<+++++++++++++++++++++>\n\n";
        std::cout << "1. Afisarea expresiei regulate.\n";
        std::cout << "2. Afisarea automatului finit determinist (AFD).\n";
        std::cout << "3. Verificarea unui cuvant in AFD.\n";
        std::cout << "0. Iesire\n";
        std::cout << "Ce optiune alegeti: ";
        std::cin >> optiune;
        std::cin.ignore(); // Consumă newline-ul rămas după std::cin
        std::cout << "\n";

        switch (optiune) {
        case 1: {
            std::cout << "Expresia regulata este: " << expression << "\n";
            break;
        }
        case 2: {
            std::cout << "Automatul finit determinist este:\n";
            dfa.PrintAutomaton();
            break;
        }
        case 3: {
            std::string word;
            std::cout << "Introduceti cuvantul pentru verificare: ";
            std::getline(std::cin, word);

            if (dfa.CheckWord(word)) {
                std::cout << "Cuvantul \"" << word << "\" este acceptat de automat.\n";
            } else {
                std::cout << "Cuvantul \"" << word << "\" NU este acceptat de automat.\n";
            }
            break;
        }
        case 0: {
            std::cout << "Iesire din program.\n";
            break;
        }
        default:
            std::cout << "Optiune invalida! Alegeti alta optiune.\n";
            break;
        }
    }
}

int main() {
    // Calea fișierului de intrare
    const std::string inputFilePath = "regex.txt";

    // Lansarea meniului interactiv
    meniu(inputFilePath);

    return 0;
}
