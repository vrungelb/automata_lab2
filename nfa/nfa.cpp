#include "nfa.hpp"

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
            addEpsilon(left.exit_state, right.entry_state);
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

        case NodeKind::Repeat:
            break;

        case NodeKind::Group: {
            NfaFragment left = build(node->left.get());
            return {left};
        }

        case NodeKind::Backref:
            break;
    }
}
