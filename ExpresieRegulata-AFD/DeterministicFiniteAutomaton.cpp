#include "DeterministicFiniteAutomaton.h"

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

// Funcție de construire a unui AFN cu λ-tranziții din forma poloneză
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
        else if (symbol == '|') {
            // Logică pentru alternare (|) între două automate din stivă
        }
        else if (symbol == '.') {
            // Logică pentru concatenare
        }
        else if (symbol == '*') {
            // Logică pentru stelare (Kleene Star)
        }
    }

    return automataStack.top(); // Automat complet construit
}

DeterministicFiniteAutomaton DeterministicFiniteAutomaton::convertToDFA() const {
    // Noul set de stări, alfabetul și starea inițială
    std::set<std::string> newStates;
    std::set<char> newAlphabet = alphabet;
    std::map<std::pair<std::string, char>, std::string> newTransitions;
    std::string newInitialState;
    std::set<std::string> newFinalStates;

    // Mapare între mulțimile de stări și noile stări ale AFD
    std::map<std::set<std::string>, std::string> stateMapping;
    std::queue<std::set<std::string>> stateQueue;

    // Calculăm închiderea λ pentru toate stările
    std::map<std::string, std::set<std::string>> lambdaClosure;
    for (const auto& state : states) {
        lambdaClosure[state] = calculateLambdaClosure(state);
    }

    // Creăm noua stare inițială ca închiderea λ a stării inițiale
    std::set<std::string> initialClosure = lambdaClosure[initialState];
    newInitialState = generateStateName(initialClosure);
    newStates.insert(newInitialState);
    stateMapping[initialClosure] = newInitialState;
    stateQueue.push(initialClosure);

    // Verificăm dacă noua stare inițială este finală
    for (const auto& state : initialClosure) {
        if (finalStates.find(state) != finalStates.end()) {
            newFinalStates.insert(newInitialState);
            break;
        }
    }

    // Procesăm fiecare stare din coadă
    while (!stateQueue.empty()) {
        auto currentSet = stateQueue.front();
        stateQueue.pop();
        std::string currentStateName = stateMapping[currentSet];

        // Calculăm tranzițiile pentru fiecare simbol din alfabet
        for (const auto& symbol : alphabet) {
            std::set<std::string> newSet;

            for (const auto& state : currentSet) {
                auto range = transitions.equal_range({ state, symbol });
                for (auto it = range.first; it != range.second; ++it) {
                    const std::string& targetState = it->second;
                    // Adăugăm toate stările din închiderea λ a stării țintă
                    newSet.insert(lambdaClosure[targetState].begin(), lambdaClosure[targetState].end());
                }
            }

            if (!newSet.empty()) {
                std::string newSetName;
                if (stateMapping.find(newSet) == stateMapping.end()) {
                    // Generăm un nume pentru noua stare
                    newSetName = generateStateName(newSet);
                    stateMapping[newSet] = newSetName;
                    newStates.insert(newSetName);
                    stateQueue.push(newSet);

                    // Verificăm dacă noua stare este finală
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

                // Adăugăm tranziția în noul AFD
                newTransitions[{currentStateName, symbol}] = newSetName;
            }
        }
    }

    return DeterministicFiniteAutomaton(newStates, newAlphabet, newTransitions, newInitialState, newFinalStates);
}

// Funcție auxiliară pentru calcularea închiderii λ a unei stări
std::set<std::string> DeterministicFiniteAutomaton::calculateLambdaClosure(const std::string& state) const {
    std::set<std::string> closure;
    std::queue<std::string> queue;

    closure.insert(state);
    queue.push(state);

    while (!queue.empty()) {
        std::string current = queue.front();
        queue.pop();

        auto range = transitions.equal_range({ current, '\0' }); // '\0' pentru λ
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

// Funcție auxiliară pentru generarea unui nume pentru o stare compusă
std::string DeterministicFiniteAutomaton::generateStateName(const std::set<std::string>& stateSet) const {
    std::string name = "{";
    for (const auto& state : stateSet) {
        if (name.length() > 1) name += ",";
        name += state;
    }
    name += "}";
    return name;
}
