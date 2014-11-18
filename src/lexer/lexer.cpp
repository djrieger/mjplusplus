#define _XOPEN_SOURCE 700

#include "lexer.hpp"
#include "token.hpp"

#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>

/** keyword list to be used as lookup in keyword automaton.
 *  used for words that can only be recognised at the end of the string
 */
Token::Token_type Lexer::kw_array[] =
{
	Token::Token_type::KEYWORD_DO,
	Token::Token_type::KEYWORD_FINAL,
	Token::Token_type::KEYWORD_IF,
	Token::Token_type::KEYWORD_INT,
	Token::Token_type::KEYWORD_THROW,
	Token::Token_type::KEYWORD_THROWS,
};

/** lookup for keyword automaton check
 *  used when only one possible keyword remains in the middle of the string
 */
std::vector<std::pair<const char*, Token::Token_type>> Lexer::kw_vector =
{
	std::make_pair("abstract", Token::Token_type::KEYWORD_ABSTRACT),
	std::make_pair("assert", Token::Token_type::KEYWORD_ASSERT),
	std::make_pair("boolean", Token::Token_type::KEYWORD_BOOLEAN),
	std::make_pair("break", Token::Token_type::KEYWORD_BREAK),
	std::make_pair("byte", Token::Token_type::KEYWORD_BYTE),
	std::make_pair("case", Token::Token_type::KEYWORD_CASE),
	std::make_pair("catch", Token::Token_type::KEYWORD_CATCH),
	std::make_pair("char", Token::Token_type::KEYWORD_CHAR),
	std::make_pair("class", Token::Token_type::KEYWORD_CLASS),
	std::make_pair("const", Token::Token_type::KEYWORD_CONST),
	std::make_pair("continue", Token::Token_type::KEYWORD_CONTINUE),
	std::make_pair("default", Token::Token_type::KEYWORD_DEFAULT),
	std::make_pair("double", Token::Token_type::KEYWORD_DOUBLE),
	std::make_pair("else", Token::Token_type::KEYWORD_ELSE),
	std::make_pair("enum", Token::Token_type::KEYWORD_ENUM),
	std::make_pair("extends", Token::Token_type::KEYWORD_EXTENDS),
	std::make_pair("false", Token::Token_type::KEYWORD_FALSE),
	std::make_pair("finally", Token::Token_type::KEYWORD_FINALLY),
	std::make_pair("float", Token::Token_type::KEYWORD_FLOAT),
	std::make_pair("for", Token::Token_type::KEYWORD_FOR),
	std::make_pair("goto", Token::Token_type::KEYWORD_GOTO),
	std::make_pair("implements", Token::Token_type::KEYWORD_IMPLEMENTS),
	std::make_pair("import", Token::Token_type::KEYWORD_IMPORT),
	std::make_pair("instanceof", Token::Token_type::KEYWORD_INSTANCEOF),
	std::make_pair("interface", Token::Token_type::KEYWORD_INTERFACE),
	std::make_pair("long", Token::Token_type::KEYWORD_LONG),
	std::make_pair("native", Token::Token_type::KEYWORD_NATIVE),
	std::make_pair("new", Token::Token_type::KEYWORD_NEW),
	std::make_pair("null", Token::Token_type::KEYWORD_NULL),
	std::make_pair("package", Token::Token_type::KEYWORD_PACKAGE),
	std::make_pair("private", Token::Token_type::KEYWORD_PRIVATE),
	std::make_pair("protected", Token::Token_type::KEYWORD_PROTECTED),
	std::make_pair("public", Token::Token_type::KEYWORD_PUBLIC),
	std::make_pair("return", Token::Token_type::KEYWORD_RETURN),
	std::make_pair("short", Token::Token_type::KEYWORD_SHORT),
	std::make_pair("static", Token::Token_type::KEYWORD_STATIC),
	std::make_pair("strictfp", Token::Token_type::KEYWORD_STRICTFP),
	std::make_pair("super", Token::Token_type::KEYWORD_SUPER),
	std::make_pair("switch", Token::Token_type::KEYWORD_SWITCH),
	std::make_pair("synchronized", Token::Token_type::KEYWORD_SYNCHRONIZED),
	std::make_pair("this", Token::Token_type::KEYWORD_THIS),
	std::make_pair("transient", Token::Token_type::KEYWORD_TRANSIENT),
	std::make_pair("true", Token::Token_type::KEYWORD_TRUE),
	std::make_pair("try", Token::Token_type::KEYWORD_TRY),
	std::make_pair("void", Token::Token_type::KEYWORD_VOID),
	std::make_pair("volatile", Token::Token_type::KEYWORD_VOLATILE),
	std::make_pair("while", Token::Token_type::KEYWORD_WHILE),
};

/** transition table for keyword automaton */
const int Lexer::kw_lex_table[][26] = {{ 1,  2,  3,  4,  5,  6, 50, 83,  7, 83, 83, 55, 83,  8, 83,  9, 83, 63, 10, 11, 83, 12, 76, 83, 83, 83, },
	{83, 30, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 31, 83, 83, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 32, 83, 83, 33, 83, 83, 83, 83, 83, 83, 34, 83, },
	{13, 83, 83, 83, 83, 83, 83, 37, 83, 83, 83, 38, 83, 83, 14, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 41, 83, 83, 83, 83, 83, 83, 83, 83, 83, 77, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 43, 83, 44, 83, 83, 83, 83, 83, 83, 83, 83, 83, 45, 83, 83, },
	{46, 83, 83, 83, 83, 83, 83, 83, 16, 83, 83, 48, 83, 83, 49, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 83, 79, 83, 83, 83, 83, 83, 83, 17, 18, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, },
	{56, 83, 83, 83, 57, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 58, 83, 83, 83, 83, 83, },
	{59, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 19, 83, 83, 62, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 83, 83, 83, 64, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 20, 67, 83, 68, 83, 69, 83, },
	{83, 83, 83, 83, 83, 83, 83, 21, 83, 83, 83, 83, 83, 83, 83, 83, 83, 22, 83, 83, 83, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 23, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 35, 36, 83, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 24, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 25, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 27, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 53, 80, 83, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 83, 83, 83, 83, 60, 83, 83, 83, 83, 83, 61, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, },
	{65, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 66, 83, 83, 83, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 83, 83, 83, 83, 70, 83, 83, 83, 83, 83, 83, 83, 83, 28, 83, 83, 83, 83, 83, 83, 83, 83, },
	{71, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 72, 83, 83, 83, 73, 83, },
	{83, 83, 83, 83, 83, 83, 83, 83, 74, 83, 83, 75, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 39, 40, 83, 83, 83, 83, 83, 83, },
	{26, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 78, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 51, 83, 83, 52, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 29, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 81, 83, 83, 83, },
	{83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 42, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 47, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 54, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 82, 83, 83, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, },
	{83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, },
};

Lexer::Lexer(const char* file_name, Stateomat const& stateomat)
	: position {1, 1}, stateomat(stateomat), line_start(0)
{

	fd = open(file_name, O_RDONLY);

	if (fd == -1)
		throw "Error opening file \"" + std::string(file_name) + "\" (" + std::string(strerror(errno)) + ")";

#ifndef __APPLE__

	int fadvise_result = posix_fadvise(fd, 0, 0, POSIX_FADV_SEQUENTIAL);

	if (fadvise_result != 0)
		throw "Error advising kernel about file access on \"" + std::string(file_name) + "\" (" + std::string(strerror(fadvise_result)) + ")";

#endif

	buf_off = 0;
	buf_len = 0;

	//get first character so that get_next_token starts on first char of token
	c = getc();
}

void Lexer::advancePosition(int nextCharacter)
{
	if (nextCharacter == '\n')
	{
		line_start += position.second;
		position.first++;
		position.second = 1;
	}
	else
		position.second++;
}

char Lexer::getc()
{

	char foo;

	if (buf_off < buf_len)
	{
		foo = buf[buf_off];
		buf_off++;
	}
	else
	{
		ssize_t bytes_read = read(fd, buf, BUF_SIZE);

		if (bytes_read == -1)
			throw "Error reading file (" + std::string(strerror(errno)) + ").";
		else if (bytes_read == 0)
			foo = EOF;
		else
		{
			buf_off = 0;
			buf_len = bytes_read;
			return getc();
		}
	}

	return foo;
}

std::string Lexer::getLine()
{

	off_t initial_offset = lseek(fd, 0, SEEK_CUR);

	lseek(fd, line_start, SEEK_SET);

	FILE* f = fdopen(fd, "r");

	char* line = NULL;
	size_t alloc = 0;
	ssize_t len = getline(&line, &alloc, f);
	line[len - 1] = '\0';

	std::string s(line);

	free(line);

	lseek(fd, initial_offset, SEEK_SET);

	return s;
}

Token::Token_type Lexer::lex_keyword_or_ident(const char* s)
{

	int state = 0;

	for (int i = 0; s[i] != '\0'; i++)
	{
		//we know that s consists only of lowercase letters,
		//so we project them to [0..25] in the transition table
		state = kw_lex_table[state][s[i] - 'a'];

		if (state == IDENT)
			return Token::Token_type::TOKEN_IDENT;//we don't have a keyword => we have an ident

		else if (CHECK_ABSTRACT <= state && state <= CHECK_WHILE)
		{
			/* now it can only be one keyword, or an ident.
			   so we need to check whether s is indeed that keyword */

			//fetch that keyword (string and token type)
			auto kw_pair = kw_vector[state - CHECK_ABSTRACT];

			//compare s to keyword
			if (strcmp(kw_pair.first, s) == 0)
				return kw_pair.second;//we have found our keyword
			else
				return Token::Token_type::TOKEN_IDENT;//we have an ident
		}
	}

	//if we are at the end of the string, we check whether we have a keyword
	//that is only recognised at the end (e.g. "do" cannot be recognised earlier
	//because of "double")
	if (KEYWORD_DO <= state && state <= KEYWORD_THROWS)
		return kw_array[state - KEYWORD_DO];
	else
		return Token::Token_type::TOKEN_IDENT;
}

Token Lexer::get_next_token()
{
	//if we have token that were ungetted, use them first
	if (!token_stack.empty())
	{
		Token t(token_stack.back());
		token_stack.pop_back();
		return t;
	}

	//initialize automaton
	Token::Token_type type = Token::Token_type::TOKEN_ERROR;
	auto token_position(position);
	std::string value;
	int state = STATE_START;

	//run the automaton
	while (1)
	{
		int new_state = stateomat.transitions[state][c == EOF ? 128 : c];

		if (new_state == STATE_STOP)
		{
			//here we know our token is complete

			//check whether token is valid
			if (stateomat.state_is_accepting(state))
			{

				if (type == Token::Token_type::TOKEN_OPERATOR)
					//if we have an operator, we look up which one we have
					type = stateomat.operators[state][value];
				else if (type == Token::Token_type::TOKEN_IDENT_OR_KEYWORD)
					//if we have a (lowercase) string, it can be a keyword or an ident
					//so we run a check with a special keyword automaton that returns
					//the appropriate keyword token or ident
					type = lex_keyword_or_ident(value.c_str());

				std::string const* inTable = Token::type_to_ref[type];

				if (inTable)
					return Token(type, inTable, token_position);
				else
					return Token(type, value, token_position);
			}
			else
			{
				//token is invalid
				return Token(Token::Token_type::TOKEN_ERROR, value, position);
			}
		}
		else if (!stateomat.state_is_accepting(new_state))
			//we had a whitespace or are within a comment
			value.clear();
		else
		{
			if (state == STATE_START)
				token_position = position;//new position after whitespace/comment

			value.push_back(c);//append character to lexed string
			type = stateomat.state_type[new_state];
		}

		//advance token
		advancePosition(c);
		c = getc();

		state = new_state;
	}
}

std::string Lexer::describe(Token::Token_type const& t) const
{
	auto it = stateomat.reverse.find(t);

	if (it != stateomat.reverse.end())
		return it->second;
	else
		return "";
}

bool Lexer::good() const
{
	return true;
}

void Lexer::unget_token(Token const& t)
{
	token_stack.push_back(t);
}
