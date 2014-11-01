#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>

#include "lexer/lexer.hpp"
#include "lexer/stateomat.hpp"
#include "lexer/token.hpp"

using namespace std;

int main(int argc, const char *argv[])
{

    vector<string> options;
    for (int i = 1; i < argc - 1; i++)
        options.push_back(string(argv[i]));

    string file_name = string(argv[argc - 1]);

    auto has_option = [&options] (string option) {
        return find(options.begin(), options.end(), option) != options.end();
    };

    if (has_option("--dumplexgraph"))
    {
        Stateomat stateomat;
        stateomat.dump_graph(file_name);
    }
    else if (has_option("--lextest"))
    {
        ifstream infile(file_name);
        if (!infile.good())
        {
            cerr << "Error reading file." << endl;
            return EXIT_FAILURE;
        }

        Stateomat stateomat;
        Lexer lexer(infile, stateomat, true);
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
            cerr << "Error: Lexer failed." << endl;
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}
