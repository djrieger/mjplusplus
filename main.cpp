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

	Stateomat stateomat;
	Lexer lexer(file_name.c_str(), stateomat);

	if (has_option("lextest"))
	{
		Token t(lexer.get_next_token());

		while (t.token_type != Token::Token_type::TOKEN_ERROR && t.token_type != Token::Token_type::TOKEN_EOF)
		{
			t.print();
			t = lexer.get_next_token();
		}

		if (t.token_type != Token::Token_type::TOKEN_EOF)
		{
			std::cerr << "Error: Lexer failed at line " << t.position.first << ", column " << t.position.second << std::endl;
			return EXIT_FAILURE;
		}

		return EXIT_SUCCESS;
	}

	Parser parser(lexer, true);
	bool valid = parser.start();

	if (!valid)
		return EXIT_FAILURE;

	if (has_option("print-ast"))
		parser.getRoot()->toString();

	return EXIT_SUCCESS;
}
