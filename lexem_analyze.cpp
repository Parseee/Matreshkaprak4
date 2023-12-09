#include "headers/lexem_analyze.h"

avl * lex_type;

std::vector < std::invalid_argument > errors;
std::vector < Token > lexemes;

int numOfLine() {
    int n = 1;
    for (int ex = 0; ex < lexemes.size(); ++ex) {
        if (lexemes[ex].level == 0)
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

std::vector < Token > lexem_array;
/* {
    {"\n", 0},    {"pribav", 1},     {"golova", 1},   {"telo", 1},
    {"zhivi", 1}, {"umri_kogda", 1}, {"idi_poka", 1}, {"ne_stanet", 1},
    {"pishi", 1}, {"sprosi", 1},     {"func", 1},     {"esli", 1},
    {"+", 4},     {"-", 4},          {"*", 4},        {"/", 4},
    {"!", 4},     {"ubav", 4},       {"verni", 4},    {"max", 4},
    {"min", 4},   {"mod", 4},        {"and", 4},      {"or", 4},
    {"not", 4},   {"=", 4},          {"/=", 4},       {">", 4},
    {"<", 4},     {">=", 4},         {"<=", 4},       {"(", 5},
    {")", 5}
}; */

std::vector < Token > lex_an(const std::string text) {
    lexem_array = initialize_lexem_array("materials/lexic.txt");
    for (int i = 0; i < lexem_array.size(); ++i) {
        lex_type -> insert(lex_type, {lexem_array[i].token, lexem_array[i].level});
    }

    std::string cur = "";
    for (int i = 0; i < text.size(); ++i) {
        if (text[i] == ' ' || text[i] == ')' || text[i] == '(') {
            if (cur.size() != 0) {

                Token lex;
                lex.token = ((cur == "\n") ? "\\n" : cur);

                if (lex_type -> find(lex_type, cur) != -1) {
                    lex.level = lex_type -> find(lex_type, cur);
                } else {
                    if ((cur[0] == '\"' || cur[cur.size() - 1] == '\"') && cur[0] != cur[cur.size() - 1]) {
                        int num_of_line = numOfLine();
                        errors.push_back(std::invalid_argument(
                            "In line: " + std::to_string(num_of_line) +
                            "\nInvalid character: \""));
                    }
                    if (cur[0] == '\"' || isdigit(cur[0]) || cur[0] == '-' ||
                        cur[0] == '+') {
                        lex.level = 3;
                    } else {
                        findError(cur);
                        lex.level = 2;
                    }
                }

                lexemes.push_back(lex);
            }

            if (text[i] == ')' || text[i] == '(') {
                Token lex;
                std::string s = "";
                s += text[i];
                lex.token = s;
                lex.level = lex_type -> find(lex_type, s);
                lexemes.push_back(lex);
            }

            cur = "";
        } else {

            cur += text[i];
        }
    }
    if (cur.size() != 0) {
        Token lex;
        lex.token = ((cur == "\n") ? "\\n" : cur);

        if (lex_type -> find(lex_type, cur) != -1) {
            lex.level = lex_type -> find(lex_type, cur);
        } else {
            if ((cur[0] == '\"' || cur[cur.size() - 1] == '\"') && cur.size() > 1 && cur[0] != cur[cur.size() - 1]) {
                int num_of_line = numOfLine();
                errors.push_back(std::invalid_argument(
                    "in line: " + std::to_string(num_of_line) +
                    "\nInvalid character: \""));
            }
            if (cur[0] == '\"' || isdigit(cur[0]) || cur[0] == '-' || cur[0] == '+') {
                lex.level = 3;
            } else {
                lex.level = 2;
            }
        }

        lexemes.push_back(lex);
    }
    if (errors.size() != 0)
        throw errors;
    return lexemes;
}