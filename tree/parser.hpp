#ifndef LAB2_PARSER_HPP
#define LAB2_PARSER_HPP

#include <string>
#include <vector>
#include <stdexcept>
#include "ast.hpp"


struct ParseResult {
    AstPtr root;
    int groupCount = 0;
};

class ParseError : public std::runtime_error {
public:
    ParseError(const std::string& msg, size_t pos)
        : std::runtime_error(msg + " (position " + std::to_string(pos) + ")"),
          position(pos) {}
    size_t position;
};

class Parser {
public:
    explicit Parser(std::string src) : s(std::move(src)) {}

    ParseResult parse();

private:
    std::string s;
    size_t pos = 0;
    int groupCounter = 0;
    std::vector<std::pair<int, size_t>> backrefs; // <номер группы, позиция в строке>

    // уровни грамматики
    AstPtr parseAlt();
    AstPtr parseConcat();
    AstPtr parseRepeat();
    AstPtr parseAtom();

    // вспомогательные разборщики
    AstPtr parseCharClass();
    AstPtr parseEscape();
    int parseNumber();

    // мелкие помощники для работы с потоком символов
    [[nodiscard]] bool atEnd() const { return pos >= s.size(); }
    [[nodiscard]] char peek() const { return s[pos]; }
    char get() { return s[pos++]; }
    [[noreturn]] void error(const std::string& msg) const {
        throw ParseError(msg, pos);
    }
};

#endif
