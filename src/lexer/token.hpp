#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>
#include <unordered_map>
#include "../globals.hpp"
#include "../util/symbol_table/Symbol.hpp"

namespace lexer
{
	class Token
	{
		private:
			/**
			 * string table mapping all strings (identifier, keywords, integers) seen by the lexer to symbols
			 */
			static std::unordered_map<std::string, shptr<Symbol>> stringTable;

		public:
			/** enum containing the different token types */
			enum Token_type
			{
				TOKEN_EOF,
				TOKEN_INT_LIT,
				TOKEN_IDENT,
				TOKEN_IDENT_OR_KEYWORD,
				TOKEN_OPERATOR,// only used before distinction which operator
				TOKEN_ERROR,
				/* Keywords */
				KEYWORD_ABSTRACT, KEYWORD_ASSERT,
				KEYWORD_BOOLEAN, KEYWORD_BREAK, KEYWORD_BYTE,
				KEYWORD_CASE, KEYWORD_CATCH, KEYWORD_CHAR, KEYWORD_CLASS, KEYWORD_CONST, KEYWORD_CONTINUE,
				KEYWORD_DEFAULT, KEYWORD_DOUBLE, KEYWORD_DO,
				KEYWORD_ELSE, KEYWORD_ENUM, KEYWORD_EXTENDS,
				KEYWORD_FALSE, KEYWORD_FINALLY, KEYWORD_FINAL, KEYWORD_FLOAT, KEYWORD_FOR,
				KEYWORD_GOTO,
				KEYWORD_IF, KEYWORD_IMPLEMENTS, KEYWORD_IMPORT, KEYWORD_INSTANCEOF, KEYWORD_INTERFACE, KEYWORD_INT,
				KEYWORD_LONG,
				KEYWORD_NATIVE, KEYWORD_NEW, KEYWORD_NULL,
				KEYWORD_PACKAGE, KEYWORD_PRIVATE, KEYWORD_PROTECTED, KEYWORD_PUBLIC,
				KEYWORD_RETURN,
				KEYWORD_SHORT, KEYWORD_STATIC, KEYWORD_STRICTFP, KEYWORD_SUPER, KEYWORD_SWITCH, KEYWORD_SYNCHRONIZED,
				KEYWORD_THIS, KEYWORD_THROWS, KEYWORD_THROW, KEYWORD_TRANSIENT, KEYWORD_TRUE, KEYWORD_TRY,
				KEYWORD_VOID, KEYWORD_VOLATILE,
				KEYWORD_WHILE,
				/* Operators */
				OPERATOR_NOTEQ, OPERATOR_NOT,
				OPERATOR_LPAREN, OPERATOR_RPAREN,
				OPERATOR_MULTEQ, OPERATOR_MULT,
				OPERATOR_PLUPLUS, OPERATOR_PLUSEQ, OPERATOR_PLUS,
				OPERATOR_COMMA,
				OPERATOR_MINUSEQ, OPERATOR_MINUSMINUS, OPERATOR_MINUS,
				OPERATOR_DOT,
				OPERATOR_SLASHEQ, OPERATOR_SLASH,
				OPERATOR_COLON, OPERATOR_SEMICOLON,
				OPERATOR_LTLTEQ, OPERATOR_LTLT, OPERATOR_LTEQ, OPERATOR_LT,
				OPERATOR_EQEQ, OPERATOR_EQ,
				OPERATOR_GTEQ, OPERATOR_GTGTEQ, OPERATOR_GTGTGTEQ, OPERATOR_GTGTGT, OPERATOR_GTGT, OPERATOR_GT,
				OPERATOR_QUESTION,
				OPERATOR_MODEQ, OPERATOR_MOD,
				OPERATOR_ANDEQ, OPERATOR_ANDAND, OPERATOR_AND,
				OPERATOR_LBRACKET, OPERATOR_RBRACKET,
				OPERATOR_XOREQ, OPERATOR_XOR,
				OPERATOR_LBRACE, OPERATOR_RBRACE,
				OPERATOR_NEG,
				OPERATOR_OREQ, OPERATOR_OROR, OPERATOR_OR
			};
			/** this token's type */
			Token_type token_type;
			/** this token's string value */
			std::string const* string_value;
			/** position of current token in source file (line, column) */
			source_position_t position;

			/** construct Token and insert string_value into stringTable */
			Token(Token_type const& token_type, std::string const& string_value, source_position_t const& position);

			/** constructor to skip lookup of string_value in stringTable. string_value MUST BE in stringTable already */
			Token(Token_type const& token_type, std::string const* string_value, source_position_t const& position);

			/** prints a string representation of this token */
			void print() const;

			static std::string const& getTableReference(std::string const& value);

			static shptr<Symbol>& getSymbol(std::string const& value);

			/** lookup table from token type to entry in stringtable
			 *  used to prepopulate stringtable with allowed keywords and operators
			 */
			static std::string const* type_to_ref[];
	};
}
#endif
