#include "headers/lexem_analyze.h"

avl *lex_type;

std::vector<std::invalid_argument> errors;
std::vector<Token> lexemes;

int numOfLine()
{
    int n = 1;
    for (int ex = 0; ex < lexemes.size(); ++ex)
    {
        if (lexemes[ex].level == 0)
            ++n;
    }
    return n;
}

void findError(std::string cur)
{
    std::smatch match;
    int num_of_line = numOfLine();
    if (regex_search(cur, match, std::regex("[$@%?№]")))
    {
        errors.push_back(std::invalid_argument(
            "in line: " + std::to_string(num_of_line) +
            "\nInvalid character in the variable name: " + match[0].str()));
    }
    return;
}

std::vector<Token> lexem_array;

void add(std::string &cur, char c)
{
    if (c == '\n')
        lexemes.push_back(Token("\\n",  5));
    else
        cur += c;
}
std::vector<Token> lex_an(const std::string text)
{
    lexem_array = initialize_lexem_array("../materials/lexic.txt");
    for (int i = 0; i < lexem_array.size(); ++i)
    {
        lex_type->insert(lex_type, {lexem_array[i].token, lexem_array[i].level});
    }

    std::string cur = "";
    for (int i = 0; i < text.size(); ++i)
    {
        if (text[i] == '\"')
        {
            add(cur, text[i++]);
            while (text[i] != '\"' && i < text.size())
            {
                add(cur, text[i++]);
            }
            if (i < text.size())
            {
                add(cur, text[i++]);
            }
            else
            {
                int num_of_line = numOfLine();
                errors.push_back(std::invalid_argument(
                    "In line: " + std::to_string(num_of_line) +
                    "\nInvalid character: \""));
            }
            Token lex;
            lex.token = cur;
            lex.level = 3;
            lexemes.push_back(lex);
            if (text[i] == ')' || text[i] == '(')
            {
                Token lex;
                std::string s = "";
                s += text[i];
                lex.token = s;
                lex.level = lex_type->find(lex_type, s);
                lexemes.push_back(lex);
            }
            cur = "";
        }
        else if (text[i] == ' ' || text[i] == ')' || text[i] == '(')
        {
            if (cur.size() != 0)
            {

                Token lex;
                lex.token = ((cur == "\n") ? "\\n" : cur);

                if (lex_type->find(lex_type, cur) != -1)
                {
                    lex.level = lex_type->find(lex_type, cur);
                }
                else
                {
                    if ((cur[0] == '\'' || cur[cur.size() - 1] == '\'' || cur[0] == '\"' || cur[cur.size() - 1] == '\"') && cur[0] != cur[cur.size() - 1])
                    {
                        int num_of_line = numOfLine();
                        errors.push_back(std::invalid_argument(
                            "In line: " + std::to_string(num_of_line) +
                            "\nInvalid character: \'/\""));
                    }
                    if (cur[0] == '\"' || cur[0] == '\'' || isdigit(cur[0]) || cur[0] == '-' ||
                        cur[0] == '+')
                    {
                        if (cur[0] == '\'' && cur[cur.size() - 1] == '\'')
                        {
                            if (cur.size() != 3)
                            {
                                int num_of_line = numOfLine();
                                errors.push_back(std::invalid_argument(
                                    "In line: " + std::to_string(num_of_line) +
                                    "\nThe char must have one character. "));
                            }
                        }
                        lex.level = 3;
                    }
                    else
                    {
                        findError(cur);
                        lex.level = 2;
                    }
                }

                lexemes.push_back(lex);
            }

            if (text[i] == ')' || text[i] == '(')
            {
                Token lex;
                std::string s = "";
                s += text[i];
                lex.token = s;
                lex.level = lex_type->find(lex_type, s);
                lexemes.push_back(lex);
            }

            cur = "";
        }
        else
        {
            add(cur, text[i]);
        }
    }
    if (cur.size() != 0)
    {
        Token lex;
        lex.token = ((cur == "\n") ? "\\n" : cur);

        if (lex_type->find(lex_type, cur) != -1)
        {
            lex.level = lex_type->find(lex_type, cur);
        }
        else
        {
            if ((cur[0] == '\"' || cur[cur.size() - 1] == '\"') && cur.size() > 1 && cur[0] != cur[cur.size() - 1])
            {
                int num_of_line = numOfLine();
                errors.push_back(std::invalid_argument(
                    "in line: " + std::to_string(num_of_line) +
                    "\nInvalid character: \""));
            }
            if (cur[0] == '\"' || isdigit(cur[0]) || cur[0] == '-' || cur[0] == '+')
            {
                lex.level = 3;
            }
            else
            {
                lex.level = 2;
            }
        }

        lexemes.push_back(lex);
    }
    if (errors.size() != 0)
        throw errors;
    return lexemes;
}