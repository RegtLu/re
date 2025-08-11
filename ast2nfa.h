//
// Created by Regt on 25-8-11.
//

#ifndef AST2NFA_H
#define AST2NFA_H

#include <map>
#include <utility>
#include <vector>
#include <memory>
#include <algorithm>

#include "re2ast.h"

inline std::vector<char> Sigma = {
    ' ', '!', '"', '#', '$', '%', '&', '\'', '(', ')', '*', '+', ',', '-', '.', '/', '0', '1', '2', '3', '4', '5', '6',
    '7', '8', '9', ':', ';', '<', '=', '>', '?', '@', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
    'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '[', '\\', ']', '^', '_', '`', 'a', 'b', 'c', 'd',
    'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '{',
    '|', '}', '~'
};

class NFANode {
public:
    bool isEnd = false;
    std::map<char, std::vector<std::shared_ptr<NFANode> > > edges;
    std::vector<std::shared_ptr<NFANode> > epsilon_edges;

    void addEdge(char c, std::shared_ptr<NFANode> n);

    void addEpsilonEdge(std::shared_ptr<NFANode> n);
};

struct Fragment {
    std::shared_ptr<NFANode> start;
    std::shared_ptr<NFANode> end;
};

class AST2NFA {
    std::shared_ptr<RegexNode> ast;

    Fragment _build(std::shared_ptr<RegexNode> childAST);

    Fragment build_Empty();

    Fragment build_Char(char c);

    Fragment build_Set(std::vector<char> elements);

    Fragment build_NegSet(std::vector<char> elements);

    Fragment build_Repeat(std::shared_ptr<RegexNode> body, int min, int max);

    Fragment build_Star(std::shared_ptr<RegexNode> body);

    Fragment build_Concat(std::shared_ptr<RegexNode> left, std::shared_ptr<RegexNode> right);

    Fragment build_Or(std::shared_ptr<RegexNode> left, std::shared_ptr<RegexNode> right);

    Fragment build_Group(std::shared_ptr<RegexNode> body);

    Fragment build_NoneCaptureGroup(std::shared_ptr<RegexNode> body);

public:
    explicit AST2NFA(std::shared_ptr<RegexNode> ast) : ast(std::move(ast)) {
    }

    std::shared_ptr<NFANode> build();
};

static std::string prettyChar(char c);

void dumpNFA(std::shared_ptr<NFANode> &start, std::ostream &os = std::cout);


#endif //AST2NFA_H
