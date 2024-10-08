#include "headers/io_kit.h"
#include "headers/lexem_analyze.h"
#include "headers/token.h"
#include "headers/parse.h"

#include <iostream>
#include <string>

size_t num_of_line = 1;

int main()
{
    setlocale(LC_ALL, "en-US.UTF-8");
    std::vector<Token> lexes;
    try
    {

        lexes = lex_an(read_file("../materials/prgrm.rus"));
    }
    catch (std::vector<std::invalid_argument> e)
    {
        for (int i = 0; i < e.size(); i++)
            std::cerr << "exception " << e[i].what() << "\n\n";
    }
    for (int i = 0; i < lexes.size(); ++i)
    {
        std::cout << i + 1 << ":  " << lexes[i].token << " " << lexes[i].level << std::endl;
    }
    try
    {
        Parser parser(lexes);
        parser.parse();
    }
    catch (std::logic_error e)
    {
        std::cerr << "Exception " << e.what() << std::endl;
        return 0;
    }
    catch (const std::overflow_error &e)
    {
        std::cerr << "Unknown exception" << std::endl;
    }
    std::cout << "Syntax analysis complete sucsessfully\n";
}

/*
(pribav (telo (esli (<= +0  4)((max  6))) (ubav c66 +0)))
(func p9 () (not ywvo -9))
*/

// added comment