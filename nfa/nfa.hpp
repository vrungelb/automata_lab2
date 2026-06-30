#ifndef LAB2_NFA_HPP
#define LAB2_NFA_HPP

#include <cstddef>
#include <vector>

#include "ast.hpp"

struct Transition {
    size_t from_state;
    size_t to_state;
    char transition_symbol;
    bool is_epsilon;
};

struct NfaFragment {                                                                    // фрагмент НКА
    size_t entry_state;
    size_t exit_state;
};

class NFA {
public:
    size_t newState();
    void addTransition(size_t from_state, size_t to_state, char transition_symbol);
    void addEpsilon(size_t from_state, size_t to_state);
    void buildFromAst(const AstNode* node);
    [[nodiscard]] size_t getNumStates() const {
        return num_states;
    }
    [[nodiscard]] size_t getStart() const {
        return start;
    }
    [[nodiscard]] size_t getAccept() const {
        return accept;
    }
    [[nodiscard]] const std::vector<Transition>& getTransitions() const {
        return transitions;
    }
private:
    std::vector<Transition> transitions;                                                // список переходов
    NfaFragment build(const AstNode* node);                                             // билдер НКА
    void mergeStates(size_t victim, size_t survivor);
    size_t num_states = 0;
    size_t start = 0;
    size_t accept = 0;
};

#endif //LAB2_NFA_HPP
