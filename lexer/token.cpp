#include <iostream>

#include "token.hpp"

void Token::print() const
{
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
