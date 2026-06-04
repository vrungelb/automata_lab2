#ifndef LAB2_TREEDOT_HPP
#define LAB2_TREEDOT_HPP

#include <string>
#include "ast.hpp"


void printAst(const AstNode* root, int depth = 0);

bool renderTree(const AstNode* root, const std::string& filename = "tree");

void writeDotFile(const AstNode* root, const std::string& dotFilename);

#endif
