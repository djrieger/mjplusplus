#include "ErrorReporter.hpp"

void ErrorReporter::printError(ErrorReporter::ErrorType type, std::string const& error_msg, std::pair<unsigned int, unsigned int> position)
{
	errors.insert(std::pair<std::pair<unsigned int, unsigned int>, std::pair<ErrorReporter::ErrorType, std::string>>(position, std::pair<ErrorReporter::ErrorType, std::string>(type, error_msg)));
}

void ErrorReporter::printErrors() const
{
	for (auto& error : errors)
	{
		switch (error.second.first)
		{
			case ErrorReporter::ErrorType::SEMANTIC:
				std::cerr << "\033[1;31mSemantic error\033[0m ";
				break;

			case ErrorReporter::ErrorType::PARSER:
				std::cerr << "\033[1;Parser error\033[0m ";
				break;

			case ErrorReporter::ErrorType::LEXER:
				std::cerr << "\033[1;Lexer error\033[0m ";
				break;
		}

		std::cerr << "at line " << error.first.first << ", column " << error.first.second << ": " << error.second.second << std::endl;;
	}

	if (errors.size() > 0)
		std::cerr << "Error during compilation" << std::endl;
}