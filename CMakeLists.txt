cmake_minimum_required(VERSION 3.31)
project(re)

set(CMAKE_CXX_STANDARD 20)

add_executable(re main.cpp
        AST2NFA.cpp
        nfa2dfa.cpp
        sharedefine.cpp
        re2ast.cpp
        re2ast.h
        ast2nfa.h)
