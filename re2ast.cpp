//
// Created by Regt on 25-8-8.
//

#include <algorithm>
#include <memory>
#include <vector>
#include <ostream>

#include "re2ast.h"


std::string Empty::print() const {
    std::string str = "Empty()";
    return str;
}


std::string Char::print() const {
    std::string str = "Char(" + std::string(1, value) + ")";
    return str;
}


std::string Set::print() const {
    std::string str = "Set([";
    for (char c: elements) str += c;
    str += "])";
    return str;
}


std::string NegSet::print() const {
    std::string str = "NegSet([^";
    for (char c: elements) str += c;
    str += "])";
    return str;
}


std::string Repeat::print() const {
    std::string str = "Repeat(" + body->print() + ", " + std::to_string(min) + ", " + std::to_string(max) + ")";
    return str;
}


std::string Star::print() const {
    std::string str = "Star(" + body->print() + ")";
    return str;
}


std::string Concat::print() const {
    std::string str = "Concat(" + left->print() + ", " + right->print() + ")";
    return str;
}


std::string Or::print() const {
    std::string str = "Or(" + left->print() + ", " + right->print() + ")";
    return str;
}


std::string Group::print() const {
    std::string str = "Group(" + body->print() + ")";
    return str;
}

std::string NoneCaptureGroup::print() const {
    std::string str = "NoneCaptureGroup(" + body->print() + ")";
    return str;
}


void Regex2AST::next() {
    ch = pos < input.size() ? input[pos++] : '\0';
    if (ch == '\\') {
        ch = pos < input.size() ? input[pos++] : '\0';
    }
}

std::shared_ptr<RegexNode> Regex2AST::parse() {
    return parse_Or();
}

std::shared_ptr<RegexNode> Regex2AST::parse_Char() {
    char c = ch;
    next();
    return std::make_shared<Char>(c);
}

std::shared_ptr<RegexNode> Regex2AST::parse_Set() {
    std::vector<char> elements;
    bool neg = false;
    char last = ch;
    next();
    while (ch != ']') {
        last = ch;
        if (ch == '^') {
            if (elements.empty()) {
                neg = true;
                next();
            }
        }
        if (ch == '-') {
            if (elements.empty()) {
                elements.push_back(ch);
                next();
            } else {
                char l = elements.back();
                elements.pop_back();
                next();
                for (; l <= ch; l++) {
                    elements.push_back(l);
                }
            }
        } else {
            elements.push_back(ch);
            next();
        }
    }
    if (last == '-') { elements.push_back('-'); }
    next();
    std::sort(elements.begin(), elements.end());
    auto idx = std::unique(elements.begin(), elements.end());
    elements.erase(idx, elements.end());
    if (neg) {
        return std::make_shared<NegSet>(elements);
    } else {
        return std::make_shared<Set>(elements);
    }
}

std::shared_ptr<RegexNode> Regex2AST::parse_Repeat(std::shared_ptr<RegexNode> node) {
    next();
    std::string min, max;
    while (ch != ',') {
        min += ch;
        next();
    }
    next();
    while (ch != '}') {
        max += ch;
        next();
    }
    next();
    return std::make_shared<Repeat>(node, std::stoi(min), std::stoi(max));
}

std::shared_ptr<RegexNode> Regex2AST::parse_Star(std::shared_ptr<RegexNode> node) {
    next();
    return std::make_shared<Star>(node);
}

std::shared_ptr<RegexNode> Regex2AST::parse_Concat() {
    std::shared_ptr<RegexNode> node;
    while (ch != '\0' && ch != '|' && ch != ')') {
        std::shared_ptr<RegexNode> atom = parse_Atom();
        if (!node) {
            node = atom;
        } else {
            node = std::make_shared<Concat>(node, atom);
        }
    }
    return node;
}

std::shared_ptr<RegexNode> Regex2AST::parse_Atom() {
    std::shared_ptr<RegexNode> node;
    if (ch == '[') {
        node = parse_Set();
    } else if (ch == '(') {
        node = parse_Group();
    } else if (ch == '{') {
        throw std::runtime_error("Wrong Atom");
    } else if (ch == '*') {
        throw std::runtime_error("Wrong Atom");
    } else if (ch == '+') {
        throw std::runtime_error("Wrong Atom");
    } else if (ch == '?') {
        throw std::runtime_error("Wrong Atom");
    } else {
        node = parse_Char();
    }
    if (ch == '{') {
        node = parse_Repeat(node);
    } else if (ch == '*') {
        node = parse_Star(node);
    } else if (ch == '+') {
        node = parse_Plus(node);
    } else if (ch == '?') {
        node = parse_Qmark(node);
    }
    return node;
}

std::shared_ptr<RegexNode> Regex2AST::parse_Or() {
    std::shared_ptr<RegexNode> node = parse_Concat();
    while (ch == '|') {
        next();
        std::shared_ptr<RegexNode> node2 = parse_Concat();
        node = std::make_shared<Or>(node, node2);
    }
    return node;
}

std::shared_ptr<RegexNode> Regex2AST::parse_Group() {
    next();
    if (ch == '?') {
        next();
        next(); // Suppose ?: always come together
        std::shared_ptr<RegexNode> node = parse_Or();
        next();
        return std::make_shared<NoneCaptureGroup>(node);
    } else {
        std::shared_ptr<RegexNode> node = parse_Or();
        next();
        return std::make_shared<Group>(node);
    }
}

std::shared_ptr<RegexNode> Regex2AST::parse_Plus(std::shared_ptr<RegexNode> node) {
    next();
    return std::make_shared<Concat>(node, std::make_shared<Star>(node));
}

std::shared_ptr<RegexNode> Regex2AST::parse_Qmark(std::shared_ptr<RegexNode> node) {
    next();
    return std::make_shared<Or>(std::make_shared<Empty>(), node);
}
