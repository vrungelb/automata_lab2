#ifndef LAB2_AST_HPP
#define LAB2_AST_HPP

#include <memory>
#include <vector>

enum class NodeKind {
    Symbol,    // одиночный литерал
    CharClass, // [...]
    Empty,     // #
    Concat,    // r1 r2
    Alt,       // r1 | r2
    Star,      // r*
    Repeat,    // r{x}
    Group,     // (r)
    Backref    // \n
};

struct AstNode {
    NodeKind kind;

    char symbol = '\0';           // для Symbol
    std::vector<char> charset;    // для CharClass, для списка символов внутри [...]
    int count = 0;                // для Repeat
    int groupId = 0;              // для Group и Backref

    std::unique_ptr<AstNode> left; // первый потомок
    std::unique_ptr<AstNode> right; // второй потомок

    explicit AstNode(NodeKind k) : kind(k) {}
};

using AstPtr = std::unique_ptr<AstNode>;

// фабрики
inline AstPtr makeSymbol(char c) {
    auto n = std::make_unique<AstNode>(NodeKind::Symbol);
    n->symbol = c;
    return n;
}

inline AstPtr makeEmpty() {
    return std::make_unique<AstNode>(NodeKind::Empty);
}

inline AstPtr makeCharClass(std::vector<char> chars) {
    auto n = std::make_unique<AstNode>(NodeKind::CharClass);
    n->charset = std::move(chars);
    return n;
}

inline AstPtr makeConcat(AstPtr a, AstPtr b) {
    auto n = std::make_unique<AstNode>(NodeKind::Concat);
    n->left = std::move(a);
    n->right = std::move(b);
    return n;
}

inline AstPtr makeAlt(AstPtr a, AstPtr b) {
    auto n = std::make_unique<AstNode>(NodeKind::Alt);
    n->left = std::move(a);
    n->right = std::move(b);
    return n;
}

inline AstPtr makeStar(AstPtr a) {
    auto n = std::make_unique<AstNode>(NodeKind::Star);
    n->left = std::move(a);
    return n;
}

inline AstPtr makeRepeat(AstPtr a, int count) {
    auto n = std::make_unique<AstNode>(NodeKind::Repeat);
    n->left = std::move(a);
    n->count = count;
    return n;
}

inline AstPtr makeGroup(AstPtr a, int id) {
    auto n = std::make_unique<AstNode>(NodeKind::Group);
    n->left = std::move(a);
    n->groupId = id;
    return n;
}

inline AstPtr makeBackref(int id) {
    auto n = std::make_unique<AstNode>(NodeKind::Backref);
    n->groupId = id;
    return n;
}

#endif
