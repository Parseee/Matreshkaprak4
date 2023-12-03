#pragma once
#include <regex>
#include <stdexcept>
#include <string>
#include <vector>
#include "avl.h"
#include "io_kit.h"


int numOfLine();

void findError(std::string cur);

std::vector < std::pair < std::string, int >> lex_an(std::string text);