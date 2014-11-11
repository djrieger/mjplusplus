#ifndef LEXER_HPP
#define LEXER_HPP

#include <iostream>

#include "token.hpp"
#include "stateomat.hpp"

class Lexer
{
	protected:
		// line, column
		std::pair<unsigned int, unsigned int> position;
		std::istream& input;
		Stateomat stateomat;
		bool debug;
		std::vector<Token> token_stack;
		int c;
		std::istream::pos_type line_start;

		/*
		 * If nextCharacter is \n the line number in position will be incremented
		 * and column is reset to 1. Otherwise column will be incremented.
		 */
		void advancePosition(int nextCharacter);

	public:
		Lexer(std::istream& input, Stateomat const& stateomat, bool debug = false);
		Token get_next_token();
		void unget_token(Token const& t);
		bool good() const;
		std::string getLine();
		std::string describe(Token::Token_type const& t) const;
};

#endif
