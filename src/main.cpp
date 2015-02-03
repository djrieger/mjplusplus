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

#include "firm_interface/optimizer/BasicInliner.hpp"


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
	semantic::SemanticAnalysis sa(root, errorReporter);
	bool valid = sa.start();
	errorReporter->printErrors();
	return valid;
}

void runFirm(std::string file_name, std::string out_name, bool outputGraphs, shptr<ast::Program> root)
{
	firm::FirmInterface::getInstance().setInput(file_name);
	firm::FirmInterface::getInstance().setOutput(out_name);
	firm::FirmInterface::getInstance().setFirmGraphOutput(outputGraphs);
	firm::FirmInterface::getInstance().convert(root);
}

void buildWithFirm()
{
	firm::FirmInterface::getInstance().buildWithFirm();
}

void build()
{
	firm::FirmInterface::getInstance().build();
}

int compileAssembly(std::string out_name_assembly, std::string out_name_file)
{
	std::cout << "Compiling" << std::endl;
	//TODO: may check if "gcc" is installed...or something.
#ifndef __APPLE__
	std::string cmd = "gcc " + out_name_assembly + " -o " + out_name_file;
#else
	std::string cmd = "gcc -Wl,-no_pie " + out_name_assembly + " -o " + out_name_file;
#endif
	int ret = system(cmd.c_str());
	return ret;
}

int removeAssembly(std::string out_name_assembly)
{
	std::string cmd = "rm " + out_name_assembly;
	int ret = system(cmd.c_str());
	return ret;
}

bool isNumber(const std::string& s)
{
	return !s.empty() && std::find_if(s.begin(),
	                                  s.end(), [](char c)
	{
		return !std::isdigit(c);
	}) == s.end();
}

int main(int argc, const char** argv)
{
	enum optionIndex {UNKNOWN, HELP, DUMP_LEX_GRAPH, DUMP_FIRM_GRAPH, PRINT_AST,
	                  SUPPRESS_ERRORS, OPTIMIZATION, OUT, KEEP, COMPILE_FIRM, LEXTEST, PARSE, CHECK, FIRM, ASSEMBLY
	                 };
	static const option::Descriptor usage[] =
	{
		{UNKNOWN, 0, "", "", option::Arg::None, "USAGE: mj++ [options] FILE\n\nOptions:"},
		{HELP, 0, "h", "help", option::Arg::None, " -h  --help\tPrint usage and exit"},
		{DUMP_LEX_GRAPH, 0, "D", "dumplexgraph", option::Arg::None, " -D  --dumplexgraph\tPrint automaton of lexer to the given file name and exit"},

		{UNKNOWN, 0, "", "", option::Arg::None, "\nRun options:"},
		{LEXTEST, 0, "l", "lextest", option::Arg::None, " -l  --lextest\tOnly run the lexer"},
		{PARSE, 0, "s", "parse", option::Arg::None, " -s  --parse\tStop after syntax analysis"},
		{CHECK, 0, "c", "check", option::Arg::None, " -c  --check\tStop after semantic analysis"},
		{FIRM, 0, "g", "graph", option::Arg::None, " -g  --firm\tStop after generating Firm graphs"},
		{ASSEMBLY, 0, "a", "assembly", option::Arg::None, " -a  --assembly\tStop after generating assembler (default output file: input_without_ending.S)"},
		{UNKNOWN, 0, "", "", option::Arg::None, "If none of these options is given, generate an executable (default output file: a.out)"},

		{UNKNOWN, 0, "", "", option::Arg::None, "\nOther options:"},
		{PRINT_AST, 0, "p", "print-ast", option::Arg::None, " -p  --print-ast\tAfter parsing, pretty print the abstract syntax tree"},
		{OPTIMIZATION, 0, "O", "optimize", option::Arg::Required, " -O  --optimize X\tSet the optimization flag to X. (X needs to be a non-negative integer.)"},
		{SUPPRESS_ERRORS, 0, "q", "suppress-errors", option::Arg::None, " -q  --suppress-errors \tDo not print error messages"},
		{KEEP, 0, "k", "keep", option::Arg::None, " -k  --keep\tKeep the assembly file after compiling"},
		{COMPILE_FIRM, 0, "f", "compile-firm", option::Arg::None, " -f  --compile-firm\tGenerate assembler using the Firm backend instead of our own codegen"},
		{DUMP_FIRM_GRAPH, 0, "d", "dumpfirmgraph", option::Arg::None, " -d  --dumpfirmgraph\tOutput the created firm graphs as vcg-files."},
		{OUT, 0, "o", "out", option::Arg::Required, " -o  --out FILE\tSet the output for various commands to FILE"},
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

	if (options[DUMP_LEX_GRAPH])
		return dumpLexGraph(stateomat, !options[OUT] ? "lexgraph.gml" : out_name);

	try
	{
		auto errorReporter = std::make_shared<ErrorReporter>(file_name, !options[SUPPRESS_ERRORS]);

		//lexer
		lexer::Lexer lexer(file_name.c_str(), stateomat, errorReporter);

		if (options[LEXTEST])
			return lexTest(lexer);

		//parser
		Parser parser(lexer, errorReporter);
		bool valid = parser.start();

		if (valid && !parser.getRoot())
		{
			std::cerr << "Parsing was successful but there is no tree..." << std::endl;
			valid = false;
		}

		if (options[PRINT_AST] && valid)
			parser.getRoot()->toString(std::cout, 0);

		if (options[PARSE] && valid)
			return EXIT_SUCCESS;
		//sementic analysis
		else if (valid)
			valid = runSemanticAnalysis(parser.getRoot(), errorReporter);

		if (!valid)
		{
			errorReporter->printErrors();
			return EXIT_FAILURE;
		}
		else if (options[CHECK])
			return EXIT_SUCCESS;

		//firm
		std::string out_name_assembly = out_name + (options[OUT] && options[ASSEMBLY] ? "" : ".S");

		if (options[OPTIMIZATION])
		{
			std::string flag = options[OPTIMIZATION].arg;

			if (isNumber(flag))
			{
				int flags = std::stoi(flag);

				if (flags && options[COMPILE_FIRM])
					flags |= firm::FirmInterface::OptimizationFlags::FIRM_COMPATIBLE;

				firm::FirmInterface::getInstance().setOptimizationFlag(flags);
			}
			else
			{
				std::cerr << "Invalid optimization flag" << std::endl << std::endl;
				option::printUsage(std::cout, usage);
				return EXIT_FAILURE;
			}
		}
		else
			firm::FirmInterface::getInstance().setOptimizationFlag(firm::FirmInterface::OptimizationFlags::FIRM_COMPATIBLE);

		runFirm(file_name, out_name_assembly, options[DUMP_FIRM_GRAPH], parser.getRoot());

		firm::BasicInliner bi;
		bi.run();

		if (options[FIRM])
			return EXIT_SUCCESS;

		//assembly
		if (options[COMPILE_FIRM])
			buildWithFirm();
		else
			build();

		if (options[ASSEMBLY])
			return EXIT_SUCCESS;

		//executable
		std::string out_name_file = options[OUT] ? out_name : "a.out";
		compileAssembly(out_name_assembly, out_name_file);

		if (!options[KEEP])
			removeAssembly(out_name_assembly);

		return EXIT_SUCCESS;
	}
	catch (std::string msg)
	{
		std::cout << msg << std::endl;
		return EXIT_FAILURE;
	}
}
