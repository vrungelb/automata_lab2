#include <iostream>
#include <string>

#include "parser.hpp"
#include "treedot.hpp"
#include "nfa.hpp"
#include "nfadot.hpp"


int main() {
    std::cout << "Enter your string: " << std::endl;
    std::string user_input_string;
    std::getline(std::cin, user_input_string);

    Parser ParserObj(user_input_string);
    try {
        auto ParsedObj = ParserObj.parse();
        bool ok = renderTree(ParsedObj.root.get(), "tree");
        if (ok) std::cout << "Tree saved to tree.png" << std::endl;
        else std::cout << "Error: Graphviz not found" << std::endl;

        NFA NfaObj;
        NfaObj.buildFromAst(ParsedObj.root.get());
        bool okk = renderNfa(NfaObj, "nfa");
        if (okk) std::cout << "NFA saved to nfa.png" << std::endl;
        else std::cout << "Error: Graphviz not found" << std::endl;
    }

    catch (const ParseError& e) {
        std::cout << "Parsing error! " << e.what() << std::endl;
    }

    catch (const std::exception& e) {
        std::cout << "Cannot build NFA: " << e.what() << std::endl;
    }

    return 0;
}
