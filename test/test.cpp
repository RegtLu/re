//
// Created by Regt on 25-8-11.
//

#include <iostream>
#include <string>

#include "re.h"
#include "test.h"

void test_re() {
    re::RE re1(R"(a.{2,3}b)");
    re1.compile();
    std::cout<<re1.match_pos("a__b")<<std::endl;
    std::cout<<re1.match_pos("a vb")<<std::endl;
}
