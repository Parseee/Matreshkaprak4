#pragma once

#include "token.h"

#include <iostream>
#include <vector>

class Parser
{
public:
    Parser(std::vector < Token > tokens);
    void parse();
    bool is_at_end();
    int getNumLine(int x);

private:
    int current;
    std::vector < Token > tokens;

    void s_exp();
    void oper();
    void arg();
    void simple_oper();
    void cond_oper();
    void loop_for();
    void loop();
    void if_op();
    void write();
    void read();
    void mod();
    void ne();
    void incf();
    void decf();
    void ret_op();
    void to_str();
    void fact();
    void setf();
    void func();
    void car();
    void cdr();
    void cons();
    void check_func(std::string name, std::vector<std::string> param_type);
    void func_call();
    void arg_sem();

    Token gc();
    //Token previous();
    //Token advance();

    //bool check(TokenType type);
    //bool match(TokenType type);
    //Token consume(TokenType type, std::string message);
    //std::runtime_error error(Token token, std::string message);
};

bool parse();


/*
(tovarisch x 11)
(tovarisch y 11)
(tovarisch z 11)
(esli (< z (+ (* x y) 5)) ((tovarisch a (< x y)) (tovarisch z (/ (+ x 6) (- a y))))((tovarisch z (* y 2))))
(takzhe PRAVDA PRAVDA PRAVDA (libo PRAVDA (libo (takzhe (libo PRAVDA PRAVDA) (libo PRAVDA PRAVDA)) (takzhe PRAVDA PRAVDA))))
*/