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
        bool isDouble = false;
        for (int i = 0; i < s.size(); ++i)
        {
            if (s[i] == '.')
            {
                isDouble = true;
                break;
            }
        }
        if (isDouble)
        {
            return "double";
        }
        else
        {
            return "int";
        }
    }
}

bool isType(std::string s)
{
    if (std::regex_match(s, std::regex("int|double|string|char|bool")))
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

void func_TID::push_name(std::string s, std::string t, std::vector<std::string> p_name)
{
    int i = 0;
    for (; i < name_.size(); ++i)
    {
        if (s == name_[i])
            throw std::logic_error("in line: " + std::to_string(num_of_line) + ". The name " + s + " has already been used. ");
    }
    if (cur_tid->find(s))
        throw std::logic_error("in line: " + std::to_string(num_of_line) + ". The name " + s + " has already been used. ");

    name_.push_back(s);
    type_.push_back(t);
    for (int j = 0; j < p_name.size(); ++j)
    {
        tid_[i].push_name(p_name[j], "NIL");
    }
}
std::string func_TID::check_name(std::string s, std::vector<std::string> p_type)
{
    for (int i = 0; i < name_.size(); ++i)
    {
        if (s == name_[i])
        {
            for (int j = 0; j < p_type.size(); ++j)
            {
                tid_[i].type_[j] = p_type[j];
            }
            return type_[i];
        }
    }
    throw std::logic_error("in line: " + std::to_string(num_of_line) + ". No function named " + s);
    return "-_-";
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
        this->prev->check_name(s);
    }
    else
    {
        this->tid.replace(s, t);
    }
}
