#include "treedot.hpp"
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <cstdint>

namespace {

    // ф-ия, экранирующая символы
    std::string esc(const std::string& s) {
    std::string out;
    for (char c : s) {
        if (c == '"' || c == '\\') out += '\\';
        out += c;
    }
    return out;
}

    std::string labelOf(const AstNode* n) {
    switch (n->kind) {
        case NodeKind::Symbol:    return esc(std::string(1, n->symbol));
        case NodeKind::Empty:     return "ε";
        case NodeKind::CharClass: {
            std::string s = "[";
            for (char c : n->charset) s += c;
            s += "]";
            return esc(s);
        }
        case NodeKind::Concat:    return "·";
        case NodeKind::Alt:       return "|";
        case NodeKind::Star:      return "*";
        case NodeKind::Repeat:    return "{" + std::to_string(n->count) + "}";
        case NodeKind::Group:     return "( ) #" + std::to_string(n->groupId);
        case NodeKind::Backref:   return esc("\\" + std::to_string(n->groupId));
    }
    return "?";
}

    void writeNode(const AstNode* n, std::ostream& out) {
    if (!n) return;
    auto id = reinterpret_cast<uintptr_t>(n);
    out << "  node_" << id << " [label=\"" << labelOf(n) << "\"];\n";
    if (n->left) {
        out << "  node_" << id << " -> node_"
            << reinterpret_cast<uintptr_t>(n->left.get()) << ";\n";
        writeNode(n->left.get(), out);
    }
    if (n->right) {
        out << "  node_" << id << " -> node_"
            << reinterpret_cast<uintptr_t>(n->right.get()) << ";\n";
        writeNode(n->right.get(), out);
    }
}

} // namespace

void writeDotFile(const AstNode* root, const std::string& dotFilename) {
    std::ofstream out(dotFilename);
    out << "digraph AST {\n";
    out << "  rankdir=TB;\n";
    out << "  ordering=out;\n";                        // сохраняем порядок: левый потомок слева
    out << "  node [shape=circle, fontname=\"DejaVu Sans\"];\n";
    writeNode(root, out);
    out << "}\n";
}

bool renderTree(const AstNode* root, const std::string& filename) {
    const std::string dot = filename + ".dot";
    const std::string png = filename + ".png";
    writeDotFile(root, dot);
    const std::string cmd = "dot -Tpng \"" + dot + "\" -o \"" + png + "\"";
    return std::system(cmd.c_str()) == 0;
}

// текстовый вывод дерева в консоль
static const char* kindName(NodeKind k) {
    switch (k) {
        case NodeKind::Symbol:    return "Symbol";
        case NodeKind::CharClass: return "CharClass";
        case NodeKind::Empty:     return "Empty";
        case NodeKind::Concat:    return "Concat";
        case NodeKind::Alt:       return "Alt";
        case NodeKind::Star:      return "Star";
        case NodeKind::Repeat:    return "Repeat";
        case NodeKind::Group:     return "Group";
        case NodeKind::Backref:   return "Backref";
    }
    return "?";
}

void printAst(const AstNode* n, int depth) {
    if (!n) return;
    std::cout << std::string(depth * 2, ' ') << kindName(n->kind);
    switch (n->kind) {
        case NodeKind::Symbol:  std::cout << " '" << n->symbol << "'"; break;
        case NodeKind::CharClass: {
            std::cout << " [";
            for (char c : n->charset) std::cout << c;
            std::cout << "]";
            break;
        }
        case NodeKind::Repeat:  std::cout << " {" << n->count << "}"; break;
        case NodeKind::Group:   std::cout << " #" << n->groupId; break;
        case NodeKind::Backref: std::cout << " \\" << n->groupId; break;
        default: break;
    }
    std::cout << "\n";
    printAst(n->left.get(), depth + 1);
    printAst(n->right.get(), depth + 1);
}
