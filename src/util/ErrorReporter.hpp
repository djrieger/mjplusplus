#ifndef ERROR_REPORTER_HPP
#define ERROR_REPORTER_HPP

#include <map>
#include "../lexer/token.hpp"

class ErrorReporter
{
	public:
		enum ErrorType
		{
			LEXER,
			PARSER,
			SEMANTIC
		};
		void printError(ErrorReporter::ErrorType type, std::string const& error_msg, std::pair<unsigned int, unsigned int> position);
	private:
		std::multimap<std::pair<unsigned int, unsigned int>, std::pair<ErrorReporter::ErrorType, std::string>> errors;
};

#endif