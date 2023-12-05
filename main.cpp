#include "io_kit.h"
#include "lexem_analyze.h"
#include "token.h"

#include <iostream>
#include <string>

int main() {
    setlocale(LC_ALL, "en-US.UTF-8");
    std::vector < Token > lexes;
    try {
        lexes = lex_an(read_file("materials/prgrm.rus"));
    } catch (std::vector < std::invalid_argument > e) {
        for (int i = 0; i < e.size(); i++)
            std::cerr << "exception " << e[i].what() << "\n\n";
    }
    for (int i = 0; i < lexes.size(); ++i) {
        std::cout << lexes[i].token << " " << lexes[i].level << std::endl;
    }
}

/*
(pribav (telo (esli (<= +0  4)((max  6))) (ubav c66 +0)))
(func p9 () (not ywvo -9))
*/

void foo() {
    std::cout << "hfbvi";
    return;
}