#ifndef LEXER_HPP
#define LEXER_HPP

#include <iostream>

#include "token.hpp"
#include "stateomat.hpp"

class Lexer
{
	protected:
		/** current lexer position in file (line, column) */
		std::pair<unsigned int, unsigned int> position;
		/** stateomat holds the transition informations */
		Stateomat stateomat;
		/**
		 *  saves tokens that are put back by unget
		 *  @see unget
		 */
		std::vector<Token> token_stack;
		/** currently read char */
		int c;
		//std::istream::pos_type line_start;

#define BUF_SIZE 1024
		//used for more efficient (blockwise) read operations
		int fd;
		/**
		 *  Reads the next character and returns it.
		 *  @return next character
		 */
		char getc();
		char buf[BUF_SIZE];
		size_t buf_off;
		size_t buf_len;

		off_t line_start;

		/**
		 * Advance the current position.
		 *
		 * If nextCharacter is \n the line number in position will be incremented
		 * and column is reset to 1. Otherwise column will be incremented.
		 * @param nextCharacter the next character
		 */
		void advancePosition(int nextCharacter);
		Token::Token_type lex_keyword_or_ident(const char* s);

	public:
		/**
		 *  Constructor.
		 *
		 *  Initializes the lexer so that llexing can be started.
		 *
		 *  @param file_name file that will be lexed
		 *  @param stateomat stateomat that helds the transitions
		 */
		Lexer(const char* file_name, Stateomat const& stateomat);
		/**
		 *  Lexes the next token and returns it.
		 *  @return the next token
		 */
		Token get_next_token();
		/**
		 *  Puts back a token that was inspected but not yet consumed.
		 *  This can happen because the grammar is not SLL(1).
		 *  If there were calls to unget, get_next_token will return them
		 *  first (LIFO order) before lexing the next token.
		 *
		 *  @param t token to be set back
		 */
		void unget_token(Token const& t);
		/**
		 *  Constant true.
		 *  @return true
		 */
		bool good() const;
		/**
		 * Returns the line the lexer is in
		 * @return current line
		 */
		std::string getLine();
		/**
		 *  Returns a description string for the given token
		 *  @param token to print the description of
		 *  @returns description for t
		 */
		std::string describe(Token::Token_type const& t) const;
};

#endif
