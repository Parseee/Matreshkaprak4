#pragma once

#include <iostream>
#include <string>
#include <vector>
#include "semantic.h"
extern func_TID *func_tid;

class POLIZ
{
private:
    std::vector<std::vector<std::string>> funcs_;
    std::vector<std::string> vec_;
    std::vector<std::string> stack;
    bool isFuncCal = false;

public:
    POLIZ() = default;
    ~POLIZ() = default;

    std::vector<std::string> getPOLIZ(){ return vec_; }

    void show()
    {
        std::cout << "________________\n";
        for (auto it : funcs_)
        {
            std::cout << "( ";
            for (auto jt : it)
            {
                std::cout << jt << " ";
            }
            std::cout << ") ";
        }
        std::cout << "\n________________\n";
        for (auto it : vec_)
        {
            std::cout << it << " ";
        }
        std::cout << "\n________________\n";
    }

    void inFunc()
    {
        isFuncCal = true;
        stack = {};
    }

    void outFunc()
    {
        isFuncCal = false;
        funcs_.push_back(stack);
        stack.clear();
    }

    void outFuncCall(std::string n)
    {
        isFuncCal = false;
        funcs_[func_tid->get_idx(n)] = stack;
        stack.clear();
    }

    int size()
    {
        return vec_.size();
    }

    void add_lex(std::string s)
    {
        if (!isFuncCal)
            vec_.push_back(s);
        else
            stack.push_back(s);
    }

    void insert_lex(std::string s, int idx)
    {
        if (!isFuncCal)
            vec_[idx] = s;
    }

    void push_stack(std::string s)
    {
        if (!isFuncCal)
            stack.push_back(s);
    }

    std::string pop_stack()
    {
        if (!isFuncCal)
        {
            std::string r = stack.back();
            stack.pop_back();
            return r;
        }
        return "";
    }

    std::vector<std::vector<std::string>> get_func_info(std::string name)
    {
        std::vector<std::vector<std::string>> res;
        std::vector<std::string> tmp;
        tmp.push_back(name);
        res.push_back(tmp);
        tmp.clear();
        tmp.push_back(func_tid->func_type(name));
        res.push_back(tmp);
        tmp.clear();
        auto tmp_tid = func_tid->get_tid_params(func_tid->get_idx(name));
        res.push_back(tmp_tid.get_names());
        res.push_back(tmp_tid.get_types());
        res.push_back(funcs_[func_tid->get_idx(name)]);
        return res;
    }
};
