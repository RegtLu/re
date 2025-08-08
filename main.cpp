#include <iostream>
#include <string>

#include "re2ast.cpp"

int main() {
    std::string re = "b[aeiou]bb(?:le){1,3}";
    Regex2AST re2ast(re);
    auto x = re2ast.parse();
    std::cout<<x->print()<<std::endl;
}
