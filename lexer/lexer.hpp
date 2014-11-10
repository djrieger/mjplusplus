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

		/*
		 * If nextCharacter is \n the line number in position will be incremented
		 * and column is reset to 1. Otherwise column will be incremented.
		 */
		void advancePosition(char nextCharacter);
		/*
		 * If nextCharacter is \n the line number in position will be decreased by one.
		 * Otherwise the column will be decreased by one.
		 */
		void rewindPosition(char nextCharacter);

	public:
		Lexer(std::istream& input, Stateomat const& stateomat, bool debug = false);
		Token get_next_token();
		void unget_token(Token t);
		bool good() const;
		std::istream& getInput();
		std::string describe(Token::Token_type t) const;
};

#endif
