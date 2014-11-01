#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>

struct Token {
	enum Type {
		TOKEN_EOF,
		TOKEN_INT_LIT,
		TOKEN_IDENT,
		TOKEN_OPERATOR,
		TOKEN_KEYWORD,
		TOKEN_ERROR,
	};

	Type type;
	std::string string_value;
	// line, column
	std::pair<unsigned int, unsigned int> position;

	void print() const;
};

#endif
