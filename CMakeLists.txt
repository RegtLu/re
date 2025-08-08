cmake_minimum_required(VERSION 3.31)
project(re)

set(CMAKE_CXX_STANDARD 20)

add_executable(re main.cpp
        ast2nfa.cpp
        nfa2dfa.cpp
        sharedefine.cpp
        re2ast.cpp)
