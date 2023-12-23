#include "headers/parse.h"
#include "headers/token.h"
#include "headers/semantic.h"

#include <vector>
#include <iostream>
#include <regex>

Token c;
extern size_t num_of_line;

TID_tree *tid_tree = new TID_tree, *cur_tid = tid_tree;
func_TID *func_tid = new func_TID;
std::vector<std::string> stack;

Parser::Parser(std::vector<Token> tokens) : tokens(tokens), current(0) {}

Token Parser::gc()
{
    if (!is_at_end())
    {
        if (tokens[current].token == "\\n")
            ++current;
        num_of_line = getNumLine(current);
        return tokens[current++];
    }
    else
    {
        // maybe add another info
        throw std::logic_error("Unexpected end gc");
    }
}
int Parser::getNumLine(int cu)
{
    int k = 1;
    for (int i = 0; i < cu; ++i)
    {
        if (tokens[i].token == "\\n")
            ++k;
    }
    return k;
}

bool Parser::is_at_end()
{
    return static_cast<size_t>(current) == tokens.size();
}

void Parser::parse()
{
    c = gc();
    s_exp();
}

// <s_exp> ::= "(" <oper> ") " | <s_exp>
void Parser::s_exp()
{

    // tid_tree->create_TID();

    if (c.token != "(")
    {
        throw std::logic_error("in line: " + std::to_string(num_of_line) + ". Found " + c.token + " instead of" + " s_exp open parent ");
    }
    c = gc();
    if (c.token == "func")
    {
        c = gc();
        func();
    }
    else
    {
        oper();
        if (c.token != ")")
        {
            throw std::logic_error("in line: " + std::to_string(num_of_line) + ". Found " + c.token + " instead of" + " s_exp close parent ");
        }
    }
    // for extending s_exp
    if (static_cast<size_t>(current) < tokens.size())
    {
        c = gc();
        s_exp();
    }
}

// <oper> ::= <easy_oper> | <hard_oper> | <func> | <empty>
void Parser::oper()
{
    // simple_oper ----------------------------
    if (std::regex_match(c.token, std::regex("[+]|-|[*]|/|max|min")))
    {
        simple_oper();
    }
    // cond_oper ------------------------------
    else if (std::regex_match(c.token, std::regex("takzhe|libo|=|>|<|>=|<=")))
    {
        cond_oper();
    }
    // hard_oper ------------------------------
    // loop_for ---------------
    else if (c.token == "idi_poka")
    {
        c = gc();
        loop_for();
    }
    // loop -------------------
    else if (c.token == "zhivi")
    {
        c = gc();
        loop();
    }
    // if ---------------------
    else if (c.token == "esli")
    {
        c = gc();
        if_op();
    }
    // write ------------------
    else if (c.token == "napishi")
    {
        c = gc();
        write();
    }
    // read -------------------
    else if (c.token == "sprosi")
    {
        c = gc();
        read();
    }
    // mod --------------------
    else if (c.token == "mod")
    {
        c = gc();
        mod();
    }
    // not --------------------
    else if (c.token == "ne")
    {
        c = gc();
        ne();
    }
    // incf -------------------
    else if (c.token == "pribav")
    {
        c = gc();
        incf();
    }
    // decf -------------------
    else if (c.token == "ubav")
    {
        c = gc();
        decf();
    }
    // fact -------------------
    else if (c.token == "!")
    {
        c = gc();
        fact();
    }
    // setf -------------------
    else if (c.token == "tovarisch")
    {
        c = gc();
        setf();
    }
    else
    {
        if (c.token == ")")
        { // all good
            return;
        }
        else if (c.level == 2)
        { // func_call
            func_call();
        }
        else
        {
            throw std::logic_error("in line: " + std::to_string(num_of_line) + ". Found " + c.token + " instead of" + " bad oper ");
        }
    }
}

void Parser::simple_oper()
{
    stack.push_back(c.token);

    c = gc();

    arg();
    while (c.token != ")")
    {
        arg();
    }

    std::string t = stack[stack.size() - 1];
    stack.pop_back();

    int i = stack.size() - 1;
    for (; isType(stack[i]); --i)
    {
        if (stack[i] != t)
            throw std::logic_error("in line: " + std::to_string(num_of_line) + ". It is not possible to perform an operation with different types: " + stack[i] + " " + t);
        stack.pop_back();
    }
    if (stack[i] == "max" || stack[i] == "min")
    {
        if (t == "int" || t == "double")
        {
            stack[i] = t;
            return;
        }
        else
        {
            throw std::logic_error("in line: " + std::to_string(num_of_line) + ". Min/max operations accept arguments of type int/double, not found: " + t);
        }
    }
    else
    {
        stack[i] = t;
    }
}

void Parser::cond_oper()
{

    stack.push_back(c.token);

    c = gc();

    arg();
    while (c.token != ")")
    {
        arg();
    }

    std::string t = stack[stack.size() - 1];
    stack.pop_back();
    int i = stack.size() - 1;
    for (; isType(stack[i]); --i)
    {
        if (stack[i] != t)
            throw std::logic_error("in line: " + std::to_string(num_of_line) + ". It is not possible to perform an operation with different types: " + stack[i] + " " + t);
        stack.pop_back();
    }
    stack[i] = "bool";
}

void Parser::loop_for()
{
    arg();
    c = gc();
    if (c.token != "ne_stanet")
    {
        throw std::logic_error("in line: " + std::to_string(num_of_line) + ". Found " + c.token + " instead of" + " ne_stanet loop_for ");
    }
    c = gc();
    arg();

    if (!(stack[stack.size() - 1] == stack[stack.size() - 2] && (stack[stack.size() - 1] == "int" || stack[stack.size() - 1] == "double")))
        throw std::logic_error("in line: " + std::to_string(num_of_line) + ". In the loop condition, the variables must be of type int/double, found: " + stack[stack.size() - 2] + " and " + stack[stack.size() - 1]);

    stack.pop_back();
    stack.pop_back();

    // c = gc();
    while (true)
    {
        if (c.token != "(")
        {
            throw std::logic_error("in line: " + std::to_string(num_of_line) + ". Found " + c.token + " instead of" + " loop_for open parent oper ");
        }
        c = gc();
        oper();
        if (c.token != ")")
        {
            throw std::logic_error("in line: " + std::to_string(num_of_line) + ". Found " + c.token + " instead of" + " loop_for close parent oper ");
        }
        c = gc();
        if (c.token == ")")
        {
            break;
        }
    }
}

void Parser::loop()
{
    while (true)
    {
        if (c.token != "(")
        {
            throw std::logic_error("in line: " + std::to_string(num_of_line) + ". Found " + c.token + " instead of" + " loop open parent oper ");
        }
        c = gc();
        if (c.token == "umri_kogda")
        {
            break;
        }
        oper();

        if (c.token != ")")
        {
            throw std::logic_error("in line: " + std::to_string(num_of_line) + ". Found " + c.token + " instead of" + " loop close parent oper ");
        }
        c = gc();
    }

    c = gc();
    if (c.token != "(")
    {
        throw std::logic_error("in line: " + std::to_string(num_of_line) + ". Found " + c.token + " instead of" + " loop open parent cond_oper ");
    }

    c = gc();
    if (!std::regex_match(c.token, std::regex("takzhe|libo|=|/=|>|<|>=|<=")))
    {
        throw std::logic_error("in line: " + std::to_string(num_of_line) + ". Found " + c.token + " instead of" + " loop cond_oper condition ");
    }

    c = gc();
    cond_oper();

    stack.pop_back();

    c = gc();
    // end of umri_kogda
    if (c.token != ")")
    {
        throw std::logic_error("in line: " + std::to_string(num_of_line) + ". Found " + c.token + " instead of" + " loop close umri_kogda parent ");
    }

    c = gc(); // end of loop
    if (c.token != ")")
    {
        throw std::logic_error("in line: " + std::to_string(num_of_line) + ". Found " + c.token + " instead of" + " loop close parent ");
    }
}

void Parser::if_op()
{
    if (c.token != "(")
    {
        throw std::logic_error("in line: " + std::to_string(num_of_line) + ". Found " + c.token + " instead of" + " if open parent ");
    }

    c = gc();
    if (!std::regex_match(c.token, std::regex("takzhe|libo|=|/=|>|<|>=|<=")))
    {
        throw std::logic_error("in line: " + std::to_string(num_of_line) + ". Found " + c.token + " instead of" + " if condition ");
    }

    c = gc();
    cond_oper();

    stack.pop_back();

    // true opers
    c = gc();
    if (c.token != "(")
    {
        throw std::logic_error("in line: " + std::to_string(num_of_line) + ". Found " + c.token + " instead of" + " if true condition open parent ");
    }

    c = gc();
    while (true)
    {
        if (c.token != "(")
        {
            throw std::logic_error("in line: " + std::to_string(num_of_line) + ". Found " + c.token + " instead of" + " if true condition opers ");
        }
        c = gc();
        oper();
        if (c.token != ")")
        {
            throw std::logic_error("in line: " + std::to_string(num_of_line) + ". Found " + c.token + " instead of" + " if true condition opers ");
        }
        c = gc();
        if (c.token == ")")
        {
            break;
        }
    }

    // false opers
    c = gc();
    if (c.token != "(")
    {
        throw std::logic_error("in line: " + std::to_string(num_of_line) + ". Found " + c.token + " instead of" + " if false condition open parent ");
    }

    c = gc();
    while (true)
    {
        if (c.token != "(")
        {
            throw std::logic_error("in line: " + std::to_string(num_of_line) + ". Found " + c.token + " instead of" + " if false condition opers ");
        }
        c = gc();
        oper();
        if (c.token != ")")
        {
            throw std::logic_error("in line: " + std::to_string(num_of_line) + ". Found " + c.token + " instead of" + " if false condition opers ");
        }
        c = gc();
        if (c.token == ")")
        {
            break;
        }
    }

    c = gc();
    if (c.token != ")")
    {
        throw std::logic_error("in line: " + std::to_string(num_of_line) + ". Found " + c.token + " instead of" + " if close parent ");
    }
}

void Parser::write()
{
    arg();
    stack.pop_back();
}

void Parser::read()
{
    if (c.level != 2)
    {
        throw std::logic_error("in line: " + std::to_string(num_of_line) + ". The read operation takes variable, but found litteral ");
    }
    c = gc();
    stack.pop_back();
}

void Parser::mod()
{
    arg();
    arg();

    if (!(stack[stack.size() - 1] == stack[stack.size() - 2] && stack[stack.size() - 1] == "int"))
        throw std::logic_error("in line: " + std::to_string(num_of_line) + ". The mod operation takes values of the int type, found " + stack[stack.size() - 2] + " and " + stack[stack.size() - 1]);
    stack.pop_back();
    stack.pop_back();
}

void Parser::ne()
{
    arg();
    stack.pop_back();
    stack.push_back("bool");
}

void Parser::incf()
{
    arg();
    if (c.token != ")")
    {
        arg();

        if (!(stack[stack.size() - 1] == stack[stack.size() - 2] && (stack[stack.size() - 1] == "int" || stack[stack.size() - 1] == "double")))
            throw std::logic_error("in line: " + std::to_string(num_of_line) + ". The increment operation takes int/double values, found: " + stack[stack.size() - 2] + " and " + stack[stack.size() - 1]);

        stack.pop_back();
        stack.pop_back();
        return;
    }
    if (!(stack[stack.size() - 1] == "int" || stack[stack.size() - 1] == "double"))
        throw std::logic_error("in line: " + std::to_string(num_of_line) + ". The increment operation takes int/double values, found: " + stack[stack.size() - 1]);

    stack.pop_back();
}

void Parser::decf()
{
    arg();
    if (c.token != ")")
    {
        arg();

        if (!(stack[stack.size() - 1] == stack[stack.size() - 2] && (stack[stack.size() - 1] == "int" || stack[stack.size() - 1] == "double")))
            throw std::logic_error("in line: " + std::to_string(num_of_line) + ". The decrement operation takes int/double values, found: " + stack[stack.size() - 2] + " and " + stack[stack.size() - 1]);

        stack.pop_back();
        stack.pop_back();
        return;
    }
    if (!(stack[stack.size() - 1] == "int" || stack[stack.size() - 1] == "double"))
        throw std::logic_error("in line: " + std::to_string(num_of_line) + ". The decrement operation takes int/double values, found: " + stack[stack.size() - 1]);

    stack.pop_back();
}

void Parser::ret_op()
{
    c = gc();
    arg();
}

void Parser::fact()
{
    arg();

    if (!(stack[stack.size() - 1] == "int"))
        throw std::logic_error("in line: " + std::to_string(num_of_line) + ". The factorial operation take value of the int type, found " + stack[stack.size() - 1]);
}

void Parser::setf()
{
    if (c.level == 2)
    {
        std::string name = c.token;

        c = gc();

        std::string type = what_type(c.token);

        arg();

        if (cur_tid->find(name))
        {
            if (func_tid->find(name))
                throw std::logic_error("in line: " + std::to_string(num_of_line) + ". Cant assign anything to function");
            cur_tid->replace(name, type);
        }
        else
        {
            cur_tid->tid.push_name(name, type);
        }
    }
    else
    {
        throw std::logic_error("in line: " + std::to_string(num_of_line) + ". Found " + c.token + " instead of" + " can't assign to non-var ");
    }
    stack.pop_back();
}

void Parser::check_func(std::string name, std::vector<std::string> param_type)
{
    int last_cur = current;

    func_tid->set_params_type(name, param_type);

    std::string type = "NIL";
    int i = func_tid->get_idx(name);

    cur_tid->create_TID(func_tid->get_tid_params(i));

    current = func_tid->get_num_of_lex(i);

    c = gc();

    if (c.token != "(")
    {
        throw std::logic_error("in line: " + std::to_string(num_of_line) + ". Found " + c.token + " instead of" + " func open opers_group parent ");
    }

    c = gc();

    while (true)
    {
        if (c.token != "(")
        {
            throw std::logic_error("in line: " + std::to_string(num_of_line) + ". Found " + c.token + " instead of" + " func open opers parent ");
        }
        c = gc();
        oper();
        // c = gc();
        if (c.token != ")")
        {
            throw std::logic_error("in line: " + std::to_string(num_of_line) + ". Found " + c.token + " instead of" + " func close opers parent ");
        }
        c = gc();
        if (c.token == ")")
        {
            break;
        }
    }

    c = gc();
    if (c.token != "(")
    {
        throw std::logic_error("in line: " + std::to_string(num_of_line) + ". Found " + c.token + " instead of" + " func return open parent ");
    }
    c = gc();
    if (c.token != "verni")
    {
        throw std::logic_error("in line: " + std::to_string(num_of_line) + ". Found " + c.token + " instead of" + " no verni found ");
    }
    int size_stack = stack.size();

    ret_op();

    if (size_stack == stack.size())
    {
        type = "NIL";
    }
    else
    {
        type = stack[stack.size() - 1];
        stack.pop_back();
    }

    if (c.token != ")")
    {
        throw std::logic_error("in line: " + std::to_string(num_of_line) + ". Found " + c.token + " instead of" + " func return close parent ");
    }

    cur_tid->del_TID();
    current = last_cur;
}
void Parser::func()
{
    if (c.level != 2)
    { // name
        throw std::logic_error("in line: " + std::to_string(num_of_line) + ". Found " + c.token + " instead of" + " no function name ");
    }
    std::string name = c.token;
    std::vector<std::string> params_name;

    c = gc();
    if (c.token != "(")
    {
        throw std::logic_error("in line: " + std::to_string(num_of_line) + ". Found " + c.token + " instead of" + " func open parent ");
    }

    c = gc();
    while (c.token != ")")
    {
        if (c.level != 2)
        {
            throw std::logic_error("in line: " + std::to_string(num_of_line) + ". Found " + c.token + " instead of" + " bad func argument ");
        }

        params_name.push_back(c.token);

        c = gc();
    }

    if (c.token != ")")
    {
        throw std::logic_error("in line: " + std::to_string(num_of_line) + ". Found " + c.token + " instead of" + " func arg close parent ");
    }

    func_tid->push_name(name, current, params_name);

    c = gc();
    c = gc();

    for (int i = 1; i > 0; c = gc())
    {
        if (c.token == "(")
            ++i;
        if (c.token == ")")
            --i;
    }

    c = gc();
    c = gc();

    for (int i = 1; i > 0; c = gc())
    {
        if (c.token == "(")
            ++i;
        if (c.token == ")")
            --i;
    }
    if (c.token != ")")
    {
        throw std::logic_error("in line: " + std::to_string(num_of_line) + ". Found " + c.token + " instead of" + " func close parent ");
    }
}

void Parser::func_call()
{
    if (!func_tid->find(c.token))
        throw std::logic_error("in line: " + std::to_string(num_of_line) + ". There is no function named: " + c.token);

    std::string name = c.token, type;
    int i = stack.size();

    auto cur_stack = stack;

    c = gc();

    while (c.token != ")")
    {
        arg();
    }

    std::vector<std::string> params_type;
    for (; i < stack.size(); ++i)
    {
        params_type.push_back(stack[i]);
    }
    stack = cur_stack;

    check_func(name, params_type);
}

// <argf> ::= <var> | <sign> <num> | "(" <oper> ")"
void Parser::arg()
{
    // literal or var name
    if (c.level == 2 || c.level == 3)
    {
        if (c.level == 3)
        {
            stack.push_back(what_type(c.token));
        }
        else
        {
            stack.push_back(cur_tid->check_name(c.token));
        }
        c = gc();
        // all good
    }
    else if (c.token == "(")
    {
        c = gc();
        oper();
        // c = gc();
        if (c.token != ")")
        {
            throw std::logic_error("in line: " + std::to_string(num_of_line) + ". Found " + c.token + " instead of"
                                                                                                      " arg close oper parent");
        }
        c = gc();
    }
    else if (c.token == ")")
    {
        return;
    }
    else
    {
        throw std::logic_error("in line: " + std::to_string(num_of_line) + ". Found " + c.token + " instead of"
                                                                                                  " bad argument ");
    }
    return;
}