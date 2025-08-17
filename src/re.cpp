//
// Created by Regt on 25-8-11.
//

#include <memory>

#include "re2ast.h"
#include "ast2nfa.h"
#include "nfa2dfa.h"
#include "re.h"

using namespace re;

void RE::compile() {
    Regex2AST re2ast(re_str);
    AST2NFA ast2nfa(re2ast.parse());
    NFA2DFA nfa2dfa(ast2nfa.build());
    root = nfa2dfa.transform();
}

bool RE::match(std::string input) {
    int pos = 0;
    char ch = input[pos];
    auto node = root;
    while (node->edges.contains(ch) && pos < input.length()) {
        node = node->edges.find(ch)->second;
        pos++;
        ch = input[pos];
    }
    if (node->isEnd) {
        return true;
    } else {
        return false;
    }
}
