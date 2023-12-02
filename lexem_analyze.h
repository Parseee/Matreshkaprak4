#pragma once
#include <regex>
#include <stdexcept>
#include <string>
#include <vector>

#include "avl.h"
#include "io_kit.h"

avl *lex_type;

std::vector<std::invalid_argument> errors;
std::vector<std::pair<std::string, int>> lexemes;

int numOfLine() {
  int n = 1;
  for (int ex = 0; ex < lexemes.size(); ++ex) {
    if (lexemes[ex].second == 0)
      ++n;
  }
  return n;
}

void findError(std::string cur) {
  std::smatch match;
	int num_of_line = numOfLine();
  if (regex_search(cur, match, std::regex("[$@%?№]"))) {
    errors.push_back(std::invalid_argument(
        "in line: " + std::to_string(num_of_line) +
        "\nInvalid character in the variable name: " + match[0].str()));
	}
  return;
}

std::vector<std::pair<std::string, int>> lexem_array;/* {
    {"\n", 0},    {"pribav", 1},     {"golova", 1},   {"telo", 1},
    {"zhivi", 1}, {"umri_kogda", 1}, {"idi_poka", 1}, {"ne_stanet", 1},
    {"pishi", 1}, {"sprosi", 1},     {"func", 1},     {"esli", 1},
    {"+", 4},     {"-", 4},          {"*", 4},        {"/", 4},
    {"!", 4},     {"ubav", 4},       {"verni", 4},    {"max", 4},
    {"min", 4},   {"mod", 4},        {"and", 4},      {"or", 4},
    {"not", 4},   {"=", 4},          {"/=", 4},       {">", 4},
    {"<", 4},     {">=", 4},         {"<=", 4},       {"(", 5},
    {")", 5}
};*/

std::vector<std::pair<std::string, int>> lex_an(std::string text) {
  //lexem_array = initialize_lexem_array("materials/lexic.txt");
  for (int i = 0; i < lexem_array.size(); ++i) {
    lex_type->insert(lex_type, lexem_array[i]);
  }

  std::string cur = "";
  for (int i = 0; i < text.size(); ++i) {
    if (text[i] == ' ' || text[i] == ')' || text[i] == '(') {
      if (cur.size() != 0) {

        std::pair<std::string, int> lex;
        lex.first = ((cur == "\n") ? "\\n" : cur);

        if (lex_type->find(lex_type, cur) != -1) {
          lex.second = lex_type->find(lex_type, cur);
        } else {
					if((cur[0] == '\"' || cur[cur.size() - 1] == '\"') && cur[0] != cur[cur.size() - 1]){
						int num_of_line = numOfLine();
						errors.push_back(std::invalid_argument(
							"In line: " + std::to_string(num_of_line) +
							"\nInvalid character: \""));
					}
          if (cur[0] == '\"' || isdigit(cur[0]) || cur[0] == '-' ||
              cur[0] == '+') {
            lex.second = 3;
          } else {
						findError(cur);
            lex.second = 2;
          }
        }

        lexemes.push_back(lex);
      }

      if (text[i] == ')' || text[i] == '(') {
        std::pair<std::string, int> lex;
        std::string s = "";
        s += text[i];
        lex.first = s;
        lex.second = lex_type->find(lex_type, s);
        lexemes.push_back(lex);
      }

      cur = "";
    } else {

      cur += text[i];
    }
  }
  if (cur.size() != 0) {
    std::pair<std::string, int> lex;
    lex.first = ((cur == "\n") ? "\\n" : cur);

    if (lex_type->find(lex_type, cur) != -1) {
      lex.second = lex_type->find(lex_type, cur);
    } else {
			if((cur[0] == '\"' || cur[cur.size() - 1] == '\"') && cur[0] != cur[cur.size() - 1]){
				int num_of_line = numOfLine();
				errors.push_back(std::invalid_argument(
					"in line: " + std::to_string(num_of_line) +
					"\nInvalid character: \""));
			}
      if (cur[0] == '\"' || isdigit(cur[0]) || cur[0] == '-' || cur[0] == '+') {
        lex.second = 3;
      } else {
        lex.second = 2;
      }
    }

    lexemes.push_back(lex);
  }
	if(errors.size() != 0)
		throw errors;
  return lexemes;
}