//
// Created by Regt on 25-8-8.
//

#include <iostream>
#include <map>
#include <utility>
#include <vector>
#include <memory>
#include <queue>
#include <sstream>
#include <unordered_map>
#include <algorithm>

#include "ast2nfa.h"


void NFANode::addEdge(char c, std::shared_ptr<NFANode> n) {
    edges[c].push_back(n);
}

void NFANode::addEpsilonEdge(std::shared_ptr<NFANode> n) {
    epsilon_edges.emplace_back(n);
}


std::shared_ptr<NFANode> AST2NFA::build() {
    Fragment frag = _build(ast);
    frag.end->isEnd = true;
    return frag.start;
};

Fragment AST2NFA::_build(std::shared_ptr<RegexNode> childAST) {
    if (std::dynamic_pointer_cast<Empty>(childAST)) {
        return build_Empty();
    } else if (auto ch = std::dynamic_pointer_cast<Char>(childAST)) {
        return build_Char(ch->value);
    } else if (auto set = std::dynamic_pointer_cast<Set>(childAST)) {
        return build_Set(set->elements);
    } else if (auto negset = std::dynamic_pointer_cast<NegSet>(childAST)) {
        return build_NegSet(negset->elements);
    } else if (auto repeat = std::dynamic_pointer_cast<Repeat>(childAST)) {
        return build_Repeat(repeat->body, repeat->min, repeat->max);
    } else if (auto star = std::dynamic_pointer_cast<Star>(childAST)) {
        return build_Star(star->body);
    } else if (auto concat = std::dynamic_pointer_cast<Concat>(childAST)) {
        return build_Concat(concat->left, concat->right);
    } else if (auto _or = std::dynamic_pointer_cast<Or>(childAST)) {
        return build_Or(_or->left, _or->right);
    } else if (auto group = std::dynamic_pointer_cast<Group>(childAST)) {
        return build_Group(group->body);
    } else if (auto ncgroup = std::dynamic_pointer_cast<NoneCaptureGroup>(childAST)) {
        return build_NoneCaptureGroup(ncgroup->body);
    } else {
        throw std::runtime_error("Wrong RegexNode");
    }
};

Fragment AST2NFA::build_Empty() {
    std::shared_ptr<NFANode> s = std::make_shared<NFANode>();
    std::shared_ptr<NFANode> e = std::make_shared<NFANode>();
    s->addEpsilonEdge(e);
    return {s, e};
}

Fragment AST2NFA::build_Char(char c) {
    std::shared_ptr<NFANode> s = std::make_shared<NFANode>();
    std::shared_ptr<NFANode> e = std::make_shared<NFANode>();
    s->addEdge(c, e);
    return {s, e};
}

Fragment AST2NFA::build_Set(std::vector<char> elements) {
    std::shared_ptr<NFANode> s = std::make_shared<NFANode>();
    std::shared_ptr<NFANode> e = std::make_shared<NFANode>();
    for (char x: elements) {
        std::shared_ptr<NFANode> m = std::make_shared<NFANode>();
        s->addEdge(x, m);
        m->addEpsilonEdge(e);
    }
    return {s, e};
}

Fragment AST2NFA::build_NegSet(std::vector<char> elements) {
    auto s = Sigma;
    std::erase_if(s, [elements](char c) {
        return std::find(elements.begin(), elements.end(), c) != elements.end();
    });
    return build_Set(s);
}

Fragment AST2NFA::build_Repeat(std::shared_ptr<RegexNode> body, int min, int max) {
    std::shared_ptr<NFANode> s = std::make_shared<NFANode>();
    std::shared_ptr<NFANode> e = std::make_shared<NFANode>();
    Fragment cur = {s, s};
    for (int i = 0; i < min; i++) {
        Fragment m = _build(body);
        cur.end->addEpsilonEdge(m.start);
        cur.end = m.end;
    }
    cur.end->addEpsilonEdge(e);
    for (int i = 0; i < max - min; i++) {
        Fragment m = _build(body);
        cur.end->addEpsilonEdge(m.start);
        cur.end = m.end;
        cur.end->addEpsilonEdge(e);
    }
    return {s, e};
}

Fragment AST2NFA::build_Star(std::shared_ptr<RegexNode> body) {
    std::shared_ptr<NFANode> s = std::make_shared<NFANode>();
    std::shared_ptr<NFANode> e = std::make_shared<NFANode>();
    s->addEpsilonEdge(e);
    Fragment m = _build(body);
    s->addEpsilonEdge(m.start);
    m.end->addEpsilonEdge(e);
    m.end->addEpsilonEdge(m.start);
    return {s, e};
}

Fragment AST2NFA::build_Concat(std::shared_ptr<RegexNode> left, std::shared_ptr<RegexNode> right) {
    Fragment l = _build(left);
    Fragment r = _build(right);
    l.end->addEpsilonEdge(r.start);
    return {l.start, r.end};
};

Fragment AST2NFA::build_Or(std::shared_ptr<RegexNode> left, std::shared_ptr<RegexNode> right) {
    std::shared_ptr<NFANode> s = std::make_shared<NFANode>();
    std::shared_ptr<NFANode> e = std::make_shared<NFANode>();
    Fragment l = _build(left);
    Fragment r = _build(right);
    s->addEpsilonEdge(l.start);
    l.end->addEpsilonEdge(e);
    s->addEpsilonEdge(r.start);
    r.end->addEpsilonEdge(e);
    return {s, e};
};

Fragment AST2NFA::build_Group(std::shared_ptr<RegexNode> body) {
    return _build(body);
};

Fragment AST2NFA::build_NoneCaptureGroup(std::shared_ptr<RegexNode> body) {
    return build_Group(body);
};


static std::string prettyChar(char c) {
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

void dumpNFA(std::shared_ptr<NFANode> &start, std::ostream &os) {
    if (!start) {
        os << "(empty NFA)\n";
        return;
    }

    std::unordered_map<const NFANode *, int> id;
    std::queue<const NFANode *> q;

    auto push = [&](const std::shared_ptr<NFANode> &n) {
        if (n && !id.count(n.get())) {
            id[n.get()] = static_cast<int>(id.size());
            q.push(n.get());
        }
    };

    push(start);

    // BFS 收集所有节点
    std::vector<const NFANode *> order;
    while (!q.empty()) {
        auto u = q.front();
        q.pop();
        order.push_back(u);

        // 遍历字符边
        for (const auto &kv: u->edges) {
            for (const auto &tgt: kv.second) {
                push(tgt);
            }
        }
        // 遍历 ε 边
        for (const auto &e: u->epsilon_edges) {
            push(e);
        }
    }

    // 反查数组：id -> 节点指针
    std::vector<const NFANode *> nodes(id.size(), nullptr);
    for (auto &kv: id) {
        nodes[kv.second] = kv.first;
    }

    os << "NFA: total nodes = " << nodes.size() << "\n";
    for (size_t i = 0; i < nodes.size(); ++i) {
        auto *u = nodes[i];
        os << "  [" << i << "]"
                << (u->isEnd ? " (ACCEPT)" : "") << "\n";

        // 输出字符边
        for (const auto &kv: u->edges) {
            char c = kv.first;
            for (const auto &tgt: kv.second) {
                os << "     -" << prettyChar(c)
                        << "-> [" << id[tgt.get()] << "]\n";
            }
        }
        // 输出 ε 边
        for (const auto &e: u->epsilon_edges) {
            os << "     -Epsilon-> [" << id[e.get()] << "]\n";
        }
    }
}
