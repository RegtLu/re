//
// Created by Regt on 25-8-11.
//
#ifndef RE2AST_H
#define RE2AST_H

#include <algorithm>
#include <memory>
#include <vector>
#include <ostream>

class RegexNode {
public:
    virtual ~RegexNode() = default;

    virtual std::string print() const = 0;
};

class Empty : public RegexNode {
public:
    explicit Empty() {
    }

    std::string print() const override;
};

class Char : public RegexNode {
public:
    char value;

    explicit Char(char v) : value(v) {
    }

    std::string print() const override;
};

class Set : public RegexNode {
public:
    std::vector<char> elements;

    explicit Set(std::vector<char> elements) : elements(std::move(elements)) {
    }

    std::string print() const override;
};


class NegSet : public RegexNode {
public:
    std::vector<char> elements;

    explicit NegSet(std::vector<char> elements) : elements(std::move(elements)) {
    }

    std::string print() const override;
};

class Repeat : public RegexNode {
public:
    std::shared_ptr<RegexNode> body;
    int min, max;

    Repeat(std::shared_ptr<RegexNode> body, int min, int max) : body(std::move(body)), min(min), max(max) {
    }

    std::string print() const override;
};

class Star : public RegexNode {
public:
    std::shared_ptr<RegexNode> body;

    explicit Star(std::shared_ptr<RegexNode> body) : body(std::move(body)) {
    }

    std::string print() const override;
};

class Concat : public RegexNode {
public:
    std::shared_ptr<RegexNode> left, right;

    Concat(std::shared_ptr<RegexNode> l, std::shared_ptr<RegexNode> r) : left(std::move(l)), right(std::move(r)) {
    }

    std::string print() const override;
};

class Or : public RegexNode {
public:
    std::shared_ptr<RegexNode> left, right;

    Or(std::shared_ptr<RegexNode> l, std::shared_ptr<RegexNode> r) : left(std::move(l)), right(std::move(r)) {
    }

    std::string print() const override;
};

class Group : public RegexNode {
public:
    std::shared_ptr<RegexNode> body;

    Group(std::shared_ptr<RegexNode> body) : body(std::move(body)) {
    }

    std::string print() const override;
};

class NoneCaptureGroup : public RegexNode {
public:
    std::shared_ptr<RegexNode> body;

    NoneCaptureGroup(std::shared_ptr<RegexNode> body) : body(std::move(body)) {
    }

    std::string print() const override;
};

class Regex2AST {
    std::string &input;
    int pos = 0;
    char ch;

    std::shared_ptr<RegexNode> parse_Char();

    std::shared_ptr<RegexNode> parse_Set();

    std::shared_ptr<RegexNode> parse_Repeat(std::shared_ptr<RegexNode> node);

    std::shared_ptr<RegexNode> parse_Star(std::shared_ptr<RegexNode> node);

    std::shared_ptr<RegexNode> parse_Concat();

    std::shared_ptr<RegexNode> parse_Atom();

    std::shared_ptr<RegexNode> parse_Or();

    std::shared_ptr<RegexNode> parse_Group();

    std::shared_ptr<RegexNode> parse_Plus(std::shared_ptr<RegexNode> node);

    std::shared_ptr<RegexNode> parse_Qmark(std::shared_ptr<RegexNode> node);

public:
    explicit Regex2AST(std::string &input) : input(input) {
        next();
    }

    void next();

    std::shared_ptr<RegexNode> parse();
};


#endif //RE2AST_H
