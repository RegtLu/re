cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 20)

add_library(re STATIC
        src/ast2nfa.cpp
        src/nfa2dfa.cpp
        src/re2ast.cpp
        src/re2ast.h
        src/ast2nfa.h
        src/nfa2dfa.h
        src/re.cpp
        include/re.h
)

target_include_directories(re
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_subdirectory(test)