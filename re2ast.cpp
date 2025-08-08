//
// Created by Regt on 25-8-8.
//

#include <memory>
#include <vector>
#include <ostream>

class RegexNode {
public:
    virtual ~RegexNode() = default;

    virtual std::string print() const = 0;
};


class Char : public RegexNode {
public:
    char value;

    explicit Char(char v) : value(v) {
    }

    std::string print() const override {
        std::string str = "Char(" + std::string(1, value) + ")";
        return str;
    }
};


class Set : public RegexNode {
public:
    std::vector<char> elements;

    explicit Set(std::vector<char> elements) : elements(std::move(elements)) {
    }

    std::string print() const override {
        std::string str = "Set([";
        for (char c: elements) str += c;
        str += "])";
        return str;
    }
};


class Repeat : public RegexNode {
public:
    std::shared_ptr<RegexNode> body;
    int min, max;

    Repeat(std::shared_ptr<RegexNode> body, int min, int max) : body(std::move(body)), min(min), max(max) {
    }

    std::string print() const override {
        std::string str = "Repeat(" + body->print() + ", " + std::to_string(min) + ", " + std::to_string(max) + ")";
        return str;
    }
};


class Star : public RegexNode {
public:
    std::shared_ptr<RegexNode> body;

    explicit Star(std::shared_ptr<RegexNode> body) : body(std::move(body)) {
    }

    std::string print() const override {
        std::string str = "Star(" + body->print() + ")";
        return str;
    }
};


class Concat : public RegexNode {
public:
    std::shared_ptr<RegexNode> left, right;

    Concat(std::shared_ptr<RegexNode> l, std::shared_ptr<RegexNode> r) : left(std::move(l)), right(std::move(r)) {
    }

    std::string print() const override {
        std::string str = "Concat(" + left->print() + ", " + right->print() + ")";
        return str;
    }
};


class Or : public RegexNode {
public:
    std::shared_ptr<RegexNode> left, right;

    Or(std::shared_ptr<RegexNode> l, std::shared_ptr<RegexNode> r) : left(std::move(l)), right(std::move(r)) {
    }

    std::string print() const override {
        std::string str = "Or(" + left->print() + ", " + right->print() + ")";
        return str;
    }
};

class Group : public RegexNode {
public:
    std::shared_ptr<RegexNode> body;

    Group(std::shared_ptr<RegexNode> body) : body(std::move(body)) {
    }

    std::string print() const override {
        std::string str = "Group(" + body->print() + ")";
        return str;
    }
};

class NoneCaptureGroup : public RegexNode {
public:
    std::shared_ptr<RegexNode> body;

    NoneCaptureGroup(std::shared_ptr<RegexNode> body) : body(std::move(body)) {
    }

    std::string print() const override {
        std::string str = "NoneCaptureGroup(" + body->print() + ")";
        return str;
    }
};

class Regex2AST {
    std::string &input;
    int pos = 0;
    char ch;

public:
    explicit Regex2AST(std::string &input) : input(input) {
        next();
    }

    void next() {
        ch = pos < input.size() ? input[pos++] : '\0';
        if (ch == '\\') {
            ch = pos < input.size() ? input[pos++] : '\0';
        }
    }

    std::shared_ptr<RegexNode> parse() {
        return parse_Or();
    }

    std::shared_ptr<RegexNode> parse_Char() {
        char c = ch;
        next();
        return std::make_shared<Char>(c);
    }

    std::shared_ptr<RegexNode> parse_Set() {
        std::vector<char> elements;
        char last = ch;
        next();
        while (ch != ']') {
            last = ch;
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
        return std::make_shared<Set>(elements);
    }

    std::shared_ptr<RegexNode> parse_Repeat(std::shared_ptr<RegexNode> node) {
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

    std::shared_ptr<RegexNode> parse_Star(std::shared_ptr<RegexNode> node) {
        next();
        return std::make_shared<Star>(node);
    }

    std::shared_ptr<RegexNode> parse_Concat() {
        std::shared_ptr<RegexNode> node;
        if (ch == '(') {
            node = parse_Group();
        } else if (ch == '[') {
            node = parse_Set();
        } else {
            node = parse_Char();
        }
        while (ch != '\0' && ch != '|'&& ch != ')') {
            if (ch == '[') {
                std::shared_ptr<RegexNode> node2 = parse_Set();
                node = std::make_shared<Concat>(node, node2);
            } else if (ch == '{') {
                node = parse_Repeat(node);
            } else if (ch == '*') {
                node = parse_Star(node);
            } else if (ch == '(') {
                std::shared_ptr<RegexNode> node2 = parse_Group();
                node = std::make_shared<Concat>(node, node2);
            } else {
                std::shared_ptr<RegexNode> node2 = parse_Char();
                node = std::make_shared<Concat>(node, node2);
            }
        }
        return node;
    }

    std::shared_ptr<RegexNode> parse_Or() {
        std::shared_ptr<RegexNode> node = parse_Concat();
        while (ch == '|') {
            next();
            std::shared_ptr<RegexNode> node2 = parse_Concat();
            node = std::make_shared<Or>(node, node2);
        }
        return node;
    }

    std::shared_ptr<RegexNode> parse_Group() {
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
};
