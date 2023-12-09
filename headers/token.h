#pragma once

#include <string>

struct Token {
    std::string token;
    int level;

    Token () {}
    Token (std::string token, int level) : token(token), level(level) {}
};
