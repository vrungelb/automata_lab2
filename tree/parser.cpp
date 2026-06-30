#include "parser.hpp"
#include <set>
#include <cctype>


ParseResult Parser::parse() {
    pos = 0;            // текущая позиция в строке
    groupCounter = 0;   // счётчик групп захвата
    backrefs.clear();   // список всех обратных ссылок

    AstPtr root = parseAlt();

    if (!atEnd()) {
        error(std::string("unexpected symbol found: '") + peek() + "'");
    }

    for (auto [n, p] : backrefs) {
        if (n < 1 || n > groupCounter)
            throw ParseError("Reference \\" + std::to_string(n) +
                             " to non-existent group!", p);
    }


    return { std::move(root), groupCounter };
}

AstPtr Parser::parseAlt() {
    AstPtr left = parseConcat();
    while (!atEnd() && peek() == '|') {
        get();
        AstPtr right = parseConcat();
        left = makeAlt(std::move(left), std::move(right));
    }
    return left;
}

AstPtr Parser::parseConcat() {
    if (atEnd() || peek() == '|' || peek() == ')') {
        return makeEmpty();
    }
    AstPtr left = parseRepeat();
    while (!atEnd() && peek() != '|' && peek() != ')') {
        AstPtr right = parseRepeat();
        left = makeConcat(std::move(left), std::move(right));
    }
    return left;
}

AstPtr Parser::parseRepeat() {
    AstPtr node = parseAtom();
    while (!atEnd() && (peek() == '*' || peek() == '{')) {
        if (peek() == '*') {
            get();
            node = makeStar(std::move(node));
        } else {                     // '{x}'
            get();                   // '{'
            int count = parseNumber();
            if (atEnd() || peek() != '}')
                error("expected '}'");
            get();                   // '}'
            node = makeRepeat(std::move(node), count);
        }
    }
    return node;
}

AstPtr Parser::parseAtom() {
    if (atEnd())
        error("expected symbol, but EOF");

    switch (char c = peek()) {
        case '(': {
            get();
            int id = ++groupCounter;
            AstPtr inner = parseAlt();
            if (atEnd() || peek() != ')')
                error("незакрытая '('");
            get();                       // ')'
            return makeGroup(std::move(inner), id);
        }
        case '[':
            return parseCharClass();
        case '#':
            get();
            return makeEmpty();
        case '\\':
            return parseEscape();

        case ')':  error("Unexpected ')'");
        case '*':  error("Nothing to repeat before '*'");
        case '{':  error("Nothing to repeat before '{'");
        case ']':  error("Unexpected ']'");
        case '}':  error("Unexpected '}'");

        default:
            get();
            return makeSymbol(c);
    }
}

AstPtr Parser::parseCharClass() {
    get();                               // кушаем '['
    std::set<char> members;
    while (!atEnd() && peek() != ']') {
        char c = get();
        if (c == '\\') {
            if (atEnd()) error("'\\' in the EOF");
            c = get();
        }
        members.insert(c);
    }
    if (atEnd())
        error("незакрытая '['");
    get();                               // кушаем ']'

    if (members.empty())
        return makeEmpty();              // получили []

    return makeCharClass(std::vector<char>(members.begin(), members.end()));
}

AstPtr Parser::parseEscape() {
    size_t escPos = pos;
    get();                               // '\'
    if (atEnd())
        error("'\\' в конце строки");

    if (std::isdigit(static_cast<unsigned char>(peek()))) {
        int n = parseNumber();
        backrefs.emplace_back(n, escPos);   // запоминаем ссылку и ее позицию
        return makeBackref(n);
    }

    char c = get();                               // если после \ идёт не число, то значит
    return makeSymbol(c);                         // делаем просто узел
}

int Parser::parseNumber() {
    if (atEnd() || !std::isdigit(static_cast<unsigned char>(peek())))
        error("integer expected");
    int n = 0;
    while (!atEnd() && std::isdigit(static_cast<unsigned char>(peek()))) {
        n = n * 10 + (get() - '0');
    }
    return n;
}
