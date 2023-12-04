#pragma once

#include "avl.h"
#include "io_kit.h"
#include "token.h"

#include <regex>
#include <stdexcept>
#include <string>
#include <vector>

int numOfLine();

void findError(std::string cur);

std::vector < Token > lex_an(const std::string text);