#include "headers/parse.h"
#include "headers/token.h"

#include <vector>
#include <iostream>
#include <regex>

Token c;
size_t num_of_line = 1;

Parser::Parser (std::vector < Token > tokens) : 
                tokens(tokens), current(0) {}

Token Parser::gc() {
    if (!is_at_end()) {
        if (tokens[current].token == "\\n") {
            ++current;
            ++num_of_line;
        }
        return tokens[current++];
    } else {
        // maybe add another info
        throw std::logic_error("Unexpected end gc" + std::to_string(current));
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
        throw std::logic_error(c.token + " s_exp " + std::to_string(current));
    }
    c = gc();
    if (c.token == "func") {
        c = gc();
        func();
    } else {
        oper();
        if (c.token != ")") {
            throw std::logic_error(c.token + " s_exp " + std::to_string(current));
        }
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
    if (std::regex_match(c.token, std::regex("[+]|-|[*]|/|max|min"))) {
        c = gc();
        simple_oper();
    }
    // cond_oper ------------------------------
    else if (std::regex_match(c.token, std::regex("takzhe|libo|=|>|<|>=|<=|golova|telo"))) {
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
    // cons -----------------------------------
    else if (c.token == "partia") {
        c = gc();
        arg();
        while (c.token != ")") {
            arg();
        }
    } else {
        if (c.token == ")") { // all good
            return;
        } else if (c.level == 2) { // func_call
            c = gc();
            func_call();
        } else {
            throw std::logic_error(c.token + " bad oper " + std::to_string(current));
        }
    }
}

void Parser::simple_oper() {
    arg();
    while (c.token != ")") {
        arg();
    }
}

void Parser::cond_oper() {
    arg();
    while (c.token != ")") {
        arg();
    }
}

void Parser::loop_for() {
    if (c.level != 2) {
        throw std::logic_error(c.token + " var loop_for " + std::to_string(current));
    }
    c = gc();
    if (c.token != "ne_stanet") {
        throw std::logic_error(c.token + " ne_stanet loop_for " + std::to_string(current));
    }
    c = gc();
    arg();

    //c = gc();
    while (true) {
        if (c.token != "(") {
            throw std::logic_error(c.token + " loop_for open parent oper " + std::to_string(current));
        }
        c = gc();
        oper();
        if (c.token != ")") {
            throw std::logic_error(c.token + " loop_for close parent oper " + std::to_string(current));
        }
        c = gc();
        if (c.token == ")") {
            break;
        }
    }
}

void Parser::loop() {
    while (true) {
        if (c.token != "(") {
            throw std::logic_error(c.token + " loop open parent oper " + std::to_string(current));
        }
        c = gc();
        if (c.token == "umri_kogda") {
            break;
        }
        oper();
        if (c.token != ")") {
            throw std::logic_error(c.token + " loop close parent oper " + std::to_string(current));
        }
        c = gc();
    }

    c = gc();
    if (c.token != "(") {
        throw std::logic_error(c.token + " loop open parent cond_oper " + std::to_string(current));
    }

    c = gc();
    if (!std::regex_match(c.token, std::regex("takzhe|libo|=|/=|>|<|>=|<="))) {
        throw std::logic_error(c.token + " loop cond_oper condition " + std::to_string(current));
    }

    c = gc();
    cond_oper();

    c = gc();
    if (c.token != "(") {
        throw std::logic_error(c.token + " loop open return paret " + std::to_string(current)); 
    }

    c = gc();
    if (c.token != "verni") {
        throw std::logic_error(c.token + " loop retrun oper " + std::to_string(current));
    }

    ret_op();
    if (c.token != ")") { // end of return
        throw std::logic_error(c.token + " loop close return parent " + std::to_string(current));
    }

    c = gc(); // end of umri_kogda
    if (c.token != ")") {
        throw std::logic_error(c.token + " loop close umri_kogda parent " + std::to_string(current));
    }

    c = gc(); // end of loop
    if (c.token != ")") {
        throw std::logic_error(c.token + " loop close parent " + std::to_string(current));
    }
}

void Parser::if_op() {
    if (c.token != "(") {
        throw std::logic_error(c.token + " if open parent " + std::to_string(current));
    }
    
    c = gc();
    if (!std::regex_match(c.token, std::regex("takzhe|libo|=|/=|>|<|>=|<="))) {
        throw std::logic_error(c.token + " if condition " + std::to_string(current));
    }

    c = gc();
    cond_oper();

    // true opers
    c = gc();
    if (c.token != "(") {
        throw std::logic_error(c.token + " if true condition open parent " + std::to_string(current));
    }

    c = gc();
    while (true) {
        if (c.token != "(") {
            throw std::logic_error(c.token + " if true condition opers " + std::to_string(current));
        }
        c = gc();
        oper();
        if (c.token != ")") {
            throw std::logic_error(c.token + " if true condition opers " + std::to_string(current));
        }
        c = gc();
        if (c.token == ")") {
            break;
        }
    }

    // false opers
    c = gc();
    if (c.token != "(") {
        throw std::logic_error(c.token + " if false condition open parent " + std::to_string(current));
    }

    c = gc();
    while (true) {
        if (c.token != "(") {
            throw std::logic_error(c.token + " if false condition opers " + std::to_string(current));
        }
        c = gc();
        oper();
        if (c.token != ")") {
            throw std::logic_error(c.token + " if false condition opers " + std::to_string(current));
        }
        c = gc();
        if (c.token == ")") {
            break;
        }
    }

    c = gc();
    if (c.token != ")") {
        throw std::logic_error(c.token + " if close parent " + std::to_string(current));
    }
}

void Parser::write() {
    arg();
}

void Parser::read() {
    if (c.level != 2) {
        throw std::logic_error(c.token + " loop_for " + std::to_string(current));
    }
    c = gc();
}

void Parser::mod() {
    arg();
    arg();
}

void Parser::ne() {
    arg();
}

void Parser::incf() {
    arg();
    if (c.token != ")") {
        arg();
    }
}

void Parser::decf() {
    arg();
    if (c.token != ")") {
        arg();
    }
}

void Parser::ret_op() {
    c = gc();
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
    } else {
        throw std::logic_error(c.token + " can't assign to non-var " + std::to_string(current));
    }
}

void Parser::func() {
    if (c.level != 2) { // name
        throw std::logic_error(c.token + " no function name " + std::to_string(current));
    }

    c = gc();
    if (c.token != "(") {
        throw std::logic_error(c.token + " func open parent " + std::to_string(current));
    }

    c = gc();
    while (c.token != ")") {
        if (c.level != 2) {
            throw std::logic_error(c.token + " bad func argument " + std::to_string(current));
        }
        c = gc();
    }

    if (c.token != ")") {
        throw std::logic_error(c.token + " func arg close parent " + std::to_string(current));
    }

    c = gc();
    if (c.token != "(") {
        throw std::logic_error(c.token + " func open opers_group parent " + std::to_string(current));
    }
    
    c = gc();
    while (true) {
        if (c.token != "(") {
            throw std::logic_error(c.token + " func open opers parent " + std::to_string(current));
        }
        c = gc();
        oper();
        // c = gc();
        if (c.token != ")") {
            throw std::logic_error(c.token + " func close opers parent " + std::to_string(current));
        }
        c = gc();
        if (c.token == ")") {
            break;
        }
    }

    c = gc();
    if (c.token != "(") {
        throw std::logic_error(c.token + " func return open parent " + std::to_string(current));
    }
    c = gc();
    if (c.token != "verni") {
        throw std::logic_error(c.token + " no verni found " + std::to_string(current));
    }
    ret_op();
    if (c.token != ")") {
        throw std::logic_error(c.token + " func return close parent " + std::to_string(current));
    }

    c = gc();
    if (c.token != ")") {
        throw std::logic_error(c.token + " func close parent " + std::to_string(current));
    }
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
        arg();
    }
}

void Parser::func_call() {
    while (c.token != ")") {
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
        //c = gc();
        if (c.token != ")") {
            throw std::logic_error(c.token + " arg close oper parent" + std::to_string(current));
        }
        c = gc();
    } else if (c.token == ")") {
        return;
    } else {
        throw std::logic_error(c.token + " bad argument " + std::to_string(current));
    }
    return;
}