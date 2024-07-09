

#include "headers/io_kit.h"
#include "headers/lexem_analyze.h"
#include "headers/token.h"
#include "headers/parse.h"
#include "headers/semantic.h"
#include "headers/poliz.h"

#include <iostream>
#include <string>

size_t num_of_line = 1;
func_TID *func_tid = new func_TID;
POLIZ poliz;

std::vector<std::string> sys = {"@", "T@", "F@", "CALL", "return", "setf", "read", "write", "mod", "to_str", "<", ">", "<=", ">=", "=", "!=", "+", "-", "/", "*", "not", "fact", "incf", "decf", "min", "max", "and", "or"};

bool sys_word(std::string w)
{
    for (int i = 0; i < sys.size(); ++i)
    {
        if (sys[i] == w)
            return true;
    }
    return false;
}

char is_liter(std::string s)
{
    if (s == "NILL")
        return '-';
    if (s == "true")
        return 't';
    if (s == "false")
        return 'f';
    if (s[0] == '\"')
        return 's';
    if (s[0] == '\'')
        return 'c';
    if ((s[0] >= '0' && s[0] <= '9') || ((s[0] == '+' || s[0] == '-') && (s[1] >= '0' && s[1] <= '9')))
    {
        for (int i = 0; i < s.size(); ++i)
            if (s[i] == '.')
                return 'd';
        return 'i';
    }
    return 'n';
}
int main()
{
    setlocale(LC_ALL, "en-US.UTF-8");
    std::vector<Token> lexes;
    try
    {
        lexes = lex_an(read_file("../materials/prgrm.rus"));
    }
    catch (std::vector<std::invalid_argument> e)
    {
        for (int i = 0; i < e.size(); i++)
            std::cerr << "exception " << e[i].what() << "\n\n";
    }
    for (int i = 0; i < lexes.size(); ++i)
    {
        std::cout << i + 1 << ":  " << lexes[i].token << " " << lexes[i].level << std::endl;
    }
    try
    {
        Parser parser(lexes);
        parser.parse();
    }
    catch (std::logic_error e)
    {
        std::cerr << "Exception " << e.what() << std::endl;
        return 0;
    }
    catch (const std::overflow_error &e)
    {
        std::cerr << "Unknown exception" << std::endl;
    }

    std::cout << "Syntax analysis complete sucsessfully\n";
    //------------Show_POLIZ------------------
    poliz.show();
    for (auto it : func_tid->name_)
    {
        auto x = poliz.get_func_info(it);
        for (auto it : x)
        {
            for (auto jt : it)
            {
                std::cout << jt << " ";
            }
            std::cout << "\n";
        }
        std::cout << "________________\n\n\n\n\n";
    }
    //--------------------------------------------*/
    std::vector<std::string> main_poliz = poliz.getPOLIZ();
    std::vector<std::string> func_poliz;
    std::vector<std::string> var_name;
    std::vector<char> var_type;
    std::vector<std::string> var_val;
    std::vector<std::string> rezerv_var_name;
    std::vector<char> rezerv_var_type;
    std::vector<std::string> rezerv_var_val;
    std::vector<std::string> rezerv_stack;
    std::vector<std::string> stack;
    std::vector<std::string> func_stack;
    int pos = 0;
    int pos_func = 0;
    bool _func = false;
    while (pos < main_poliz.size())
    {
        std::string cur;
        if (_func)
            cur = func_poliz[pos_func];
        else
            cur = main_poliz[pos];
        if (sys_word(cur))
        {
            if (cur == "CALL")
            {
                pos_func = -1;
                std::string func_name = stack.back();
                stack.pop_back();
                std::vector<std::vector<std::string>> func_info = poliz.get_func_info(func_name);
                rezerv_var_name = var_name;
                rezerv_var_type = var_type;
                rezerv_var_val = var_val;
                var_name = func_info[2];
                var_val.resize(var_name.size());
                var_type.resize(var_name.size());
                for (int i = var_name.size() - 1; i >= 0; --i)
                {
                    std::string tmp = stack.back();
                    stack.pop_back();
                    char tnp = is_liter(tmp);
                    if (tnp == 'n')
                    {
                        for (int i = 0; i < rezerv_var_name.size(); ++i)
                        {
                            if (rezerv_var_name[i] == tmp)
                            {
                                tnp = rezerv_var_type[i];
                                tmp = rezerv_var_val[i];
                                break;
                            }
                        }
                    }
                    var_val[i] = tmp;
                    var_type[i] = tnp;
                }
                func_poliz = func_info[4];
                rezerv_stack = stack;
                stack.clear();
                _func = true;
            }
            else if (std::regex_match(cur, std::regex("[+]|-|[*]|/|max|min|and|or|setf|incf|decf|mod|<|>|<=|>=|=|!=")))
            {
                std::string s2 = stack.back();
                stack.pop_back();
                std::string s1 = stack.back();
                stack.pop_back();
                char h1 = is_liter(s1), h2 = is_liter(s2);
                if (cur == "setf")
                {
                    bool _found = false;
                    for (int i = 0; i < var_name.size(); ++i)
                    {
                        if (var_name[i] == s1)
                        {
                            var_type[i] = h2;
                            var_val[i] = s2;
                            _found = true;
                            break;
                        }
                    }
                    if (!_found)
                    {
                        var_name.push_back(s1);
                        var_type.push_back(h2);
                        var_val.push_back(s2);
                    }
                    goto end;
                }
                if (h2 == 'n')
                {
                    for (int i = 0; i < var_name.size(); ++i)
                    {
                        if (var_name[i] == s2)
                        {
                            h2 = var_type[i];
                            s2 = var_val[i];
                            break;
                        }
                    }
                }
                if ((cur == "incf" || cur == "decf") && h1 == 'n')
                {
                    for (int i = 0; i < var_name.size(); ++i)
                    {
                        if (var_name[i] == s1)
                        {
                            if (cur == "incf")
                            {
                                if (h2 == 'i')
                                {
                                    var_val[i] = std::to_string(std::stoi(var_val[i]) + std::stoi(s2));
                                }
                                else
                                {
                                    var_val[i] = std::to_string(std::stod(var_val[i]) + std::stod(s2));
                                }
                            }
                            else
                            {
                                if (h2 == 'i')
                                {
                                    var_val[i] = std::to_string(std::stoi(var_val[i]) - std::stoi(s2));
                                }
                                else
                                {
                                    var_val[i] = std::to_string(std::stod(var_val[i]) - std::stod(s2));
                                }
                            }
                            break;
                        }
                    }
                    goto end;
                }
                if (h1 == 'n')
                {
                    for (int i = 0; i < var_name.size(); ++i)
                    {
                        if (var_name[i] == s1)
                        {
                            h1 = var_type[i];
                            s1 = var_val[i];
                            break;
                        }
                    }
                }
                if (cur == "+" || cur == "incf")
                {
                    if (h1 == 'i' && h2 == 'i')
                    {
                        stack.push_back(std::to_string(std::stoi(s1) + std::stoi(s2)));
                    }
                    else if (h1 == 'd' && h2 == 'd')
                    {
                        stack.push_back(std::to_string(std::stod(s1) + std::stod(s2)));
                    }
                    else
                    {
                        s1.pop_back();
                        s2.erase(s2.begin());
                        stack.push_back(s1 + s2);
                    }
                }
                else if (cur == "-" || cur == "decf")
                {
                    if (h1 == 'i' && h2 == 'i')
                    {
                        stack.push_back(std::to_string(std::stoi(s1) - std::stoi(s2)));
                    }
                    else
                    {
                        stack.push_back(std::to_string(std::stod(s1) - std::stod(s2)));
                    }
                }
                else if (cur == "*")
                {
                    if (h1 == 'i' && h2 == 'i')
                    {
                        stack.push_back(std::to_string(std::stoi(s1) * std::stoi(s2)));
                    }
                    else
                    {
                        stack.push_back(std::to_string(std::stod(s1) * std::stod(s2)));
                    }
                }
                else if (cur == "/")
                {
                    if (h1 == 'i' && h2 == 'i')
                    {
                        stack.push_back(std::to_string(std::stoi(s1) / std::stoi(s2)));
                    }
                    else
                    {
                        stack.push_back(std::to_string(std::stod(s1) / std::stod(s2)));
                    }
                }
                else if (cur == "min")
                {
                    if (h1 == 'i' && h2 == 'i')
                    {
                        stack.push_back(std::to_string(std::min(std::stoi(s1), std::stoi(s2))));
                    }
                    else
                    {
                        stack.push_back(std::to_string(std::min(std::stod(s1), std::stod(s2))));
                    }
                }
                else if (cur == "max")
                {
                    if (h1 == 'i' && h2 == 'i')
                    {
                        stack.push_back(std::to_string(std::max(std::stoi(s1), std::stoi(s2))));
                    }
                    else
                    {
                        stack.push_back(std::to_string(std::max(std::stod(s1), std::stod(s2))));
                    }
                }
                else if (cur == "and")
                {
                    if (h1 == 't' && h2 == 't')
                    {
                        stack.push_back("true");
                    }
                    else
                    {
                        stack.push_back("false");
                    }
                }
                else if (cur == "or")
                {
                    if (h1 == 't' || h2 == 't')
                    {
                        stack.push_back("true");
                    }
                    else
                    {
                        stack.push_back("false");
                    }
                }
                else if (cur == "<")
                {
                    if (s1 < s2)
                        stack.push_back("true");
                    else
                        stack.push_back("false");
                }
                else if (cur == ">")
                {
                    if (s1 > s2)
                        stack.push_back("true");
                    else
                        stack.push_back("false");
                }
                else if (cur == "<=")
                {
                    if (s1 <= s2)
                        stack.push_back("true");
                    else
                        stack.push_back("false");
                }
                else if (cur == ">=")
                {
                    if (s1 >= s2)
                        stack.push_back("true");
                    else
                        stack.push_back("false");
                }
                else if (cur == "=")
                {
                    if (s1 == s2)
                        stack.push_back("true");
                    else
                        stack.push_back("false");
                }
                else if (cur == "!=")
                {
                    if (s1 != s2)
                        stack.push_back("true");
                    else
                        stack.push_back("false");
                }
                else if (cur == "mod")
                {
                    stack.push_back(std::to_string(std::stoi(s1) % std::stoi(s2)));
                }
            }
            else
            {
                std::string s1 = stack.back(), s2 = s1;
                stack.pop_back();
                char h1 = is_liter(s1);
                if (h1 == 'n')
                {
                    for (int i = 0; i < var_name.size(); ++i)
                    {
                        if (var_name[i] == s1)
                        {
                            h1 = var_type[i];
                            s1 = var_val[i];
                            break;
                        }
                    }
                }
                if (cur == "write")
                {
                    if (h1 == 's' || h1 == 'c')
                    {
                        s1.erase(s1.begin());
                        s1.pop_back();
                    }
                    int i = 0;
                    for (; i < s1.size();)
                    {
                        if (s1[i] == '\\' && i + 1 < s1.size())
                        {
                            if (s1[i + 1] == 'n')
                            {
                                i = i + 1;
                                std::cout << '\n';
                            }
                        }
                        else
                            std::cout << s1[i];
                        ++i;
                    }
                }
                else if (cur == "read")
                {
                    for (int i = 0; i < var_name.size(); ++i)
                    {
                        if (var_name[i] == s2)
                        {
                            std::string new_val;
                            std::cin >> new_val;
                            var_type[i] = is_liter(new_val);
                            var_val[i] = new_val;
                            break;
                        }
                    }
                }
                else if (cur == "not")
                {
                    if (h1 == 't')
                        stack.push_back("false");
                    else
                        stack.push_back("true");
                }
                else if (cur == "fact")
                {
                    int num = std::stoi(s1), res = 1;
                    for (int i = 1; i <= num; ++i)
                    {
                        res *= i;
                    }
                    stack.push_back(std::to_string(res));
                }
                else if (cur == "@")
                {
                    pos = std::stoi(s1) - 1;
                }
                else if (cur == "T@")
                {
                    bool cur_bool = (stack.back() == "true");
                    stack.pop_back();
                    if (cur_bool)
                    {
                        pos = std::stoi(s1) - 1;
                    }
                }
                else if (cur == "F@")
                {
                    bool cur_bool = (stack.back() == "true");
                    stack.pop_back();
                    if (!cur_bool)
                    {
                        pos = std::stoi(s1) - 1;
                    }
                }
                else if (cur == "to_str")
                {
                    stack.push_back("\"" + s1 + "\"");
                }
                else if (cur == "return")
                {
                    _func = false;
                    var_name = rezerv_var_name;
                    var_type = rezerv_var_type;
                    var_val = rezerv_var_val;
                    stack = rezerv_stack;
                    if (h1 != '-')
                        stack.push_back(s1);
                }
            }
        }
        else
        {
            stack.push_back(cur);
        }
    end:
        if (_func)
            ++pos_func;
        else
            ++pos;
    }
    //
}

/*
(pribav (telo (esli (<= +0  4)((max  6))) (ubav c66 +0)))
(func p9 () (not ywvo -9))

----- num from 10SS to 2SS ---------
(tovarisch x 0)
(sprosi x)
(napishi x " binary: ")
(tovarisch res "")
(tovarisch tmp 1)
(zhivi (tovarisch res (+ res (v_stroku (mod x 2))))(tovarisch x (/ x 2)) (umri_kogda (= x 0)))
(napishi res)

------ check work loop and else ------------------
(tovarisch i 0)
(idi_poka i ne_stanet 20 (esli (= (mod i 2) 0)((napishi i))((napishi " - chet\n"))))

------ check work func -------------------------
(func foo (a b) ( (tovarisch c (+ a b)) ) (verni c) )
(napishi (+ 1 2) "\n")
(napishi (- 1 2) "\n")
(napishi (+ (foo 1 2) 2))


*/
