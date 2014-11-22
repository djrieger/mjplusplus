#ifndef LEXER_HPP
#define LEXER_HPP

#include <iostream>

#include "token.hpp"
#include "stateomat.hpp"

namespace lexer
{
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

			/** (partial) state list for keyword checking automaton */
			enum kw_states
			{
				CHECK_ABSTRACT = 30,
				CHECK_ASSERT,
				CHECK_BOOLEAN,
				CHECK_BREAK,
				CHECK_BYTE,
				CHECK_CASE,
				CHECK_CATCH,
				CHECK_CHAR,
				CHECK_CLASS,
				CHECK_CONST,
				CHECK_CONTINUE,
				CHECK_DEFAULT,
				CHECK_DOUBLE,
				CHECK_ELSE,
				CHECK_ENUM,
				CHECK_EXTENDS,
				CHECK_FALSE,
				CHECK_FINALLY,
				CHECK_FLOAT,
				CHECK_FOR,
				CHECK_GOTO,
				CHECK_IMPLEMENTS,
				CHECK_IMPORT,
				CHECK_INSTANCEOF,
				CHECK_INTERFACE,
				CHECK_LONG,
				CHECK_NATIVE,
				CHECK_NEW,
				CHECK_NULL,
				CHECK_PACKAGE,
				CHECK_PRIVATE,
				CHECK_PROTECTED,
				CHECK_PUBLIC,
				CHECK_RETURN,
				CHECK_SHORT,
				CHECK_STATIC,
				CHECK_STRICTFP,
				CHECK_SUPER,
				CHECK_SWITCH,
				CHECK_SYNCHRONIZED,
				CHECK_THIS,
				CHECK_TRANSIENT,
				CHECK_TRUE,
				CHECK_TRY,
				CHECK_VOID,
				CHECK_VOLATILE,
				CHECK_WHILE,
				KEYWORD_DO,
				KEYWORD_FINAL,
				KEYWORD_IF,
				KEYWORD_INT,
				KEYWORD_THROW,
				KEYWORD_THROWS,
				IDENT,
			};

			/** keyword list to be used as lookup in keyword automaton.
			 *  used for words that can only be recognised at the end of the string
			 */
			static Token::Token_type kw_array[];

			/** lookup for keyword automaton check
			 *  used when only one possible keyword remains in the middle of the string
			 */
			static std::vector<std::pair<const char*, Token::Token_type>> kw_vector;

			/** transition table for keyword automaton */
			static const int kw_lex_table[][26];

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
			 *  Returns a description string for the given token
			 *  @param token to print the description of
			 *  @returns description for t
			 */
			std::string describe(Token::Token_type const& t) const;
	};
}
#endif
