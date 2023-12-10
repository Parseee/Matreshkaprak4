#include "headers/parse.h"
#include "headers/token.h"

#include <vector>
#include <iostream>
#include <regex>

Token c;

Parser::Parser (std::vector < Token > tokens) : 
                tokens(tokens), current(0) {}

Token Parser::gc() {
    if (!is_at_end()) {
        return tokens[current++];
    } else {
        // maybe add another info
        throw std::logic_error("Unexpected end gc" + std::to_string(current - 1));
    }
}

bool Parser::is_at_end() {
    return static_cast < size_t > (current) == tokens.size();
}

void Parser::parse() {
    c = gc();
    s_exp();
}

// <s_exp> ::= "(" <oper> ") " | <s_exp> 
void Parser::s_exp() {
    if (c.token != "(") {
        throw std::logic_error(c.token + " s_exp" + std::to_string(current - 1));
    }
    c = gc();
    if (c.token == "func") {
        c = gc();
        func();
        c = gc();
        return;
    }
    oper();
    c = gc();
    if (c.token != ")") {
        throw std::logic_error(c.token + " s_exp" + std::to_string(current - 1));
    }
    // for extending s_exp
    if (static_cast < size_t > (current) < tokens.size()) {
        c = gc();
        s_exp();
    }
}

// <oper> ::= <easy_oper> | <hard_oper> | <func> | <car> | <cdr> | <empty>
void Parser::oper() {
    // simple_oper ----------------------------
    if (std::regex_match(c.token, std::regex("/+|-|/*|max|min"))) {
        c = gc();
        simple_oper();
    }
    // cond_oper ------------------------------
    else if (std::regex_match(c.token, std::regex("takzhe|libo|=|>|<|>=|<=|cons|golova|telo"))) {
        c = gc();
        cond_oper();
    }
    // hard_oper ------------------------------
    // loop_for ---------------
    else if (c.token == "idi_poka") {
        c = gc();
        loop_for();
    }
    // loop -------------------
    else if (c.token == "zhivi") {
        c = gc();
        loop();
    }
    // if ---------------------
    else if (c.token == "esli") {
        c = gc();
        if_op();
    }
    // write ------------------
    else if (c.token == "napishi") {
        c = gc();
        write();
    }
    // read -------------------
    else if (c.token == "sprosi") {
        c = gc();
        read();
    }
    // mod --------------------
    else if (c.token == "mod") {
        c = gc();
        mod();
    }
    // not --------------------
    else if (c.token == "ne") {
        c = gc();
        ne();
    }
    // incf -------------------
    else if (c.token == "pribav") {
        c = gc();
        incf();
    }
    // decf -------------------
    else if (c.token == "ubav") {
        c = gc();
        decf();
    }
    // return -----------------
    else if (c.token == "verni") {
        c = gc();
        ret_op();
    }
    // fact -------------------
    else if (c.token == "!") {
        c = gc();
        fact();
    }
    // setf -------------------
    else if (c.token == "tovarisch") {
        c = gc();
        setf();
    }
    // car ------------------------------------
    else if (c.token == "golova") {
        c = gc();
        s_exp();
    }
    // cdr ------------------------------------
    else if (c.token == "telo") {
        c = gc();
        s_exp();
    }
    // cons -----------------------------------
    else if (c.token == "partia") {
        c = gc();
        if (c.token != "(") {
            throw std::logic_error(c.token + " partia " + std::to_string(current - 1));
        }
        c = gc();
        arg();
        while (c.token != ")") {
            arg();
        }
    }
    else {
        // all good
        return;
    }
    return;
}

void Parser::simple_oper() {
    arg();
    while (c.token != ")") {
        arg();
    }
    return;
}

void Parser::cond_oper() {
    arg();
    while (c.token != ")") {
        arg();
    }
    return;
}

void Parser::loop_for() {
    if (c.level == 2 || c.level == 3) {
        c = gc();
        if (c.token == "ne_stanet") {
            c = gc();
            if (c.level == 2 || c.level == 3) {
                c = gc();
                oper();
                c = gc();
            }
        }
    }
    return;
}

void Parser::loop() {
    c = gc();
    oper();
    c = gc();
    if (c.token != "(") {
        throw std::logic_error(c.token + " loop" + std::to_string(current - 1));
    }
    
    c = gc();
    if (c.token != "umri_kogda") {
        throw std::logic_error(c.token + " loop" + std::to_string(current - 1));
    }

    c = gc();
    if (c.token != "(") {
        throw std::logic_error(c.token + " loop" + std::to_string(current - 1));
    }

    c = gc();
    if (!std::regex_match(c.token, std::regex("takzhe|libo|=|/=|>|<|>=|<="))) {
        throw std::logic_error(c.token + " loop" + std::to_string(current - 1));
    }

    c = gc();
    // if we cant compare a lot of operands switch while to if
    while (c.token != ")") {
        arg();
    }
    
    c = gc();
    if (c.token != "(") {
        throw std::logic_error(c.token + " loop" + std::to_string(current - 1)); 
    }

    c = gc();
    if (c.token != "verni") {
        throw std::logic_error(c.token + " loop" + std::to_string(current - 1));
    }

    c = gc(); // get (
    ret_op();
    if (c.token != ")") { // end of return
        throw std::logic_error(c.token + " loop" + std::to_string(current - 1));
    }

    c = gc(); // end of loop
    if (c.token != ")") {
        throw std::logic_error(c.token + " loop" + std::to_string(current - 1));
    }
}

void Parser::if_op() {
    if (c.token != "(") {
        throw std::logic_error(c.token + " if" + std::to_string(current - 1));
    }
    
    c = gc();
    if (!std::regex_match(c.token, std::regex("takzhe|libo|=|/=|>|<|>=|<="))) {
        throw std::logic_error(c.token + " if" + std::to_string(current - 1));
    }

    c = gc();
    arg();
    while (c.token != ")") {
        // c = gc();
        arg();
    }

    if (c.token != ")") {
        throw std::logic_error(c.token + " if" + std::to_string(current - 1));
    }

    c = gc();
    if (c.token != "(") {
        throw std::logic_error(c.token + " if" + std::to_string(current - 1));
    }

    c = gc();
    oper();
    if (c.token != ")") {
        throw std::logic_error(c.token + " if" + std::to_string(current - 1));
    }
    s_exp();
}

void Parser::write() {
    arg();
}

void Parser::read() {
    arg();
}

void Parser::mod() {
    arg();
}

void Parser::ne() {
    arg();
}

void Parser::incf() {
    arg();
    if (c.token != ")") {
        //c = gc();
        arg();
    }
}

void Parser::decf() {
    arg();
    if (c.token != ")") {
        //c = gc();
        arg();
    }
}

void Parser::ret_op() {
    arg();
}

void Parser::fact() {
    arg();
    while (c.token != ")") {
        c = gc();
        arg();
    }
}

void Parser::setf() {
    if (c.level == 2) {
        c = gc();
        arg();
    }
}

void Parser::func() {
    if (c.level != 2) { // name
        throw std::logic_error(c.token + " no function name " + std::to_string(current - 1));
    }

    c = gc();
    if (c.token != "(") {
        throw std::logic_error(c.token + " bad func parenthesis " + std::to_string(current - 1));
    }

    c = gc();
    while (c.token != ")") {
        if (c.level != 2 && c.level != 3) {
            throw std::logic_error(c.token + " bad func arg arguments " + std::to_string(current - 1));
        }
        c = gc();
    }

    if (c.token != ")") {
        throw std::logic_error(c.token + " bad func arg parenthesis " + std::to_string(current - 1));
    }

    c = gc();
    oper();
}

void Parser::car() {
    arg();
}

void Parser::cdr() {
    arg();
}

void Parser::cons() {
    arg();

    while (c.token != ")") {
        // c = gc();
        arg();
    }
}

// <argf> ::= <var> | <sign> <num> | "(" <oper> ")"
void Parser::arg() {
    // literal or var name
    if (c.level == 2 || c.level == 3) {
        c = gc();
        // all good
    } else if (c.token == "(") {
        c = gc();
        oper();
        c = gc();
        if (c.token != ")") {
            throw std::logic_error(c.token + " arg " + std::to_string(current - 1));
        }
    } else if (c.token == ")") {
        return;
    } else {
        throw std::logic_error(c.token + " arg " + std::to_string(current - 1));
    }
    return;
}