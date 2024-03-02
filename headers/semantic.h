#pragma once

#include <iostream>
#include <string>
#include <vector>

extern size_t num_of_line;

// Defining the type of token
std::string what_type(std::string s);

// Checking whether a token is a type
bool isType(std::string s);

struct TID
{
    std::vector<std::string> name_;
    std::vector<std::string> type_;

    // Adding an identifier to the TID
    void push_name(std::string s, std::string t);

    // Checking for the presence of an ID in the TID
    std::string check_name(std::string s);

    // Replacing the ID type
    void replace(std::string s, std::string t);

    std::vector<std::string> get_names() { return name_; }
    
    std::vector<std::string> get_types() { return type_; }
};
struct func_TID
{
    std::vector<std::string> name_;
    std::vector<std::string> type_;
    std::vector<int> num_of_lex;
    std::vector<TID> tid_;

    // Adding an identifier of function to the TID
    void push_name(std::string s, int n, std::vector<std::string> p_name);

    // Checking for the presence of an ID in the TID
    std::string func_type(std::string s);

    // Checking for the presence of an identifier in func_TID (without throwing an error)
    bool find(std::string s);

    void set_type(std::string s, std::string t);

    void set_params_type(std::string s, std::vector<std::string> p_type);

    int get_idx(std::string s)
    {
        for (int i = 0; i < name_.size(); ++i)
            if (name_[i] == s)
                return i;
    }

    int get_num_of_lex(int i)
    {
        return num_of_lex[i];
    }

    TID get_tid_params(int i)
    {
        return tid_[i];
    }
};
class TID_tree
{
public:
    TID tid;
    TID_tree *next = nullptr, *prev = nullptr;

    // Adding a new TID to TID_tree and changing the current TID (cur_ID)
    void create_TID();

    void create_TID(TID tid);

    // Deleting a current TID from TID_tree and changing the current TID (cur_ID)
    void del_TID();

    // Checking for the presence of an ID in the TID_tree
    std::string check_name(std::string s);

    // Checking for the presence of an identifier in TID_tree (without throwing an error)
    bool find(std::string s);

    // Replacing the ID type in TID_tree
    void replace(std::string s, std::string t);
};

extern TID_tree *tid_tree, *cur_tid;
extern func_TID *func_tid;