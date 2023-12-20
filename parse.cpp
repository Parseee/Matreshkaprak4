#include "headers/parse.h"
#include "headers/token.h"
#include "headers/semantic.h"

#include <vector>
#include <iostream>
#include <regex>

Token c;
size_t num_of_line = 1;

TID_tree *tid_tree = new TID_tree, *cur_tid = tid_tree;

Parser::Parser(std::vector<Token> tokens) : tokens(tokens), current(0) {}

Token Parser::gc()
{
    if (!is_at_end())
    {
        if (tokens[current].token == "\\n")
        {
            ++current;
            ++num_of_line;
        }
        return tokens[current++];
    }
    else
    {
        // maybe add another info
        throw std::logic_error("Unexpected end gc");
    }
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

    tid_tree->create_TID();

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

// <oper> ::= <easy_oper> | <hard_oper> | <func> | <car> | <cdr> | <empty>
void Parser::oper()
{
    // simple_oper ----------------------------
    if (std::regex_match(c.token, std::regex("[+]|-|[*]|/|max|min")))
    {
        stack.push_back(c.token);

        c = gc();
        simple_oper();

        stack.clear();
    }
    // cond_oper ------------------------------
    else if (std::regex_match(c.token, std::regex("takzhe|libo|=|>|<|>=|<=|golova|telo")))
    {
        stack.push_back(c.token);

        c = gc();
        cond_oper();

        stack.clear();
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
    // cons -----------------------------------
    else if (c.token == "partia")
    {
        c = gc();
        arg();
        while (c.token != ")")
        {
            arg();
        }
    }
    else
    {
        if (c.token == ")")
        { // all good
            return;
        }
        else if (c.level == 2)
        { // func_call
            c = gc();
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
    arg();
    while (c.token != ")")
    {
        arg();
    }

    int i = stack.size() - 1;
    std::string t1 = "NIL", t2 = "NIL";
    auto tmp_stack = stack;
    for (; isType(stack[i]); --i)
    {
        t2 = t1;
        t1 = stack[i];
        tmp_stack.pop_back();
    }
    if (stack[i] == "golova")
    {
        stack = tmp_stack;
        stack[i] = t1;
        return;
    }
    else if (stack[i] == "telo")
    {
        stack = tmp_stack;
        stack[i] = t2;
        return;
    }
    std::string t = stack[stack.size() - 1];
    stack.pop_back();
    i = stack.size() - 1;
    for (; isType(stack[i]); --i)
    {
        if (stack[i] != t)
            throw std::logic_error("in line: " + std::to_string(num_of_line) + ". It is not possible to perform an operation with different types: " + t + " " + stack[i]);
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
    if (std::regex_match(t, std::regex("[+]|-|[*]|/")))
    {
        stack[i] = t;
    }
    else
    {
        stack[i] = "bool";
    }
}

void Parser::cond_oper()
{
    arg();
    while (c.token != ")")
    {
        arg();
    }
}

void Parser::loop_for()
{
    if (c.level != 2)
    {
        throw std::logic_error("in line: " + std::to_string(num_of_line) + ". Found " + c.token + " instead of" + " var loop_for ");
    }
    c = gc();
    if (c.token != "ne_stanet")
    {
        throw std::logic_error("in line: " + std::to_string(num_of_line) + ". Found " + c.token + " instead of" + " ne_stanet loop_for ");
    }
    c = gc();
    arg();

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

    c = gc();
    if (c.token != "(")
    {
        throw std::logic_error("in line: " + std::to_string(num_of_line) + ". Found " + c.token + " instead of" + " loop open return paret ");
    }

    c = gc();
    if (c.token != "verni")
    {
        throw std::logic_error("in line: " + std::to_string(num_of_line) + ". Found " + c.token + " instead of" + " loop retrun oper ");
    }

    ret_op();
    if (c.token != ")")
    { // end of return
        throw std::logic_error("in line: " + std::to_string(num_of_line) + ". Found " + c.token + " instead of" + " loop close return parent ");
    }

    c = gc(); // end of umri_kogda
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
}

void Parser::read()
{
    if (c.level != 2)
    {
        throw std::logic_error("in line: " + std::to_string(num_of_line) + ". Found " + c.token + " instead of" + " loop_for ");
    }
    c = gc();
}

void Parser::mod()
{
    arg();
    arg();
}

void Parser::ne()
{
    arg();
}

void Parser::incf()
{
    arg();
    if (c.token != ")")
    {
        arg();
    }
}

void Parser::decf()
{
    arg();
    if (c.token != ")")
    {
        arg();
    }
}

void Parser::ret_op()
{
    c = gc();
    arg();
}

void Parser::fact()
{
    arg();
    while (c.token != ")")
    {
        c = gc();
        arg();
    }
}

void Parser::setf()
{
    if (c.level == 2)
    {
        std::string name = c.token;

        c = gc();

        std::string type = what_type(c.token);

        arg();

        if (tid_tree->find(name))
        {
            tid_tree->replace(name, type);
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
}

void Parser::func()
{
    if (c.level != 2)
    { // name
        throw std::logic_error("in line: " + std::to_string(num_of_line) + ". Found " + c.token + " instead of" + " no function name ");
    }

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
        c = gc();
    }

    if (c.token != ")")
    {
        throw std::logic_error("in line: " + std::to_string(num_of_line) + ". Found " + c.token + " instead of" + " func arg close parent ");
    }

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
    ret_op();
    if (c.token != ")")
    {
        throw std::logic_error("in line: " + std::to_string(num_of_line) + ". Found " + c.token + " instead of" + " func return close parent ");
    }

    c = gc();
    if (c.token != ")")
    {
        throw std::logic_error("in line: " + std::to_string(num_of_line) + ". Found " + c.token + " instead of" + " func close parent ");
    }
}

void Parser::car()
{
    arg();
}

void Parser::cdr()
{
    arg();
}

void Parser::cons()
{
    arg();

    while (c.token != ")")
    {
        arg();
    }
}

void Parser::func_call()
{
    while (c.token != ")")
    {
        arg();
    }
}

// <argf> ::= <var> | <sign> <num> | "(" <oper> ")"
void Parser::arg()
{
    // literal or var name
    if (c.level == 2 || c.level == 3)
    {
        std::string type;
        if (c.level == 2)
        {
            type = cur_tid->check_name(c.token);
        }
        else
        {
            type = what_type(c.token);
        }
        stack.push_back(type);

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