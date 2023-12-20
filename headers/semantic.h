#include "io_kit.h"
#include "lexem_analyze.h"
#include "token.h"
#include "../parse.cpp"

#include <iostream>
#include <string>
#include <vector>

std::vector<std::string> stack;

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

bool isType(std::string s){
    if(std::regex_match(s, std::regex("int|double|string|char|bool"))){
        return true;
    }
    return false;
}

class TID_tree
{
    struct TID
    {
        std::vector<std::string> name_;
        std::vector<std::string> type_;

        void push_name(std::string s, std::string t)
        {
            for (int i = 0; i < name_.size(); ++i)
            {
                if (s == name_[i])
                    throw std::logic_error("in line: " + std::to_string(num_of_line) + ". The name " + s + " has already use. ");
            }
            name_.push_back(s);
            type_.push_back(t);
        }

        std::string check_name(std::string s)
        {
            for (int i = 0; i < name_.size(); ++i)
            {
                if (s == name_[i])
                    return type_[i];
            }
            return "-_-";
        }

        void replace(std::string s, std::string t)
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
    };
    TID tid;
    TID_tree *next = nullptr, *prev = nullptr;

public:
    void create_TID()
    {
        TID_tree *now = new TID_tree;
        now->tid.name_.clear();
        now->tid.type_.clear();
        now->prev = cur_tid;
        cur_tid->next = now;
        cur_tid = now;
    }

    void del_TID()
    {
        TID_tree *p = tid_tree, *q;
        for (; p->next != cur_tid; p = p->next)
            ;
        p->next = nullptr;
        q = cur_tid;
        cur_tid = p;
        delete q;
    }

    std::string check_name(std::string s)
    {
        if (this == nullptr)
            throw std::logic_error("in line: " + std::to_string(num_of_line) + ". There is no identifier with the name " + s);
        if (this->tid.check_name(s) == "-_-")
        {
            this->prev->check_name(s);
        }
        else
        {
            return this->tid.check_name(s);
        }
    }

    bool find(std::string s)
    {
        if (this == nullptr)
            return false;
        if (this->tid.check_name(s) == "-_-")
        {
            this->prev->check_name(s);
        }
        else
        {
            return true;
        }
    }
    void replace(std::string s, std::string t)
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
};
