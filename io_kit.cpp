#include "headers/io_kit.h"
#include "headers/token.h"

std::vector < Token >
    initialize_lexem_array(const std::string s) {
    std::vector < Token > lex_array;
    std::ifstream input(s);
    lex_array.push_back(Token ("\n", 0));
    std::string name;
    int priority;
    while (input.good()) {
        input >> name >> priority;
        lex_array.push_back(Token(name, priority));
    }
    return lex_array;
}

std::string read_file(std::string path) {
    std::ifstream stream = std::ifstream(path);
    std::string content =
        std::string(std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>());
    return content;
}