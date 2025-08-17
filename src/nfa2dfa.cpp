//
// Created by Regt on 25-8-8.
//

#include "nfa2dfa.h"

#include <queue>
#include <set>
#include <stack>
#include <utility>

using namespace re;

void DFANode::addEdge(char c, std::shared_ptr<DFANode> n) {
    edges[c] = std::move(n);
}

std::set<std::shared_ptr<NFANode> > NFA2DFA::mergeEpsilon(std::set<std::shared_ptr<NFANode> > nodes) {
    std::stack<std::shared_ptr<NFANode> > nodeStack;
    std::set<std::shared_ptr<NFANode> > closure = nodes;
    for (auto node: nodes) {
        nodeStack.push(node);
    }
    while (!nodeStack.empty()) {
        std::shared_ptr<NFANode> node = nodeStack.top();
        nodeStack.pop();
        for (std::shared_ptr<NFANode> child: node->epsilon_edges) {
            auto [it, inserted] = closure.insert(child);
            if (inserted) {
                nodeStack.push(child);
            }
        }
    }
    return closure;
}

std::shared_ptr<DFANode> NFA2DFA::transform() {
    std::set<std::shared_ptr<NFANode> > start = {nfa};
    return _transform(start);
}

std::shared_ptr<DFANode> NFA2DFA::_transform(std::set<std::shared_ptr<NFANode> > nodes) {
    std::set<std::shared_ptr<NFANode> > closure = mergeEpsilon(std::move(nodes));
    if (auto it = cache.find(closure); it != cache.end()) {
        return cache[closure];
    }
    std::shared_ptr<DFANode> dfaNode = std::make_shared<DFANode>();
    cache[closure] = dfaNode;
    for (auto node: closure) {
        if (node->isEnd) {
            dfaNode->isEnd = true;
            break;
        }
    }
    std::set<char> move;
    for (auto node: closure) {
        for (auto edge: node->edges) {
            move.insert(edge.first);
        }
    }
    for (char c: move) {
        std::set<std::shared_ptr<NFANode> > moveSet;
        for (auto node: closure) {
            for (auto edge: node->edges[c]) {
                moveSet.insert(edge);
            }
        }
        if (moveSet.empty()) {
            continue;
        }
        dfaNode->addEdge(c, _transform(moveSet));
    }
    return dfaNode;
}
