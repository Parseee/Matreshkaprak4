#pragma once

#include <iostream>
#include <string>
#include <vector>

extern size_t num_of_line;

// Defining the type of token
std::string what_type(std::string s);

// Checking whether a token is a type
bool isType(std::string s);

class TID_tree
{
public:
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
    };

    TID tid;
    TID_tree *next = nullptr, *prev = nullptr;

    // Adding a new TID to TID_tree and changing the current TID (cur_ID)
    void create_TID();

    // Deleting a current TID from TID_tree and changing the current TID (cur_ID)
    void del_TID();

    // Checking for the presence of an ID in the TID_tree
    std::string check_name(std::string s);

    // Checking for the presence of an identifier in TID_true (without throwing an error)
    bool find(std::string s);

    // Replacing the ID type in TID_tree
    void replace(std::string s, std::string t);
};

extern TID_tree *tid_tree, *cur_tid;