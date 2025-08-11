//
// Created by Regt on 25-8-8.
//

#include "nfa2dfa.h"

#include <queue>
#include <set>
#include <sstream>
#include <stack>
#include <unordered_map>

void DFANode::addEdge(char c, std::shared_ptr<DFANode> n) {
    edges[c] = n;
}

std::set<std::shared_ptr<NFANode> > NFA2DFA::mergeEpsilon(std::set<std::shared_ptr<NFANode> > nodes) {
    std::stack<std::shared_ptr<NFANode> > nodeStack;
    std::set<std::shared_ptr<NFANode> > closure = nodes;
    for (auto node: nodes) {
        nodeStack.push(node);
    }
    std::shared_ptr<NFANode> node;
    while (!nodeStack.empty()) {
        node = nodeStack.top();
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
    std::set<std::shared_ptr<NFANode> > closure = mergeEpsilon(nodes);
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

std::string prettyChar(char c) {
    // 把不可见字符与特殊符号转义一下
    switch (c) {
        case '\n': return "\\n";
        case '\r': return "\\r";
        case '\t': return "\\t";
        case '\\': return "\\\\";
        case '"': return "\\\"";
        default:
            if (std::isprint(static_cast<unsigned char>(c))) {
                return std::string(1, c);
            } else {
                std::ostringstream oss;
                oss << "\\x" << std::hex << std::uppercase << (int) (unsigned char) c;
                return oss.str();
            }
    }
}

void dumpDFA(std::shared_ptr<DFANode> &start, std::ostream &os) {
    if (!start) {
        os << "(empty DFA)\n";
        return;
    }

    std::unordered_map<const DFANode *, int> id;
    std::queue<const DFANode *> q;

    auto push = [&](const std::shared_ptr<DFANode> &n) {
        if (n && !id.count(n.get())) {
            id[n.get()] = static_cast<int>(id.size());
            q.push(n.get());
        }
    };

    push(start);

    // BFS 收集所有节点
    std::vector<const DFANode *> order;
    while (!q.empty()) {
        auto u = q.front();
        q.pop();
        order.push_back(u);

        // 遍历字符边
        for (const auto &kv : u->edges) {
            push(kv.second);
        }
    }

    // 反查数组：id -> 节点指针
    std::vector<const DFANode *> nodes(id.size(), nullptr);
    for (auto &kv : id) {
        nodes[kv.second] = kv.first;
    }

    os << "DFA: total nodes = " << nodes.size() << "\n";
    for (size_t i = 0; i < nodes.size(); ++i) {
        auto *u = nodes[i];
        os << "  [" << i << "]"
           << (u->isEnd ? " (ACCEPT)" : "") << "\n";

        // 输出字符边
        for (const auto &kv : u->edges) {
            char c = kv.first;
            auto tgt = kv.second.get();
            os << "     -" << prettyChar(c)
               << "-> [" << id[tgt] << "]\n";
        }
    }
}
