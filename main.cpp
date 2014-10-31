#include <iostream>
#include <fstream>
#include <map>
#include <set>
#include <vector>
#include <cstdlib>

#include <string.h>

#include "lexer/stateomat.hpp"
#include "lexer/token.hpp"
#include "lexer/lexer.hpp"

int main(int argc, const char *argv[]) {

    if (argc == 3 && std::string(argv[1]) == "--dumplexgraph") {
        Stateomat stateomat;
        stateomat.dump_graph(std::string(argv[2]));
    }
    else {
        bool debug = false;
        if (argc == 3 && strncmp(argv[1], "--lextest", 10) == 0)
        {
            debug = true;
        }

        std::ifstream infile(argv[2]);
        if (!infile.good())
        {
            std::cerr << "Error reading file." << std::endl;
            return 1;
        }

        Stateomat stateomat;
        Lexer lexer(infile, stateomat, debug);
        token t = lexer.get_next_token();
        while (t.type != TOKEN_ERROR)
        {
            if (t.type == TOKEN_EOF)
                break;
            t = lexer.get_next_token();
        }

        infile.close();

        if (t.type != TOKEN_EOF)
        {
            std::cerr << "Error: Lexer failed." << std::endl;
            return 1;
        }
        return 0;
    }

    return EXIT_SUCCESS;
}
