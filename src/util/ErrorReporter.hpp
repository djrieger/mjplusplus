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
		/*
		 * Record an error without a position in the errors multimap
		 *
		 * @type Error type
		 * @error_msg Error message to be printed
		 */
		void recordError(ErrorReporter::ErrorType type, std::string const& error_msg);
		/**
		 * Print all errors stored in the errors multimap to std::cerr. This should be called after parsing and semantic analysis are complete.
		 */
		void printErrors() const;
		/*
		 * Initialize ErrorReporter for source file with name file_name
		 */
		ErrorReporter(std::string const& file_name);

		static std::string formatType(std::string const& typeName);
		static std::string formatIdent(std::string const& typeName);
	private:
		/*
		 * Errors reported by Parser or SemanticAnalysis
		 * key: (line, column)
		 * value: (ErrorReporter::ErrorType, std::string)
		 *
		 * key is -1 if an error has no position
		 */
		std::multimap<std::pair<int, unsigned int>, std::pair<ErrorReporter::ErrorType, std::string>> errors;
		/*
		 * name of input file
		 */
		std::string const& file_name;

		static  std::unordered_map<std::string, std::string> escapeCodes;

};

#endif