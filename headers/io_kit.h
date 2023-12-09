#pragma once

#include "token.h"

#include <iostream>
#include <stdexcept>
#include <vector>
#include <fstream>

std::vector < Token >
    initialize_lexem_array(const std::string s);

std::string read_file(std::string path);