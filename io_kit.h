#pragma once

#include <fcntl.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <stdlib.h>
#include <unistd.h>
#include <vector>

std::vector<std::pair<std::string, int>>
initialize_lexem_array(const char *filename) {
  std::vector<std::pair<std::string, int>> lex_array;
  std::ifstream input("materials/lexic.txt");
  lex_array.push_back({"\n", 0});
  std::string name; int priority;
  while (input.good()) {
    input >> name >> priority;
    lex_array.push_back({name, priority});
  }
  return lex_array;
}

std::string get_text(const char *s) {
  int file_descriptor = open(s, O_RDONLY);
  if (file_descriptor == -1) {
    throw std::logic_error("file missing");
  }

  char text[32768];
  int size = read(file_descriptor, text, 32768);
  std::cerr << "size is " << size << '\n';

  return std::string(text);
}