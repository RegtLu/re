//
// Created by Regt on 25-8-8.
//

#include <algorithm>
#include <memory>
#include <vector>

#include "re2ast.h"

using namespace re;

std::string Empty::print() const {
    std::string str = "Empty()";
    return str;
}


std::string Char::print() const {
    std::string value_c;
    if (value == '\n') {
        value_c = "\\n";
    } else if (value == '\r') {
        value_c = "\\r";
    } else if (value == '\v') {
        value_c = "\\v";
    } else if (value == '\f') {
        value_c = "\\f";
    } else if (value == '\t') {
        value_c = "\\t";
    } else {
        value_c = std::string(1, value);
    }
    std::string str = "Char(" + value_c + ")";
    return str;
}


std::string Set::print() const {
    std::string str = "Set([";
    for (char c: elements) {
        if (c == '\n') {
            str += "\\n";
        } else if (c == '\r') {
            str += "\\r";
        } else if (c == '\v') {
            str += "\\v";
        } else if (c == '\f') {
            str += "\\f";
        } else if (c == '\t') {
            str += "\\t";
        } else {
            str += c;
        }
    }
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
}

std::shared_ptr<RegexNode> Regex2AST::parse() {
    return parse_Or();
}

std::vector<char> Regex2AST::make_complement(std::vector<char> elements) {
    auto s = Sigma;
    std::erase_if(s, [elements](char c) {
        return std::find(elements.begin(), elements.end(), c) != elements.end();
    });
    return s;
}

std::shared_ptr<RegexNode> Regex2AST::parse_Char() {
    char c = ch;
    next();
    if (c == '.') {
        std::vector<char> elements = Sigma;
        return std::make_shared<Set>(elements);
    }
    return std::make_shared<Char>(c);
}

std::shared_ptr<RegexNode> Regex2AST::parse_Set() {
    std::vector<char> elements;
    bool neg = false;
    char last = ch;
    next();
    while (ch != ']') {
        last = ch;
        if (ch == '\\') {
            std::shared_ptr<RegexNode> node = parse_Escape();
            if (auto char_ = std::dynamic_pointer_cast<Char>(node)) {
                elements.push_back(char_->value);
            } else {
                auto set = std::dynamic_pointer_cast<Set>(node);
                elements.insert(elements.end(), set->elements.begin(), set->elements.end());
            }
            continue;
        }
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
        elements = make_complement(elements);
    }
    return std::make_shared<Set>(elements);
}

std::shared_ptr<RegexNode> Regex2AST::parse_Repeat(std::shared_ptr<RegexNode> node) {
    next();
    std::string min, max;
    while (ch != ',' && ch != '}') {
        min += ch;
        next();
    }
    if (ch == '}') {
        next();
        max = min;
        return std::make_shared<Repeat>(node, std::stoi(min), std::stoi(max));
    }
    next();
    while (ch != '}') {
        max += ch;
        next();
    }
    next();
    if (min == "") {
        min = "0";
        return std::make_shared<Repeat>(node, std::stoi(min), std::stoi(max));
    }
    return std::make_shared<Concat>(std::make_shared<Repeat>(node, std::stoi(min), std::stoi(min)),
                                    std::make_shared<Star>(node));
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
    if (ch == '\\') {
        node = parse_Escape();
    } else if (ch == '[') {
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

std::shared_ptr<RegexNode> Regex2AST::parse_Escape() {
    next();
    std::vector<char> elements;
    char ch_;
    if (ch == 'n') {
        ch_ = '\n';
        next();
        return std::make_shared<Char>(ch_);
    }
    if (ch == '\\') {
        ch_ = '\\';
        next();
        return std::make_shared<Char>(ch_);
    }
    if (ch == 't') {
        ch_ = '\t';
        next();
        return std::make_shared<Char>(ch_);
    }
    if (ch == 'r') {
        ch_ = '\r';
        next();
        return std::make_shared<Char>(ch_);
    }
    if (ch == 'v') {
        ch_ = '\v';
        next();
        return std::make_shared<Char>(ch_);
    }
    if (ch == 'f') {
        ch_ = '\f';
        next();
        return std::make_shared<Char>(ch_);
    }
    if (ch == '[') {
        ch_ = '[';
        next();
        return std::make_shared<Char>(ch_);
    }
    if (ch == ']') {
        ch_ = ']';
        next();
        return std::make_shared<Char>(ch_);
    }
    if (ch == '(') {
        ch_ = '(';
        next();
        return std::make_shared<Char>(ch_);
    }
    if (ch == ')') {
        ch_ = ')';
        next();
        return std::make_shared<Char>(ch_);
    }
    if (ch == '{') {
        ch_ = '{';
        next();
        return std::make_shared<Char>(ch_);
    }
    if (ch == '}') {
        ch_ = '}';
        next();
        return std::make_shared<Char>(ch_);
    }
    if (ch == '^') {
        ch_ = '^';
        next();
        return std::make_shared<Char>(ch_);
    }
    if (ch == '.') {
        ch_ = '.';
        next();
        return std::make_shared<Char>(ch_);
    }
    if (ch == '?') {
        ch_ = '?';
        next();
        return std::make_shared<Char>(ch_);
    }
    if (ch == '+') {
        ch_ = '+';
        next();
        return std::make_shared<Char>(ch_);
    }
    if (ch == '*') {
        ch_ = '*';
        next();
        return std::make_shared<Char>(ch_);
    }
    if (ch == '|') {
        ch_ = '|';
        next();
        return std::make_shared<Char>(ch_);
    }
    if (ch == 'd') {
        elements = Digits;
        next();
        return std::make_shared<Set>(elements);
    }
    if (ch == 'D') {
        elements = make_complement(Digits);
        next();
        return std::make_shared<Set>(elements);
    }
    if (ch == 's') {
        elements = Whitespace;
        next();
        return std::make_shared<Set>(elements);
    }
    if (ch == 'S') {
        elements = make_complement(Whitespace);
        next();
        return std::make_shared<Set>(elements);
    }
    if (ch == 'w') {
        elements = Words;
        next();
        return std::make_shared<Set>(elements);
    }
    if (ch == 'W') {
        elements = make_complement(Words);
        next();
        return std::make_shared<Set>(elements);
    }
    return std::make_shared<Char>(ch);
}
