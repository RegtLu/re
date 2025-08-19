//
// Created by Regt on 25-8-11.
//

#ifndef RE_H
#define RE_H

#include <string>
#include <utility>
#include <memory>

namespace re {
    class DFANode;
    class RE {
        std::string re_str;
        std::shared_ptr<DFANode> root;

    public:
        explicit RE(std::string re_str) : re_str(std::move(re_str)) {
        }

        void compile();

        int match_pos(std::string input);

        bool match(std::string input);
    };
}


#endif //RE_H
