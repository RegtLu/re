//
// Created by Regt on 25-8-11.
//

#include <iostream>
#include <string>

#include "re.h"
#include "test.h"

void test_re() {
    re::RE re1(R"(a{,3}b{2}c{1,}\w\s)");
    re1.compile();
    std::cout<<re1.match("abbccq ")<<std::endl;
    std::cout<<re1.match("aaabbccccccccp\n")<<std::endl;
}
