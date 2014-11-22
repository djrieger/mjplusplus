#ifndef ERROR_REPORTER_HPP
#define ERROR_REPORTER_HPP

#include <map>
#include "../lexer/token.hpp"

/**
 * A common error reporter which can be used for lexer, parser and semantic errors
 */
class ErrorReporter
{
	public:
		enum ErrorType
		{
			LEXER,
			PARSER,
			SEMANTIC
		};
		/*
		 * Record an error in the errors multimap
		 *
		 * @type Error type
		 * @error_msg Error message to be printed
		 * @position line and column where the error occurred
		 */
		void recordError(ErrorReporter::ErrorType type, std::string const& error_msg, std::pair<unsigned int, unsigned int> position);
		/**
		 * Print all errors stored in the errors multimap to std::cerr. This should be called after parsing and semantic analysis are complete.
		 */
		void printErrors() const;
		/*
		 * Initialize ErrorReporter for source file with name file_name
		 */
		ErrorReporter(std::string const& file_name);
	private:
		/*
		 * Errors reported by Parser or SemanticAnalysis
		 * key: (line, column)
		 * value: (ErrorReporter::ErrorType, std::string)
		 */
		std::multimap<std::pair<unsigned int, unsigned int>, std::pair<ErrorReporter::ErrorType, std::string>> errors;
		/*
		 * name of input file
		 */
		std::string const& file_name;
};

#endif