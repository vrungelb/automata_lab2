#ifndef LAB2_NFADOT_HPP
#define LAB2_NFADOT_HPP

#include <string>
#include "nfa.hpp"

void writeNfaDot(const NFA& nfa, const std::string& dotFilename);

bool renderNfa(const NFA& nfa, const std::string& filename = "nfa");

#endif //LAB2_NFADOT_HPP
