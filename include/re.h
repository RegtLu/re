//
// Created by Regt on 25-8-11.
//

#ifndef RE_H
#define RE_H

#include <string>
#include <utility>
#include <memory>

#include "../src/nfa2dfa.h"

namespace re {
    class RE {
        std::string re_str;
        std::shared_ptr<DFANode> root;

    public:
        explicit RE(std::string re_str) : re_str(std::move(re_str)) {
        }

        void compile();

        bool match(std::string input);
    };
}


#endif //RE_H
