cmake_minimum_required(VERSION 3.20)
set(CMAKE_CXX_STANDARD 20)
PROJECT(re-test)


add_executable(re-test
        main.cpp
        test.cpp
        test.h
)

target_link_libraries(re-test PRIVATE re)
