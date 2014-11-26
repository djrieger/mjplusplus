#ifndef ERROR_REPORTER_HPP
#define ERROR_REPORTER_HPP

#include <map>
#include <regex>
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
		void recordError(ErrorReporter::ErrorType type, std::string const& error_msg, source_position_t position);
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
		ErrorReporter(std::string const& file_name, bool recordErrors);
	private:
		/*
		 * Regex to match $type{...} strings in error messages
		 */
		std::regex typeRegex;
		/*
		 * Regex to match $ident{...} strings in error messages
		 */
		std::regex identRegex;
		/*
		 * Format error_msg (colors/bold face for $type{...} and $ident{...} strings)
		 */
		void formatAndRecordError(ErrorReporter::ErrorType type, std::string const& error_msg, std::pair<int, unsigned int> position);
		/*
		 * ANSI Escape codes used for formatting messages with colors, bold fonts etc.
		 * See https://en.wikipedia.org/wiki/ANSI_escape_code
		 */
		static std::unordered_map<std::string, std::string> escapeCodes;
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
		/*
		 * Whether to record errors
		 */
		bool recordErrors;
};

#endif