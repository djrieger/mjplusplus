#include <fstream>
#include <algorithm>
#include "ErrorReporter.hpp"

ErrorReporter::ErrorReporter(std::string const& file_name): file_name(file_name)
{

}

void ErrorReporter::recordError(ErrorReporter::ErrorType type, std::string const& error_msg, std::pair<unsigned int, unsigned int> position)
{
	errors.insert(std::pair<std::pair<int, unsigned int>, std::pair<ErrorReporter::ErrorType, std::string>>(position, std::pair<ErrorReporter::ErrorType, std::string>(type, error_msg)));
}

void ErrorReporter::recordError(ErrorReporter::ErrorType type, std::string const& error_msg)
{
	errors.insert(std::pair<std::pair<int, unsigned int>, std::pair<ErrorReporter::ErrorType, std::string>>(std::pair<int, unsigned int>(-1, 1), std::pair<ErrorReporter::ErrorType, std::string>(type, error_msg)));
}


void ErrorReporter::printErrors() const
{
	std::ifstream is(file_name);
	unsigned int lineNumber = 1;
	std::string lineOfCode;
	getline(is, lineOfCode);

	for (auto& error : errors)
	{
		switch (error.second.first)
		{
			case ErrorReporter::ErrorType::SEMANTIC:
				std::cerr << "\033[1;31mSemantic error\033[0m";
				break;

			case ErrorReporter::ErrorType::PARSER:
				std::cerr << "\033[1;31mParser error\033[0m";
				break;

			case ErrorReporter::ErrorType::LEXER:
				std::cerr << "\033[1;31mLexer error\033[0m";
				break;
		}

		if (error.first.first == -1)
			std::cerr << ": " << error.second.second << std::endl;
		else
		{
			//at this point: error.first.first >= 0
			unsigned int currentLine = (unsigned int) error.first.first;

			while (lineNumber != currentLine)
			{
				getline(is, lineOfCode);
				lineNumber++;
			}

			for (auto pos = lineOfCode.find('\t'); pos < lineOfCode.length(); pos = lineOfCode.find('\t', pos + 1))
				lineOfCode[pos] = ' ';

			std::string markerline(error.first.second - 1, ' ');
			markerline += '^';

			std::cerr << " at line " << error.first.first << ", column " << error.first.second << ": " << error.second.second << std::endl;
			// output input line where error occurred and markerline
			auto minPos = std::max(((int)error.first.second) - 50, 0);
			auto length = std::min(100, (int)lineOfCode.length());
			auto leading_dots = (minPos > 0) ? "... " : "";
			auto trailing_dots = (length == 100) ? " ..." : "";
			std::cerr << leading_dots << lineOfCode.substr(minPos, length) << trailing_dots << std::endl;
			std::cerr << leading_dots << markerline.substr(minPos, length) << trailing_dots << std::endl;
		}
	}

	if (errors.size() > 0)
		std::cerr << "Error during compilation" << std::endl;
}

std::unordered_map<std::string, std::string> ErrorReporter::escapeCodes =
{
	{"RED_BOLD", "\033[1;31m"},
	{"CLEAR_FORMAT", "\033[0m"},
	{"IDENT", "\033[1;34m"},
	{"TYPE", "\033[1;33m"}
};

std::string ErrorReporter::formatType(std::string const& typeName)
{
	return escapeCodes["TYPE"] + typeName + escapeCodes["CLEAR_FORMAT"];
}

std::string ErrorReporter::formatIdent(std::string const& identName)
{
	return escapeCodes["IDENT"] + identName + escapeCodes["CLEAR_FORMAT"];
}