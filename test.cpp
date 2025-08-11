//
// Created by Regt on 25-8-11.
//

#include <iostream>
#include <string>

#include "re2ast.h"
#include "ast2nfa.h"
#include "nfa2dfa.h"
#include "test.h"

void test_re2ast() {
    std::string re1 = "b[aeiou]bb(?:le){1,3}";
    Regex2AST re2ast1(re1);
    auto x1 = re2ast1.parse();
    std::cout << x1->print() << std::endl;

    std::string re2 = "[b-chm-pP]at|ot";
    Regex2AST re2ast2(re2);
    auto x2 = re2ast2.parse();
    std::cout << x2->print() << std::endl;


    std::string re3 = "gr(a|e)y";
    Regex2AST re2ast3(re3);
    auto x3 = re2ast3.parse();
    std::cout << x3->print() << std::endl;


    std::string re4 = "a[^a-zA-Z0-6]c";
    Regex2AST re2ast4(re4);
    auto x4 = re2ast4.parse();
    std::cout << x4->print() << std::endl;


    std::string re5 = "[cd]+o(es)?";
    Regex2AST re2ast5(re5);
    auto x5 = re2ast5.parse();
    std::cout << x5->print() << std::endl;
}

void test_ast2nfa() {
    std::string re1 = "b[aeiou]bb(?:le){1,3}";
    Regex2AST re2ast1(re1);
    auto x1 = re2ast1.parse();
    AST2NFA ast2nfa1(x1);
    auto g1 = ast2nfa1.build();
    dumpNFA(g1);

    std::string re2 = "[b-chm-pP]at|ot";
    Regex2AST re2ast2(re2);
    auto x2 = re2ast2.parse();
    AST2NFA ast2nfa2(x2);
    auto g2 = ast2nfa2.build();
    dumpNFA(g2);


    std::string re3 = "gr(a|e)y";
    Regex2AST re2ast3(re3);
    auto x3 = re2ast3.parse();
    AST2NFA ast2nfa3(x3);
    auto g3 = ast2nfa3.build();
    dumpNFA(g3);


    std::string re4 = "a[^a-zA-Z0-6]c";
    Regex2AST re2ast4(re4);
    auto x4 = re2ast4.parse();
    AST2NFA ast2nfa4(x4);
    auto g4 = ast2nfa4.build();
    dumpNFA(g4);


    std::string re5 = "[cd]+o(es)?";
    Regex2AST re2ast5(re5);
    auto x5 = re2ast5.parse();
    AST2NFA ast2nfa5(x5);
    auto g5 = ast2nfa5.build();
    dumpNFA(g5);
}


void test_nfa2dfa() {
    std::string re1 = "b[aeiou]bb(?:le){1,3}";
    Regex2AST re2ast1(re1);
    auto x1 = re2ast1.parse();
    AST2NFA ast2nfa1(x1);
    auto g1 = ast2nfa1.build();
    NFA2DFA nfa2dfa1(g1);
    auto d1 = nfa2dfa1.transform();
    dumpDFA(d1);

    std::string re2 = "[b-chm-pP]at|ot";
    Regex2AST re2ast2(re2);
    auto x2 = re2ast2.parse();
    AST2NFA ast2nfa2(x2);
    auto g2 = ast2nfa2.build();
    NFA2DFA nfa2dfa2(g2);
    auto d2 = nfa2dfa2.transform();
    dumpDFA(d2);


    std::string re3 = "gr(a|e)y";
    Regex2AST re2ast3(re3);
    auto x3 = re2ast3.parse();
    AST2NFA ast2nfa3(x3);
    auto g3 = ast2nfa3.build();
    NFA2DFA nfa2dfa3(g3);
    auto d3 = nfa2dfa3.transform();
    dumpDFA(d3);


    std::string re4 = "a[^a-zA-Z0-6]c";
    Regex2AST re2ast4(re4);
    auto x4 = re2ast4.parse();
    AST2NFA ast2nfa4(x4);
    auto g4 = ast2nfa4.build();
    NFA2DFA nfa2dfa4(g4);
    auto d4 = nfa2dfa4.transform();
    dumpDFA(d4);


    std::string re5 = "[cd]+o(es)?";
    Regex2AST re2ast5(re5);
    auto x5 = re2ast5.parse();
    AST2NFA ast2nfa5(x5);
    auto g5 = ast2nfa5.build();
    NFA2DFA nfa2dfa5(g5);
    auto d5 = nfa2dfa5.transform();
    dumpDFA(d5);
}
