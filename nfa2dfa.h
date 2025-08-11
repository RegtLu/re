//
// Created by Regt on 25-8-11.
//

#ifndef NFA2DFA_H
#define NFA2DFA_H

#include <set>
#include <utility>

#include "ast2nfa.h"

class DFANode {
public:
    bool isEnd = false;
    std::map<char, std::shared_ptr<DFANode> > edges;

    void addEdge(char c, std::shared_ptr<DFANode> n);
};


class NFA2DFA {
    std::shared_ptr<NFANode> nfa;

    std::map<std::set<std::shared_ptr<NFANode> >, std::shared_ptr<DFANode> > cache;

    std::set<std::shared_ptr<NFANode> > mergeEpsilon(std::set<std::shared_ptr<NFANode> > nodes);

public:
    NFA2DFA(std::shared_ptr<NFANode> nfa) : nfa(std::move(nfa)) {
    };

    std::shared_ptr<DFANode> transform();

    std::shared_ptr<DFANode> _transform(std::set<std::shared_ptr<NFANode> > nodes);
};

std::string prettyChar(char c);

void dumpDFA(std::shared_ptr<DFANode> &start, std::ostream &os = std::cout);

#endif //NFA2DFA_H
