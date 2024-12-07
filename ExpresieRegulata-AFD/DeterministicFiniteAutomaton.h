#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <stack>
#include <string>
#include <queue>

class DeterministicFiniteAutomaton {
private:
    std::set<std::string> states;
    std::set<char> alphabet;
    std::map<std::pair<std::string, char>, std::string> transitions;
    std::string initialState;
    std::set<std::string> finalStates;

public:
    DeterministicFiniteAutomaton(const std::set<std::string>& Q, const std::set<char>& A,
        const std::map<std::pair<std::string, char>, std::string>& T,
        const std::string& q0, const std::set<std::string>& F);

    bool VerifyAutomaton() const;
    void PrintAutomaton() const;
    bool CheckWord(const std::string& word) const;
    DeterministicFiniteAutomaton buildLambdaNFA(const std::string& polishExpression);
    DeterministicFiniteAutomaton convertToDFA() const;
    std::set<std::string> calculateLambdaClosure(const std::string& state) const;
    std::string generateStateName(const std::set<std::string>& stateSet) const;
};
