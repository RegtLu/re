//
// Created by Regt on 25-8-8.
//

#include <vector>

class State {
public:
    std::vector<std::pair<char, State *> > edges;

    State() {
    }

    void add_edge(char c, State *s) {
        edges.emplace_back(std::make_pair(c, s));
    }
};

class Graph {
public:
    State *start;
    State *end;

    Graph(State *start, State *end): start(start), end(end) {
    }
};
