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

#include "firm_interface/FirmInterface.hpp"


int dumpLexGraph(lexer::Stateomat stateomat, std::string out_name)
{
	try
	{
		stateomat.dump_graph(out_name);
	}
	catch (std::string e)
	{
		std::cerr << e << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

int lexTest(lexer::Lexer lexer)
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

bool runSemanticAnalysis(shptr<ast::Program> root, shptr<ErrorReporter> errorReporter)
{
	SemanticAnalysis sa(root, errorReporter);
	bool valid = sa.start();
	errorReporter->printErrors();
	return valid;
}

void runFirm(std::string file_name, std::string out_name, shptr<ast::Program> root)
{
	FirmInterface::getInstance().setInput(file_name);
	FirmInterface::getInstance().setOutput(out_name);
	// TODO
	// Create instance of FirmVisitor / ProgramVisitor / whatever is suitable

	FirmInterface::getInstance().convert(root);
}

int compileAssembly(std::string out_name_assembly)
{
	std::cout << "Compiling" << std::endl;
	//TODO: may check if "gcc" is installed...or something.
	std::string cmd = "gcc " + out_name_assembly + " -o " + out_name_assembly.substr(0, out_name_assembly.find_last_of('.'));
	int ret = system(cmd.c_str());
	return ret;
}

int main(int argc, const char** argv)
{
	enum optionIndex {UNKNOWN, HELP, DUMPLEXGRAPH, LEXTEST, PRINT_AST, CHECK, SUPPRESS_ERRORS, FIRM, OUT, COMPILE_FIRM};
	static const option::Descriptor usage[] =
	{
		{UNKNOWN, 0, "", "", option::Arg::None, "USAGE: mj++ [option] FILE\n\nOptions:"},
		{HELP, 0, "h", "help", option::Arg::None, "  --help, -h\tPrint usage and exit."},
		{DUMPLEXGRAPH, 0, "d", "dumplexgraph", option::Arg::None, "  --dumplexgraph\tPrint automaton of lexer to the given file name."},
		{LEXTEST, 0, "l", "lextest", option::Arg::None, "  --lextest\tOnly run the lexer on the input FILE."},
		{PRINT_AST, 0, "p", "print-ast", option::Arg::None, "  --print-ast\tPretty prints the content of the abstract syntax tree after a file has been parsed."},
		{CHECK, 0, "c", "check", option::Arg::None, "  --check\tRuns the semantic analysis"},
		{SUPPRESS_ERRORS, 0, "s", "suppress-errors", option::Arg::None, "  --suppress-errors\tprevents any errors from being printed"},
		{FIRM, 0, "f", "firm", option::Arg::None, "  --firm\tInitialize libFirm"},
		{OUT, 0, "o", "out", option::Arg::Required, "  --out FILE\tSet the output for various commands to FILE"},
		{COMPILE_FIRM, 0, "cf", "compile-firm", option::Arg::None, "  --compile-firm\tRun the semantic analysis, build the FIRM-graph and produce backend-code using FIRM."},
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
	{
		option::printUsage(std::cout, usage);
		// optionally print error message
		return EXIT_FAILURE;
	}
	else if (options[HELP])
	{
		option::printUsage(std::cout, usage);
		return EXIT_SUCCESS;
	}
	else if (parse.nonOptionsCount() == 0)
	{
		std::cerr << "No file specified for compilation" << std::endl << std::endl;
		option::printUsage(std::cout, usage);
		return EXIT_FAILURE;
	}

	std::string file_name(parse.nonOption(0));
	std::string out_name;

	if (options[OUT])
		out_name = std::string(options[OUT].arg);
	else
		out_name = file_name.substr(0, file_name.find_last_of('.'));

	lexer::Stateomat stateomat;

	if (options[DUMPLEXGRAPH])
		return dumpLexGraph(stateomat, !options[OUT] ? "lexgraph.gml" : out_name);

	try
	{
		auto errorReporter = std::make_shared<ErrorReporter>(file_name, !options[SUPPRESS_ERRORS]);
		lexer::Lexer lexer(file_name.c_str(), stateomat, errorReporter);


		if (options[LEXTEST])
			return lexTest(lexer);

		Parser parser(lexer, errorReporter);
		bool valid = parser.start();

		if (valid && !parser.getRoot())
		{
			std::cerr << "Parsing was successful but there is no tree..." << std::endl;
			valid = false;
		}

		if (options[PRINT_AST] && valid)
			parser.getRoot()->toString(std::cout, 0);

		if (options[CHECK] && valid)
		{
			valid = runSemanticAnalysis(parser.getRoot(), errorReporter);

			if (options[FIRM] && valid)
				runFirm(file_name, out_name + (options[OUT] ? "" : ".S"), parser.getRoot());

		}
		else if (options[COMPILE_FIRM] && valid)
		{
			valid = runSemanticAnalysis(parser.getRoot(), errorReporter);

			if (valid)
			{
				std::string out_name_assembly = out_name + (options[OUT] ? "" : ".S");
				runFirm(file_name, out_name_assembly, parser.getRoot());
				compileAssembly(out_name_assembly);
			}
		}

		if (!valid)
			return EXIT_FAILURE;

		return EXIT_SUCCESS;
	}
	catch (std::string msg)
	{
		std::cout << msg << std::endl;
		return EXIT_FAILURE;
	}
}
