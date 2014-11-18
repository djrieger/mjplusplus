#include <iostream>

#include "token.hpp"

std::unordered_set<std::string> Token::stringTable;

Token::Token(Token_type const& token_type, std::string const& string_value, std::pair<unsigned int, unsigned int> const& position)
	: token_type(token_type), string_value(&*stringTable.insert(string_value).first), position(position)
{
	;
}

void Token::print() const
{
	// uncommenting the following line is for debug purposes during lextest
	//std::cout << position.first << "," << position.second << ":\t";
	switch (token_type)
	{
		case TOKEN_EOF:
			std::cout << "EOF" << std::endl;
			break;

		case TOKEN_INT_LIT:
			std::cout << "integer literal " << string_value << std::endl;
			break;

		case TOKEN_IDENT:
			std::cout << "identifier " << string_value << std::endl;
			break;

		case TOKEN_ERROR:
			break;

		default:
			std::cout << string_value << std::endl;
			break;
	}
}
