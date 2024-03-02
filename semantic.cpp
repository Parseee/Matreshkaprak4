#include "headers/semantic.h"
#include <regex>

// global funcs

std::string what_type(std::string s)
{
    if (s == "PRAVDA" || s == "LOZH")
    {
        return "bool";
    }
    else if (s[0] == '\'')
    {
        return "char";
    }
    else if (s[0] == '\"')
    {
        return "string";
    }
    else
    {
        for (int i = 0; i < s.size(); ++i)
        {
            if (s[i] == '.')
            {
                return "double";
            }
        }
        return "int";
    }
}

bool isType(std::string s)
{
    if (std::regex_match(s, std::regex("int|double|string|char|bool|NIL")))
    {
        return true;
    }
    return false;
}

// TID funcs

void TID::push_name(std::string s, std::string t)
{
    for (int i = 0; i < name_.size(); ++i)
    {
        if (s == name_[i])
            throw std::logic_error("in line: " + std::to_string(num_of_line) + ". The name " + s + " has already been used. ");
    }
    name_.push_back(s);
    type_.push_back(t);
}

std::string TID::check_name(std::string s)
{
    for (int i = 0; i < name_.size(); ++i)
    {
        if (s == name_[i])
            return type_[i];
    }
    return "-_-";
}

void TID::replace(std::string s, std::string t)
{
    for (int i = 0; i < name_.size(); ++i)
    {
        if (s == name_[i])
        {
            type_[i] = t;
            return;
        }
    }
}

// func_TID funcs

void func_TID::push_name(std::string s, int n, std::vector<std::string> p_name)
{
    if (cur_tid->find(s))
        throw std::logic_error("in line: " + std::to_string(num_of_line) + ". The name " + s + " has already been used. ");

    name_.push_back(s);
    type_.push_back("NIL");
    num_of_lex.push_back(n);
    TID tmp;
    for (int j = 0; j < p_name.size(); ++j)
    {
        tmp.push_name(p_name[j], "NIL");
    }
    tid_.push_back(tmp);
}
std::string func_TID::func_type(std::string s)
{
    for (int i = 0; i < name_.size(); ++i)
    {
        if (s == name_[i])
        {
            return type_[i];
        }
    }
}

bool func_TID::find(std::string s)
{
    for (int i = 0; i < name_.size(); ++i)
    {
        if (s == name_[i])
            return true;
    }
    return false;
}

void func_TID::set_type(std::string s, std::string t)
{
    for (int i = 0; i < name_.size(); ++i)
    {
        if (s == name_[i])
        {
            type_[i] = t;
            return;
        }
    }
}

void func_TID::set_params_type(std::string s, std::vector<std::string> p_type)
{
    for (int i = 0; i < name_.size(); ++i)
    {
        if (s == name_[i])
        {
            tid_[i].type_ = p_type;
            return;
        }
    }
}

// TID_tree funcs

void TID_tree::create_TID()
{
    TID_tree *now = new TID_tree;
    now->tid.name_.clear();
    now->tid.type_.clear();
    now->prev = cur_tid;
    cur_tid->next = now;
    cur_tid = now;
}

void TID_tree::create_TID(TID tid)
{
    TID_tree *now = new TID_tree;
    now->tid = tid;
    now->prev = cur_tid;
    cur_tid->next = now;
    cur_tid = now;
}

void TID_tree::del_TID()
{
    TID_tree *p = tid_tree, *q;
    for (; p->next != cur_tid; p = p->next)
        ;
    p->next = nullptr;
    q = cur_tid;
    cur_tid = p;
    delete q;
}

std::string TID_tree::check_name(std::string s)
{
    if (this == nullptr)
        throw std::logic_error("in line: " + std::to_string(num_of_line) + ". There is no identifier with the name " + s);
    if (this->tid.check_name(s) == "-_-")
    {
        return this->prev->check_name(s);
    }
    else
    {
        return this->tid.check_name(s);
    }
}

bool TID_tree::find(std::string s)
{
    if (this == nullptr)
    {
        if (func_tid->find(s))
            return true;
        else
            return false;
    }
    if (this->tid.check_name(s) == "-_-")
    {
        return this->prev->find(s);
    }
    else
    {
        return true;
    }
}

void TID_tree::replace(std::string s, std::string t)
{
    if (this->tid.check_name(s) == "-_-")
    {
        this->prev->replace(s, t);
    }
    else
    {
        this->tid.replace(s, t);
    }
}
