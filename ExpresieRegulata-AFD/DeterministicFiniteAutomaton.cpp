#pragma once
#include "DeterministicFiniteAutomaton.h"

DeterministicFiniteAutomaton::DeterministicFiniteAutomaton() {

}

const std::set<std::string>& DeterministicFiniteAutomaton::getStates() const {
    return states;
}

const std::set<char>& DeterministicFiniteAutomaton::getAlphabet() const {
    return alphabet;
}

const std::map<std::pair<std::string, char>, std::string>& DeterministicFiniteAutomaton::getTransitions() const {
    return transitions;
}

const std::string& DeterministicFiniteAutomaton::getInitialState() const {
    return initialState;
}

const std::set<std::string>& DeterministicFiniteAutomaton::getFinalStates() const {
    return finalStates;
}


DeterministicFiniteAutomaton::DeterministicFiniteAutomaton(const std::set<std::string>& Q, const std::set<char>& A,
    const std::map<std::pair<std::string, char>, std::string>& T,
    const std::string& q0, const std::set<std::string>& F)
    : states(Q), alphabet(A), transitions(T), initialState(q0), finalStates(F) {
}

bool DeterministicFiniteAutomaton::VerifyAutomaton() const {
    // Verifica daca starea initiala e valida
    if (states.find(initialState) == states.end()) {
        std::cerr << "Eroare: Starea initiala nu este in multimea starilor.\n";
        return false;
    }

    // Verifica daca toate starile finale sunt valide
    for (const auto& finalState : finalStates) {
        if (states.find(finalState) == states.end()) {
            std::cerr << "Eroare: O stare finala nu este in multimea starilor.\n";
            return false;
        }
    }

    // Verifica corectitudinea functiei de tranzitie
    for (const auto& [key, value] : transitions) {
        const auto& [state, symbol] = key;
        if (states.find(state) == states.end() || alphabet.find(symbol) == alphabet.end()) {
            std::cerr << "Eroare: O cheie din functia de tranzitie nu este valida.\n";
            return false;
        }
        if (states.find(value) == states.end()) {
            std::cerr << "Eroare: O stare tinta din functia de tranzitie nu este valida.\n";
            return false;
        }
    }

    return true;
}

void DeterministicFiniteAutomaton::PrintAutomaton() const {
    std::cout << "Multimea starilor (Q): { ";
    for (const auto& state : states) {
        std::cout << state << " ";
    }
    std::cout << "}\n";

    std::cout << "Alfabetul (Σ): { ";
    for (const auto& symbol : alphabet) {
        std::cout << symbol << " ";
    }
    std::cout << "}\n";

    std::cout << "Functia de tranzitie (δ):\n";
    for (const auto& [key, value] : transitions) {
        std::cout << "  δ(" << key.first << ", " << key.second << ") -> " << value << "\n";
    }

    std::cout << "Starea initiala (q0): " << initialState << "\n";

    std::cout << "Multimea starilor finale (F): { ";
    for (const auto& finalState : finalStates) {
        std::cout << finalState << " ";
    }
    std::cout << "}\n";
}

bool DeterministicFiniteAutomaton::CheckWord(const std::string& word) const {
    std::string currentState = initialState;

    for (char symbol : word) {
        if (alphabet.find(symbol) == alphabet.end()) {
            std::cerr << "Eroare: Simbolul '" << symbol << "' nu este in alfabet.\n";
            return false;
        }

        auto it = transitions.find({ currentState, symbol });
        if (it == transitions.end()) {
            std::cerr << "Eroare: Nu exista tranzitie pentru starea " << currentState
                << " cu simbolul '" << symbol << "'.\n";
            return false;
        }

        currentState = it->second; // Trece in starea urmatoare
    }

    // Verificam daca suntem intr-o stare finala
    return finalStates.find(currentState) != finalStates.end();
}

// Functie care construieste AFN cu λ-tranzitii din forma poloneza
DeterministicFiniteAutomaton buildLambdaNFA(const std::string& polishExpression) {
    std::stack<DeterministicFiniteAutomaton> automataStack;
    int stateCounter = 0;

    for (char symbol : polishExpression) {
        if (std::isalpha(symbol)) { // Simbol este caracter
            std::set<std::string> states = { "q" + std::to_string(stateCounter), "q" + std::to_string(stateCounter + 1) };
            std::set<char> alphabet = { symbol };
            std::map<std::pair<std::string, char>, std::string> transitions = {
                { {"q" + std::to_string(stateCounter), symbol}, "q" + std::to_string(stateCounter + 1) }
            };
            DeterministicFiniteAutomaton automaton(states, alphabet, transitions, "q" + std::to_string(stateCounter), { "q" + std::to_string(stateCounter + 1) });
            automataStack.push(automaton);
            stateCounter += 2;
        }
        else if (symbol == '|') { // Alternare (sau logic)
            if (automataStack.size() < 2) {
                std::cerr << "Eroare: prea putine automate pentru operatorul '|'.\n";
                return DeterministicFiniteAutomaton({}, {}, {}, "", {});
            }

            // Scoatem cele doua automate din stiva
            DeterministicFiniteAutomaton automaton2 = automataStack.top(); automataStack.pop();
            DeterministicFiniteAutomaton automaton1 = automataStack.top(); automataStack.pop();

            // Cream noi stari initială si finala
            std::string newInitial = "q" + std::to_string(stateCounter++);
            std::string newFinal = "q" + std::to_string(stateCounter++);

            // Construim noul automat
            std::set<std::string> states = automaton1.getStates();
            states.insert(automaton2.getStates().begin(), automaton2.getStates().end());
            states.insert(newInitial);
            states.insert(newFinal);

            std::map<std::pair<std::string, char>, std::string> transitions = automaton1.getTransitions();
            transitions.insert(automaton2.getTransitions().begin(), automaton2.getTransitions().end());

            transitions[{newInitial, '\0'}] = automaton1.getInitialState();
            transitions[{newInitial, '\0'}] = automaton2.getInitialState();
            for (const auto& finalState : automaton1.getFinalStates()) {
                transitions[{finalState, '\0'}] = newFinal;
            }
            for (const auto& finalState : automaton2.getFinalStates()) {
                transitions[{finalState, '\0'}] = newFinal;
            }

            DeterministicFiniteAutomaton newAutomaton(states, automaton1.getAlphabet(), transitions, newInitial, { newFinal });
            automataStack.push(newAutomaton);
        }
        else if (symbol == '.') { // Concatenare
            if (automataStack.size() < 2) {
                std::cerr << "Eroare: prea putine automate pentru operatorul '.'.\n";
                return DeterministicFiniteAutomaton({}, {}, {}, "", {});
            }

            // Scoatem cele doua automate din stiva
            DeterministicFiniteAutomaton automaton2 = automataStack.top(); automataStack.pop();
            DeterministicFiniteAutomaton automaton1 = automataStack.top(); automataStack.pop();

            std::set<std::string> states = automaton1.getStates();
            states.insert(automaton2.getStates().begin(), automaton2.getStates().end());

            std::map<std::pair<std::string, char>, std::string> transitions = automaton1.getTransitions();
            transitions.insert(automaton2.getTransitions().begin(), automaton2.getTransitions().end());

            for (const auto& finalState : automaton1.getFinalStates()) {
                transitions[{finalState, '\0'}] = automaton2.getInitialState();
            }

            DeterministicFiniteAutomaton newAutomaton(states, automaton1.getAlphabet(), transitions, automaton1.getInitialState(), automaton2.getFinalStates());
            automataStack.push(newAutomaton);
        }
        else if (symbol == '*') { // Stelare (Kleene Star)
            if (automataStack.empty()) {
                std::cerr << "Eroare: niciun automat pentru operatorul '*'.\n";
                return DeterministicFiniteAutomaton({}, {}, {}, "", {});
            }

            // Scoatem automatul din stiva
            DeterministicFiniteAutomaton automaton = automataStack.top(); automataStack.pop();

            std::string newInitial = "q" + std::to_string(stateCounter++);
            std::string newFinal = "q" + std::to_string(stateCounter++);

            std::set<std::string> states = automaton.getStates();
            states.insert(newInitial);
            states.insert(newFinal);

            std::map<std::pair<std::string, char>, std::string> transitions = automaton.getTransitions();

            transitions[{newInitial, '\0'}] = automaton.getInitialState();
            transitions[{newInitial, '\0'}] = newFinal;
            for (const auto& finalState : automaton.getFinalStates()) {
                transitions[{finalState, '\0'}] = automaton.getInitialState();
                transitions[{finalState, '\0'}] = newFinal;
            }

            DeterministicFiniteAutomaton newAutomaton(states, automaton.getAlphabet(), transitions, newInitial, { newFinal });
            automataStack.push(newAutomaton);
        }
    }

    if (automataStack.empty()) {
        std::cerr << "Eroare: stiva este goala la sfarait.\n";
        return DeterministicFiniteAutomaton({}, {}, {}, "", {});
    }

    return automataStack.top(); // Automat complet construit
}

DeterministicFiniteAutomaton DeterministicFiniteAutomaton::convertToDFA() const {
    // Noul set de stări, alfabetul si starea initială
    std::set<std::string> newStates;
    std::set<char> newAlphabet = alphabet;
    std::map<std::pair<std::string, char>, std::string> newTransitions;
    std::string newInitialState;
    std::set<std::string> newFinalStates;

    // Mapare intre multimile de stari si noile stari ale AFD
    std::map<std::set<std::string>, std::string> stateMapping;
    std::queue<std::set<std::string>> stateQueue;

    // Calculeaza inchiderea lambda pentru toate starile
    std::map<std::string, std::set<std::string>> lambdaClosure;
    for (const auto& state : states) {
        lambdaClosure[state] = calculateLambdaClosure(state);
    }

    // Creaza noua stare initială ca inchiderea lambda a starii initiale
    std::set<std::string> initialClosure = lambdaClosure[initialState];
    newInitialState = generateStateName(initialClosure);
    newStates.insert(newInitialState);
    stateMapping[initialClosure] = newInitialState;
    stateQueue.push(initialClosure);

    // Verifica daca noua stare initială e finala
    for (const auto& state : initialClosure) {
        if (finalStates.find(state) != finalStates.end()) {
            newFinalStates.insert(newInitialState);
            break;
        }
    }

    // Proceseza fiecare stare din coada
    while (!stateQueue.empty()) {
        auto currentSet = stateQueue.front();
        stateQueue.pop();
        std::string currentStateName = stateMapping[currentSet];

        // Calculeaza tranzitiile pentru fiecare simbol din alfabet
        for (const auto& symbol : alphabet) {
            std::set<std::string> newSet;

            for (const auto& state : currentSet) {
                auto range = transitions.equal_range({ state, symbol });
                for (auto it = range.first; it != range.second; ++it) {
                    const std::string& targetState = it->second;
                    // Adauga toate starile din inchiderea /lambda a starii tinta
                    newSet.insert(lambdaClosure[targetState].begin(), lambdaClosure[targetState].end());
                }
            }

            if (!newSet.empty()) {
                std::string newSetName;
                if (stateMapping.find(newSet) == stateMapping.end()) {
                    // Genereaza un nume pentru noua stare
                    newSetName = generateStateName(newSet);
                    stateMapping[newSet] = newSetName;
                    newStates.insert(newSetName);
                    stateQueue.push(newSet);

                    // Verifica daca noua stare este finala
                    for (const auto& state : newSet) {
                        if (finalStates.find(state) != finalStates.end()) {
                            newFinalStates.insert(newSetName);
                            break;
                        }
                    }
                }
                else {
                    newSetName = stateMapping[newSet];
                }

                // Adauga tranzitia in noul AFD
                newTransitions[{currentStateName, symbol}] = newSetName;
            }
        }
    }

    return DeterministicFiniteAutomaton(newStates, newAlphabet, newTransitions, newInitialState, newFinalStates);
}

// Functie auxiliara pentru calcularea inchiderii lambda a unei stari
std::set<std::string> DeterministicFiniteAutomaton::calculateLambdaClosure(const std::string& state) const {
    std::set<std::string> closure;
    std::queue<std::string> queue;

    closure.insert(state);
    queue.push(state);

    while (!queue.empty()) {
        std::string current = queue.front();
        queue.pop();

        auto range = transitions.equal_range({ current, '\0' }); // '\0' pentru lambda
        for (auto it = range.first; it != range.second; ++it) {
            const std::string& target = it->second;
            if (closure.find(target) == closure.end()) {
                closure.insert(target);
                queue.push(target);
            }
        }
    }

    return closure;
}

// Functie auxiliara pentru generarea unui nume pentru o stare compusă
std::string DeterministicFiniteAutomaton::generateStateName(const std::set<std::string>& stateSet) const {
    std::string name = "{";
    for (const auto& state : stateSet) {
        if (name.length() > 1) name += ",";
        name += state;
    }
    name += "}";
    return name;
}
