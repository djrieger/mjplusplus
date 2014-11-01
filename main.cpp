#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>

#include "lexer/lexer.hpp"
#include "lexer/stateomat.hpp"
#include "lexer/token.hpp"

#define CMD_LINE_OPTION_PREFIX "--"

int main(int argc, const char** argv)
{
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
	}
	else if (has_option("lextest"))
	{
		std::ifstream infile(file_name);

		if (!infile.good())
		{
			std::cerr << "Error reading file " << file_name << std::endl;
			return EXIT_FAILURE;
		}

		Stateomat stateomat;
		Lexer lexer(infile, stateomat, true);
		Token t = lexer.get_next_token();

		while (t.type != Token::Type::TOKEN_ERROR)
		{
			if (t.type == Token::Type::TOKEN_EOF)
				break;

			t = lexer.get_next_token();
		}

		infile.close();

		if (t.type != Token::Type::TOKEN_EOF)
		{
			std::cerr << "Error: Lexer failed." << std::endl;
			return EXIT_FAILURE;
		}
	}

	return EXIT_SUCCESS;
}
