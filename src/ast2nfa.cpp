//
// Created by Regt on 25-8-8.
//

#include <iostream>
#include <map>
#include <utility>
#include <vector>
#include <memory>
#include <sstream>
#include <algorithm>

#include "ast2nfa.h"

using namespace re;

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

Fragment AST2NFA::build_Set(const std::vector<char> &elements) {
    std::shared_ptr<NFANode> s = std::make_shared<NFANode>();
    std::shared_ptr<NFANode> e = std::make_shared<NFANode>();
    for (char x: elements) {
        std::shared_ptr<NFANode> m = std::make_shared<NFANode>();
        s->addEdge(x, m);
        m->addEpsilonEdge(e);
    }
    return {s, e};
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
    Fragment m = _build(std::move(body));
    s->addEpsilonEdge(m.start);
    m.end->addEpsilonEdge(e);
    m.end->addEpsilonEdge(m.start);
    return {s, e};
}

Fragment AST2NFA::build_Concat(std::shared_ptr<RegexNode> left, std::shared_ptr<RegexNode> right) {
    Fragment l = _build(std::move(left));
    Fragment r = _build(std::move(right));
    l.end->addEpsilonEdge(r.start);
    return {l.start, r.end};
};

Fragment AST2NFA::build_Or(std::shared_ptr<RegexNode> left, std::shared_ptr<RegexNode> right) {
    std::shared_ptr<NFANode> s = std::make_shared<NFANode>();
    std::shared_ptr<NFANode> e = std::make_shared<NFANode>();
    Fragment l = _build(std::move(left));
    Fragment r = _build(std::move(right));
    s->addEpsilonEdge(l.start);
    l.end->addEpsilonEdge(e);
    s->addEpsilonEdge(r.start);
    r.end->addEpsilonEdge(e);
    return {s, e};
};

Fragment AST2NFA::build_Group(std::shared_ptr<RegexNode> body) {
    return _build(std::move(body));
};

Fragment AST2NFA::build_NoneCaptureGroup(std::shared_ptr<RegexNode> body) {
    return build_Group(std::move(body));
};
