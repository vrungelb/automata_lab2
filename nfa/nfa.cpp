#include "nfa.hpp"
#include <stdexcept>

size_t NFA::newState() {
    return num_states++;
}

void NFA::addTransition(size_t from_state, size_t to_state, char transition_symbol) {
    transitions.push_back({from_state, to_state, transition_symbol, false});
}

void NFA::addEpsilon(size_t from_state, size_t to_state) {
    transitions.push_back({from_state, to_state, '\0',true});
}

void NFA::buildFromAst(const AstNode* node) {
    NfaFragment nfa_result = build(node);
    start = nfa_result.entry_state;
    accept = nfa_result.exit_state;
}

void NFA::mergeStates(size_t victim, size_t survivor) {
    for (Transition& t: transitions) {
        if (t.from_state == victim) {
            t.from_state = survivor;
        }
        if (t.to_state == victim) {
            t.to_state = survivor;
        }
    }
}

NfaFragment NFA::build(const AstNode* node) {
    switch (node->kind) {
        case NodeKind::Symbol: {
            size_t entry_state = newState();
            size_t exit_state = newState();
            addTransition(entry_state, exit_state, node->symbol);
            return {entry_state, exit_state};
        }

        case NodeKind::CharClass: {
            size_t entry_state = newState();
            size_t exit_state = newState();
            for (char c: node->charset) {
                addTransition(entry_state, exit_state, c);
            }
            return {entry_state, exit_state};
        }

        case NodeKind::Empty: {
            size_t entry_state = newState();
            size_t exit_state = newState();
            addEpsilon(entry_state, exit_state);
            return {entry_state, exit_state};
        }

        case NodeKind::Concat: {
            NfaFragment left = NFA::build(node->left.get());
            NfaFragment right = NFA::build(node->right.get());
            mergeStates(left.exit_state, right.entry_state);
            return{left.entry_state, right.exit_state};
        }

        case NodeKind::Alt: {
            NfaFragment left = NFA::build(node->left.get());
            NfaFragment right = NFA::build(node->right.get());
            size_t entry_state = newState();
            size_t exit_state = newState();
            addEpsilon(entry_state, left.entry_state);
            addEpsilon(entry_state, right.entry_state);
            addEpsilon(left.exit_state, exit_state);
            addEpsilon(right.exit_state, exit_state);
            return{entry_state, exit_state};
        }

        case NodeKind::Star: {
            NfaFragment left = NFA::build(node->left.get());
            size_t entry_state = newState();
            size_t exit_state = newState();
            addEpsilon(entry_state, left.entry_state);
            addEpsilon(entry_state, exit_state);
            addEpsilon(left.exit_state, left.entry_state);
            addEpsilon(left.exit_state, exit_state);
            return {entry_state, exit_state};
        }

        case NodeKind::Repeat: {
            if (node->count == 0) {
                size_t entry_state = newState();
                size_t exit_state = newState();
                addEpsilon(entry_state, exit_state);
                return {entry_state, exit_state};
            }
            if (node->count == 1) {
                NfaFragment left = build(node->left.get());
                return left;
            }
            NfaFragment chain = build(node->left.get());
            for (int i = 1; i < node->count; i++) {
                NfaFragment next = build(node->left.get());
                mergeStates(chain.exit_state, next.entry_state);
                chain.exit_state = next.exit_state;
            }
            return chain;
        }

        case NodeKind::Group: {
            NfaFragment left = build(node->left.get());
            return left;
        }

        case NodeKind::Backref:
            throw std::runtime_error("Language with backrefs is non-regular");
    }
    throw std::runtime_error("Unknown node kind");
}
