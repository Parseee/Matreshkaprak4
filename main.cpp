#include <iostream>
#include <string>
#include "io_kit.h"
#include "lexem_analyze.h"

int main() {
    setlocale(LC_ALL, "en-US.UTF-8");
    std::vector < std::pair < std::string, int >> lexes;
    try {
        lexes = lex_an(get_text("materials/prgrm.rus"));
    } catch (std::vector < std::invalid_argument > e) {
        for (int i = 0; i < e.size(); i++)
            std::cerr << "exception " << e[i].what() << "\n\n";
    }
    for (int i = 0; i < lexes.size(); ++i) {
        std::cout << lexes[i].first << " " << lexes[i].second << std::endl;
    }
}

/*
(pribav (telo (esli (<= +0  4)((max  6))) (ubav c66 +0)))
(func p9 () (not ywvo -9))
*/