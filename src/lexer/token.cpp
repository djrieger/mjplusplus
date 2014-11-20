#include <iostream>

#include "token.hpp"

namespace lexer
{
	std::unordered_set<std::string> Token::stringTable;

	Token::Token(Token_type const& token_type, std::string const& string_value, std::pair<unsigned int, unsigned int> const& position)
		: token_type(token_type), string_value(&getTableReference(string_value)), position(position)
	{
		;
	}

	Token::Token(Token_type const& token_type, std::string const* string_value, std::pair<unsigned int, unsigned int> const& position)
		: token_type(token_type), string_value(string_value), position(position)
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
				std::cout << "integer literal " << *string_value << std::endl;
				break;

			case TOKEN_IDENT:
				std::cout << "identifier " << *string_value << std::endl;
				break;

			case TOKEN_ERROR:
				break;

			default:
				std::cout << *string_value << std::endl;
				break;
		}
	}

	std::string const& Token::getTableReference(std::string const& value)
	{
		return *stringTable.insert(value).first;
	}

	std::string const* Token::type_to_ref[] =
	{
		//TOKEN_
		nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
		//KEYWORD_
		nullptr, nullptr,
		&Token::getTableReference("boolean"),
		nullptr, nullptr, nullptr, nullptr, nullptr,
		&Token::getTableReference("class"),
		nullptr, nullptr, nullptr, nullptr, nullptr,
		&Token::getTableReference("else"),
		nullptr, nullptr,
		&Token::getTableReference("false"),
		nullptr, nullptr, nullptr, nullptr, nullptr,
		&Token::getTableReference("if"),
		nullptr, nullptr, nullptr,
		&Token::getTableReference("int"),
		nullptr, nullptr, nullptr,
		&Token::getTableReference("new"),
		&Token::getTableReference("null"),
		nullptr, nullptr, nullptr,
		&Token::getTableReference("public"),
		&Token::getTableReference("return"),
		nullptr,
		&Token::getTableReference("static"),
		nullptr, nullptr, nullptr, nullptr,
		&Token::getTableReference("this"),
		nullptr, nullptr, nullptr,
		&Token::getTableReference("true"),
		nullptr,
		&Token::getTableReference("void"),
		nullptr,
		&Token::getTableReference("while"),
		//OPERATOR_
		&Token::getTableReference("!="), &Token::getTableReference("!"),
		&Token::getTableReference("("), &Token::getTableReference(")"),
		nullptr, &Token::getTableReference("*"),
		nullptr, nullptr, &Token::getTableReference("+"),
		&Token::getTableReference(","),
		nullptr, nullptr, &Token::getTableReference("-"),
		&Token::getTableReference("."),
		nullptr, &Token::getTableReference("/"),
		nullptr, &Token::getTableReference(";"),
		nullptr, nullptr, &Token::getTableReference("<="), &Token::getTableReference("<"),
		&Token::getTableReference("=="), &Token::getTableReference("="),
		&Token::getTableReference(">="), nullptr, nullptr, nullptr, nullptr, &Token::getTableReference(">"),
		nullptr,
		nullptr, &Token::getTableReference("%"),
		nullptr, &Token::getTableReference("&&"), nullptr,
		&Token::getTableReference("["), &Token::getTableReference("]"),
		nullptr, nullptr,
		&Token::getTableReference("{"), &Token::getTableReference("}"),
		nullptr,
		nullptr, &Token::getTableReference("||"), nullptr
	};
}
