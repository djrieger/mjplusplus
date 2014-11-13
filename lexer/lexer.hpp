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
		Stateomat stateomat;
		std::vector<Token> token_stack;
		int c;
		//std::istream::pos_type line_start;

#define BUF_SIZE 1024

		int fd;
		char getc();
		char buf[BUF_SIZE];
		size_t buf_off;
		size_t buf_len;
		off_t line_start;

		/*
		 * If nextCharacter is \n the line number in position will be incremented
		 * and column is reset to 1. Otherwise column will be incremented.
		 */
		void advancePosition(int nextCharacter);
		Token::Token_type lex_keyword_or_ident(const char* s);

	public:
		Lexer(const char* file_name, Stateomat const& stateomat);
		Token get_next_token();
		void unget_token(Token const& t);
		bool good() const;
		std::string getLine();
		std::string describe(Token::Token_type const& t) const;
};

#endif
