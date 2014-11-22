#include "ErrorReporter.hpp"

void ErrorReporter::printError(ErrorReporter::ErrorType type, std::string const& error_msg, std::pair<unsigned int, unsigned int> position)
{
	errors.insert(std::pair<std::pair<unsigned int, unsigned int>, std::pair<ErrorReporter::ErrorType, std::string>>(position, std::pair<ErrorReporter::ErrorType, std::string>(type, error_msg)));

	std::cerr << "\033[1;31mSemantic error\033[0m at line " << position.first << ", column " << position.second << ": " << error_msg << std::endl;
}