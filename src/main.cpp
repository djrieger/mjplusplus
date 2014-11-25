#include <algorithm>
#include <fstream>
#include <iostream>
#include <vector>

#include "lexer/lexer.hpp"
#include "lexer/stateomat.hpp"
#include "lexer/token.hpp"
#include "parser/parser.hpp"
#include "semantic_analysis/SemanticAnalysis.hpp"
#include "util/optionparser.h"
#include "util/ErrorReporter.hpp"


int main(int argc, const char** argv)
{
	enum optionIndex {UNKNOWN, HELP, DUMPLEXGRAPH, LEXTEST, PRINT_AST, CHECK, SUPPRESS_ERRORS};
	static const option::Descriptor usage[] =
	{
		{UNKNOWN, 0, "", "", option::Arg::None, "USAGE: mj++ [option] FILE\n\nOptions:"},
		{HELP, 0, "-h", "help", option::Arg::None, "  --help, -h\tPrint usage and exit."},
		{DUMPLEXGRAPH, 0, "", "dumplexgraph", option::Arg::None, "  --dumplexgraph\tPrint automaton of lexer to the given file name."},
		{LEXTEST, 0, "", "lextest", option::Arg::None, "  --lextest\tOnly run the lexer on the input FILE."},
		{PRINT_AST, 0, "", "print-ast", option::Arg::None, "  --print-ast\tPretty prints the content of the abstract syntax tree after a file has been parsed."},
		{CHECK, 0, "", "check", option::Arg::None, "  --check\tRuns the semantic analysis"},
		{SUPPRESS_ERRORS, 0, "", "suppress-errors", option::Arg::None, "--suppress-errors\tprevents any errors from being printed"},
		{UNKNOWN, 0, "", "", option::Arg::None, "If no option is given, the parser will be run in silent mode."},
		{0, 0, 0, 0, 0, 0}
	};

	argc -= (argc > 0);
	argv += (argc > 0); // skip program name argv[0] if present
	option::Stats stats(usage, argc, argv);
	std::vector<option::Option> options(stats.options_max);
	std::vector<option::Option> buffer(stats.buffer_max);
	option::Parser parse(usage, argc, argv, &options[0], &buffer[0]);

	if (parse.error())
		// optionally print error message
		return EXIT_FAILURE;

	if (options[HELP])
	{
		option::printUsage(std::cout, usage);
		return EXIT_SUCCESS;
	}

	if (argc == 0)
	{
		std::cerr << "No file specified for compilation" << std::endl << std::endl;
		option::printUsage(std::cout, usage);
		return EXIT_FAILURE;
	}

	// iterate over options to detect if multiple options are specified
	int num_options = 0;

	for (int begin = HELP; begin <= CHECK; ++begin)
		num_options += options[begin] ? 1 : 0;

	// require exactly one of the options
	if (num_options <= 2)
	{
		/*
		if (argc - num_options != 2)
		{
			std::cout << "Missing file." << std::endl;
			return EXIT_FAILURE;
		}*/

		std::string file_name = argv[argc - 1];
		lexer::Stateomat stateomat;

		if (options[DUMPLEXGRAPH])
		{
			try
			{
				stateomat.dump_graph(file_name);
			}
			catch (std::string e)
			{
				std::cerr << e << std::endl;
			}

			return EXIT_SUCCESS;
		}

		try
		{
			auto errorReporter = std::make_shared<ErrorReporter>(file_name, !options[SUPPRESS_ERRORS]);
			lexer::Lexer lexer(file_name.c_str(), stateomat, errorReporter);


			if (options[LEXTEST])
			{
				lexer::Token t(lexer.get_next_token());

				while (t.token_type != lexer::Token::Token_type::TOKEN_ERROR && t.token_type != lexer::Token::Token_type::TOKEN_EOF)
				{
					t.print();
					t = lexer.get_next_token();
				}

				if (t.token_type != lexer::Token::Token_type::TOKEN_EOF)
				{
					std::cerr << "Error: Lexer failed at line " << t.position.first << ", column " << t.position.second << std::endl;
					return EXIT_FAILURE;
				}

				return EXIT_SUCCESS;
			}

			Parser parser(lexer, errorReporter);
			bool valid = parser.start();

			if (options[CHECK] && parser.getRoot())
			{
				SemanticAnalysis sa(parser.getRoot(), errorReporter);

				if (!sa.start())
					valid = false;
			}

			if (options[PRINT_AST] && parser.getRoot())
				parser.getRoot()->toString(std::cout, 0);

			errorReporter->printErrors();

			if (!valid)
				return EXIT_FAILURE;
			else
				return EXIT_SUCCESS;

		}
		catch (std::string msg)
		{
			std::cout << msg << std::endl;
			return EXIT_FAILURE;
		}

	}
	else
	{
		std::cout << "You can only specify up to 1 option. See --help for usage." << std::endl;
		return EXIT_FAILURE;
	}
}
