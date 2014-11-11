#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>

#include "lexer/lexer.hpp"
#include "lexer/stateomat.hpp"
#include "lexer/token.hpp"
#include "parser/parser.hpp"

#define CMD_LINE_OPTION_PREFIX "--"

int main(int argc, const char** argv)
{
	if (argc == 1)
	{
		std::cerr << "No file specified for compilation." << std::endl;
		return EXIT_FAILURE;
	}

	std::vector<std::string> options;

	for (int i = 1; i < argc - 1; i++)
		options.push_back(argv[i]);

	std::string file_name = argv[argc - 1];
	auto has_option = [&options] (std::string option)
	{
		return std::find(options.begin(), options.end(), CMD_LINE_OPTION_PREFIX + option) != options.end();
	};

	if (has_option("dumplexgraph"))
	{
		Stateomat stateomat;
		stateomat.dump_graph(file_name);
		return EXIT_SUCCESS;
	}

	std::ifstream infile(file_name);

	if (!infile.good())
	{
		std::cerr << "Error reading file " << file_name << std::endl;
		return EXIT_FAILURE;
	}

	Stateomat stateomat;

	if (has_option("lextest"))
	{
		Lexer lexer(infile, stateomat, true);
		Token t;

		do
		{
			t = lexer.get_next_token();
		}
		while (t.token_type != Token::Token_type::TOKEN_ERROR && t.token_type != Token::Token_type::TOKEN_EOF);

		if (t.token_type != Token::Token_type::TOKEN_EOF)
		{
			std::cerr << "Error: Lexer failed at line " << t.position.first << ", column " << t.position.second << std::endl;
			return EXIT_FAILURE;
		}
	}
	else
	{
		Lexer lexer(infile, stateomat, false);
		Parser parser(lexer, true);
		bool valid = parser.start();

		if (!valid)
			return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
