#include <fstream>
#include <algorithm>
#include "ErrorReporter.hpp"

ErrorReporter::ErrorReporter(std::string const& file_name, bool recordErrors)
	: typeRegex("\\$type\\{(.*?)\\}"), identRegex("\\$ident\\{(.*?)\\}"), file_name(file_name), recordErrors(recordErrors)
{

}

std::unordered_map<std::string, std::string> ErrorReporter::escapeCodes =
{
	{"RED_BOLD", "\033[1;31m"},
	{"BOLD", "\033[1m"},
	{"CLEAR_FORMAT", "\033[0m"}
};

void ErrorReporter::recordError(ErrorReporter::ErrorType type, std::string const& error_msg, source_position_t position)
{
	formatAndRecordError(type, error_msg, position);
}

void ErrorReporter::recordError(ErrorReporter::ErrorType type, std::string const& error_msg)
{
	formatAndRecordError(type, error_msg, std::pair<int, unsigned int>(-1, 1));
}

void ErrorReporter::formatAndRecordError(ErrorReporter::ErrorType type, std::string const& error_msg, std::pair<int, unsigned int> position)
{
	if (position.first == 0)
		throw "Invalid line number 0 for error " + error_msg;

	if (recordErrors)
	{
		std::string formattedErrorMsg = std::regex_replace(error_msg, typeRegex, escapeCodes["BOLD"] + "$1" + escapeCodes["CLEAR_FORMAT"]);
		formattedErrorMsg = std::regex_replace(formattedErrorMsg, identRegex, "'$1'");
		errors.insert(std::pair<std::pair<int, unsigned int>, std::pair<ErrorReporter::ErrorType, std::string>>(position, std::pair<ErrorReporter::ErrorType, std::string>(type, formattedErrorMsg)));
	}
}

void ErrorReporter::printErrors() const
{
	if (!recordErrors)
		return;

	std::ifstream is(file_name);
	unsigned int lineNumber = 1;
	std::string lineOfCode;
	getline(is, lineOfCode);

	for (auto& error : errors)
	{
		std::cerr << escapeCodes["RED_BOLD"];

		switch (error.second.first)
		{
			case ErrorReporter::ErrorType::SEMANTIC:
				std::cerr << "Semantic error";
				break;

			case ErrorReporter::ErrorType::PARSER:
				std::cerr << "Parser error";
				break;

			case ErrorReporter::ErrorType::LEXER:
				std::cerr << "Lexer error";
				break;
		}

		std::cerr << escapeCodes["CLEAR_FORMAT"];

		if (error.first.first == -1)
			std::cerr << ": " << error.second.second << std::endl;
		else
		{
			//at this point: error.first.first >= 0
			unsigned int currentLine = (unsigned int) error.first.first;

			while (lineNumber < currentLine)
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
			if (!lineOfCode.empty())
			{
				auto minPos = lineOfCode.length() >= 100U && error.first.second > 50U ? error.first.second - 50U : 0U;
				auto length = std::min<size_t>(100U, lineOfCode.length());
				std::cerr << (minPos ? "... " : "") << lineOfCode.substr(minPos, length) << (length == 100U ? " ..." : "") << std::endl;
				std::cerr << (minPos ? "    " : "") << markerline.substr(minPos, length) << std::endl;
			}
		}
	}

	if (errors.size() > 0)
		std::cerr << "Error during compilation" << std::endl;
}
