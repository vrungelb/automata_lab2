#include "nfadot.hpp"
#include <fstream>
#include <cstdlib>

void writeNfaDot(const NFA& nfa, const std::string& dotFilename) {
    std::ofstream out(dotFilename);
    out << "digraph NFA {\n";
    out << "  rankdir=LR;\n";
    out << "  node [shape=circle, fontname=\"DejaVu Sans\"];\n";

    out << "  " << nfa.getAccept() << " [shape=doublecircle];\n";

    out << "  start_arrow [shape=point];\n";
    out << "  start_arrow -> " << nfa.getStart() << ";\n";

    for (const Transition& t : nfa.getTransitions()) {
        std::string label = t.is_epsilon ? "ε" : std::string(1, t.transition_symbol);
        out << "  " << t.from_state << " -> " << t.to_state
            << " [label=\"" << label << "\"];\n";
    }

    out << "}\n";
}

bool renderNfa(const NFA& nfa, const std::string& filename) {
    const std::string dot = filename + ".dot";
    const std::string png = filename + ".png";
    writeNfaDot(nfa, dot);
    const std::string cmd = "dot -Tpng \"" + dot + "\" -o \"" + png + "\"";
    return std::system(cmd.c_str()) == 0;
}
