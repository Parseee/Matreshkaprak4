#pragma once

#include <iostream>
#include <stdexcept>
#include <vector>
#include <fstream>

std::vector < std::pair < std::string, int >>
    initialize_lexem_array(const std::string s);

std::string read_file(std::string path);