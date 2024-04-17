#include "headers/parse.h"
#include "headers/token.h"
#include "headers/semantic.h"
#include "headers/poliz.h"

#include <vector>
#include <iostream>
#include <regex>

Token c;
extern size_t num_of_line;

TID_tree *tid_tree = new TID_tree, *cur_tid = tid_tree;
func_TID *func_tid = new func_TID;
std::vector<std::string> stack;

POLIZ poliz;

Parser::Parser(std::vector<Token> tokens) : tokens(tokens), current(0) {}

Token Parser::gc()
{
    if (!is_at_end())
    {
        while (!is_at_end() && tokens[current].token == "\\n")
            ++current;

        if (is_at_end())
            return Token("-_-", 404);

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
    return static_cast<size_t>(current) >= tokens.size();
}

void Parser::parse()
{
    c = gc();
    s_exp();

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
        std::cout << "________________\n";
    }
}

// <s_exp> ::= "(" <oper> ") " | <s_exp>
void Parser::s_exp()
{

    // tid_tree->create_TID();
    stack.clear();
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
        poliz.add_lex("(");
        oper();
        poliz.add_lex(")");
    }
    if (c.token != ")")
    {
        throw std::logic_error("in line: " + std::to_string(num_of_line) + ". Found " + c.token + " instead of" + " s_exp close parent ");
    }

    // for extending s_exp
    if (static_cast<size_t>(current) < tokens.size())
    {
        c = gc();

        if (c.level == 404)
            return;

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
    else if (std::regex_match(c.token, std::regex("takzhe|libo|=|>|<|>=|<=|!=")))
    {
        cond_oper();
    }
    // hard_oper ------------------------------
    // loop_for ---------------
    else if (c.token == "idi_poka")
    {
        cur_tid->create_TID();
        c = gc();
        loop_for();
        cur_tid->del_TID();
    }
    // loop -------------------
    else if (c.token == "zhivi")
    {
        cur_tid->create_TID();
        c = gc();
        loop();
        cur_tid->del_TID();
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
        poliz.add_lex("mod");
    }
    // not --------------------
    else if (c.token == "ne")
    {
        c = gc();
        ne();
        poliz.add_lex("not");
    }
    // incf -------------------
    else if (c.token == "pribav")
    {
        c = gc();
        incf();
        poliz.add_lex("incf");
    }
    // decf -------------------
    else if (c.token == "ubav")
    {
        c = gc();
        decf();
        poliz.add_lex("decf");
    }
    // fact -------------------
    else if (c.token == "!")
    {
        c = gc();
        fact();
        poliz.add_lex("fact");
    }
    // setf -------------------
    else if (c.token == "tovarisch")
    {
        c = gc();
        setf();
        poliz.add_lex("setf");
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
    std::string cur_oper = c.token;

    c = gc();

    arg();
    while (c.token != ")")
    {
        arg();
        poliz.add_lex(cur_oper);
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
    std::string cur_oper = (c.token == "takzhe" ? "and" : (c.token == "libo" ? "or" : c.token));

    c = gc();

    arg();
    while (c.token != ")")
    {
        arg();
        poliz.add_lex(cur_oper);
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
    int idx_in = poliz.size();
    if (c.level != 2)
    {
        throw std::logic_error("in line: " + std::to_string(num_of_line) + ". Found " + c.token + " instead of variadle");
    }
    stack.push_back(cur_tid->check_name(c.token));

    poliz.add_lex(c.token);
    std::string var = c.token;
    c = gc();
    if (c.token != "ne_stanet")
    {
        throw std::logic_error("in line: " + std::to_string(num_of_line) + ". Found " + c.token + " instead of" + " ne_stanet loop_for ");
    }
    c = gc();
    if (c.level != 3)
    {
        throw std::logic_error("in line: " + std::to_string(num_of_line) + ". Found " + c.token + " instead of integer litteral");
    }
    stack.push_back(what_type(c.token));

    poliz.add_lex(c.token);

    poliz.add_lex("!=");
    int idx_out = poliz.size();
    poliz.add_lex("");
    poliz.add_lex("F@");

    if (!(stack[stack.size() - 1] == stack[stack.size() - 2] && (stack[stack.size() - 1] == "int" || stack[stack.size() - 1] == "double")))
        throw std::logic_error("in line: " + std::to_string(num_of_line) + ". In the loop condition, the variables must be of type int/double, found: " + stack[stack.size() - 2] + " and " + stack[stack.size() - 1]);

    stack.pop_back();
    stack.pop_back();

    c = gc();
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

    poliz.add_lex(var);
    poliz.add_lex(var);
    poliz.add_lex("1");
    poliz.add_lex("+");
    poliz.add_lex("setf");
    poliz.add_lex(std::to_string(idx_in));
    poliz.add_lex("@");
    poliz.insert_lex(std::to_string(poliz.size()), idx_out);
}

void Parser::loop()
{
    int idx_in = poliz.size();
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

    cond_oper();

    int idx_out = poliz.size();
    poliz.add_lex("");
    poliz.add_lex("F@");
    poliz.add_lex(std::to_string(idx_in));
    poliz.add_lex("@");

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
    poliz.insert_lex(std::to_string(poliz.size()), idx_out);
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

    cond_oper();

    stack.pop_back();

    int idx_f = poliz.size();
    poliz.add_lex("");
    poliz.add_lex("F@");

    // true opers
    c = gc();
    if (c.token != "(")
    {
        throw std::logic_error("in line: " + std::to_string(num_of_line) + ". Found " + c.token + " instead of" + " if true condition open parent ");
    }

    cur_tid->create_TID();
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
    cur_tid->del_TID();

    int idx_out = poliz.size();
    poliz.add_lex("");
    poliz.add_lex("@");
    poliz.insert_lex(std::to_string(poliz.size()), idx_f);

    // false opers
    cur_tid->create_TID();
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
    cur_tid->del_TID();

    c = gc();
    if (c.token != ")")
    {
        throw std::logic_error("in line: " + std::to_string(num_of_line) + ". Found " + c.token + " instead of" + " if close parent ");
    }
    poliz.insert_lex(std::to_string(poliz.size()), idx_out);
}

void Parser::write()
{
    int s = stack.size();
    arg();
    if(stack.size() > s)
        stack.pop_back();
    poliz.add_lex("write");
    std::cerr << "stack size on write: " << stack.size() << std::endl;
    while (c.token != ")")
    {
        s = stack.size();
        arg();
        if(stack.size() > s)
            stack.pop_back();
        poliz.add_lex("write");
    }
    
}

void Parser::read()
{
    if (c.level != 2)
    {
        throw std::logic_error("in line: " + std::to_string(num_of_line) + ". The read operation takes variable, but found litteral ");
    }
    poliz.add_lex(c.token);
    poliz.add_lex("read");
    c = gc();
    while (c.token != ")")
    {
        if (c.level != 2)
        {
            throw std::logic_error("in line: " + std::to_string(num_of_line) + ". The read operation takes variable, but found litteral ");
        }
        poliz.add_lex(c.token);
        poliz.add_lex("read");
        c = gc();
    }
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
    stack.push_back("not");
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
    poliz.add_lex("return");
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

        poliz.add_lex(name);

        c = gc();

        arg();

        std::string type = stack.back();
        
        stack.pop_back();
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

    poliz.add_lex("(");

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
    poliz.add_lex(")");

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

    poliz.add_lex("(");
    ret_op();
    poliz.add_lex(")");

    if (size_stack == stack.size())
    {
        type = "NIL";
        stack.push_back("NIL");
    }
    else
    {
        type = stack[stack.size() - 1];
    }

    if (c.token != ")")
    {
        throw std::logic_error("in line: " + std::to_string(num_of_line) + ". Found " + c.token + " instead of" + " func return close parent ");
    }
    func_tid->set_type(name, type);
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

    poliz.inFunc();
    for (int i = 1; i > 0; c = gc())
    {
        if (c.token == "(")
            ++i;
        if (c.token == ")")
            --i;
    }
    poliz.outFunc();

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

    poliz.add_lex(name);
    poliz.add_lex("CALL");
    poliz.inFunc();

    std::vector<std::string> params_type;
    for (; i < stack.size(); ++i)
    {
        params_type.push_back(stack[i]);
    }
    stack = cur_stack;

    check_func(name, params_type);

    poliz.outFuncCall(name);
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
        poliz.add_lex(c.token);
        c = gc();
        // all good
    }
    else if (c.token == "(")
    {
        poliz.push_stack(c.token);
        c = gc();
        oper();
        // c = gc();
        if (c.token != ")")
        {
            throw std::logic_error("in line: " + std::to_string(num_of_line) + ". Found " + c.token + " instead of arg close oper parent");
        }
        poliz.push_stack(c.token);
        c = gc();
    }
    else if (c.token == ")")
    {
        return;
    }
    else
    {
        throw std::logic_error("in line: " + std::to_string(num_of_line) + ". Found " + c.token + " instead of bad argument ");
    }
    return;
}