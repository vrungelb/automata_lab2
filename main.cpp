#include <iostream>
#include <string>

#include "parser.hpp"
#include "treedot.hpp"


int main() {
    std::cout << "Enter your string: " << std::endl;
    std::string user_input_string;
    std::getline(std::cin, user_input_string);

    Parser ParserObj(user_input_string);
    try {
        auto ParsedObj = ParserObj.parse();
    }

    catch (const ParseError& e) {
        std::cout << "Parsing error! " << e.what() << std::endl;
    }

    return 0;
}
