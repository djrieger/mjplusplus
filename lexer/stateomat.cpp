#include <algorithm>
#include <fstream>
#include <iostream>

#include "stateomat.hpp"

Stateomat::Stateomat()
{
	transitions = {{19, 19, 19, 19, 19, 19, 19, 19, 19, 0, 0, 19, 19, 0, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 0, 6, 19, 19, 19, 6, 7, 19, 5, 5, 6, 9, 5, 10, 5, 4, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 5, 5, 11, 6, 13, 5, 19, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 5, 19, 5, 6, 3, 19, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 5, 8, 5, 5, 19, 18,},
		{19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,},
		{19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,},
		{19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 19, 19, 19, 19, 19, 19, 19, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 19, 19, 19, 19, 3, 19, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 19, 19, 19, 19, 19, 19,},
		{19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 16, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 5, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,},
		{19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,},
		{19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 5, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,},
		{19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 5, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 5, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,},
		{19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 5, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 5, 19, 19, 19, 19,},
		{19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 5, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 5, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,},
		{19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 5, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 5, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,},
		{19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 12, 5, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,},
		{19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 5, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,},
		{19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 5, 14, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,},
		{19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 5, 15, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,},
		{19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 5, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,},
		{16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 17, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 19,},
		{16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 17, 16, 16, 16, 16, 0, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 19,},
		{19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,},
		{19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19, 19,}
	};
	state_type = {{ 1, Token::Token_type::TOKEN_INT_LIT},  { 2, Token::Token_type::TOKEN_INT_LIT},
		{ 3, Token::Token_type::TOKEN_IDENT},    { 4, Token::Token_type::TOKEN_OPERATOR},
		{ 5, Token::Token_type::TOKEN_OPERATOR}, { 6, Token::Token_type::TOKEN_OPERATOR},
		{ 7, Token::Token_type::TOKEN_OPERATOR}, { 8, Token::Token_type::TOKEN_OPERATOR},
		{ 9, Token::Token_type::TOKEN_OPERATOR}, {10, Token::Token_type::TOKEN_OPERATOR},
		{11, Token::Token_type::TOKEN_OPERATOR}, {12, Token::Token_type::TOKEN_OPERATOR},
		{13, Token::Token_type::TOKEN_OPERATOR}, {14, Token::Token_type::TOKEN_OPERATOR},
		{15, Token::Token_type::TOKEN_OPERATOR}, {18, Token::Token_type::TOKEN_EOF}
	};
	keywords = {"abstract", "assert", "boolean", "break", "byte", "case",
	            "catch", "char", "class", "const", "continue", "default",
	            "double", "do", "else", "enum", "extends", "false", "finally",
	            "final", "float", "for", "goto", "if", "implements", "import",
	            "instanceof", "interface", "int", "long", "native", "new",
	            "null", "package", "private", "protected", "public", "return",
	            "short", "static", "strictfp", "super", "switch",
	            "synchronized", "this", "throws", "throw", "transient", "true",
	            "try", "void", "volatile", "while"
	           };
	non_accepting_states = {0, 16, 17};
	keyword_map = {{"abstract", Token::Keyword_type::KEYWORD_ABSTRACT}, {"assert", Token::Keyword_type::KEYWORD_ASSERT},
		{"boolean", Token::Keyword_type::KEYWORD_BOOLEAN}, {"break", Token::Keyword_type::KEYWORD_BREAK},
		{"byte", Token::Keyword_type::KEYWORD_BYTE}, {"case", Token::Keyword_type::KEYWORD_CASE},
		{"catch", Token::Keyword_type::KEYWORD_CATCH}, {"char", Token::Keyword_type::KEYWORD_CHAR},
		{"class", Token::Keyword_type::KEYWORD_CLASS}, {"const", Token::Keyword_type::KEYWORD_CONST},
		{"continue", Token::Keyword_type::KEYWORD_CONTINUE}, {"default", Token::Keyword_type::KEYWORD_DEFAULT},
		{"double", Token::Keyword_type::KEYWORD_DOUBLE}, {"do", Token::Keyword_type::KEYWORD_DO},
		{"else", Token::Keyword_type::KEYWORD_ELSE}, {"enum", Token::Keyword_type::KEYWORD_ENUM},
		{"extends", Token::Keyword_type::KEYWORD_EXTENDS}, {"false", Token::Keyword_type::KEYWORD_FALSE},
		{"finally", Token::Keyword_type::KEYWORD_FINALLY}, {"final", Token::Keyword_type::KEYWORD_FINAL},
		{"float", Token::Keyword_type::KEYWORD_FLOAT}, {"for", Token::Keyword_type::KEYWORD_FOR},
		{"goto", Token::Keyword_type::KEYWORD_GOTO}, {"if", Token::Keyword_type::KEYWORD_IF},
		{"implements", Token::Keyword_type::KEYWORD_IMPLEMENTS}, {"import", Token::Keyword_type::KEYWORD_IMPORT},
		{"instanceof", Token::Keyword_type::KEYWORD_INSTANCEOF}, {"interface", Token::Keyword_type::KEYWORD_INTERFACE},
		{"int", Token::Keyword_type::KEYWORD_INT}, {"long", Token::Keyword_type::KEYWORD_LONG},
		{"native", Token::Keyword_type::KEYWORD_NATIVE}, {"new", Token::Keyword_type::KEYWORD_NEW},
		{"null", Token::Keyword_type::KEYWORD_NULL}, {"package", Token::Keyword_type::KEYWORD_PACKAGE},
		{"private", Token::Keyword_type::KEYWORD_PRIVATE}, {"protected", Token::Keyword_type::KEYWORD_PROTECTED},
		{"public", Token::Keyword_type::KEYWORD_PUBLIC}, {"return", Token::Keyword_type::KEYWORD_RETURN},
		{"short", Token::Keyword_type::KEYWORD_SHORT}, {"static", Token::Keyword_type::KEYWORD_STATIC},
		{"strictfp", Token::Keyword_type::KEYWORD_STRICTFP}, {"super", Token::Keyword_type::KEYWORD_SUPER},
		{"switch", Token::Keyword_type::KEYWORD_SWITCH}, {"synchronized", Token::Keyword_type::KEYWORD_SYNCHRONIZED},
		{"this", Token::Keyword_type::KEYWORD_THIS}, {"throws", Token::Keyword_type::KEYWORD_THROWS},
		{"throw", Token::Keyword_type::KEYWORD_THROW}, {"transient", Token::Keyword_type::KEYWORD_TRANSIENT},
		{"true", Token::Keyword_type::KEYWORD_TRUE}, {"try", Token::Keyword_type::KEYWORD_TRY},
		{"void", Token::Keyword_type::KEYWORD_VOID}, {"volatile", Token::Keyword_type::KEYWORD_VOLATILE},
		{"while", Token::Keyword_type::KEYWORD_WHILE}
	};
	operator_map = {{"!=", Token::Operator_type::OPERATOR_NOTEQ}, {"!", Token::Operator_type::OPERATOR_NOT},
		{"(", Token::Operator_type::OPERATOR_LPARA}, {")", Token::Operator_type::OPERATOR_RPARA},
		{"*=", Token::Operator_type::OPERATOR_MULTEQ}, {"*", Token::Operator_type::OPERATOR_MULT},
		{"++", Token::Operator_type::OPERATOR_PLUPLUS}, {"+=", Token::Operator_type::OPERATOR_PLUSEQ},
		{"+", Token::Operator_type::OPERATOR_PLUS}, {",", Token::Operator_type::OPERATOR_COMMA},
		{"-=", Token::Operator_type::OPERATOR_MINUSEQ}, {"--", Token::Operator_type::OPERATOR_MINUSMINUS},
		{"-", Token::Operator_type::OPERATOR_MINUS}, {".", Token::Operator_type::OPERATOR_DOT},
		{"/=", Token::Operator_type::OPERATOR_SLASHEQ}, {"/", Token::Operator_type::OPERATOR_SLASH},
		{":", Token::Operator_type::OPERATOR_COLON}, {";", Token::Operator_type::OPERATOR_SEMICOLON},
		{"<<=", Token::Operator_type::OPERATOR_LTLTEQ}, {"<<", Token::Operator_type::OPERATOR_LTLT},
		{"<=", Token::Operator_type::OPERATOR_LTEQ}, {"<", Token::Operator_type::OPERATOR_LT},
		{"==", Token::Operator_type::OPERATOR_EQEQ}, {"=", Token::Operator_type::OPERATOR_EQ},
		{">=", Token::Operator_type::OPERATOR_GTEQ}, {">>=", Token::Operator_type::OPERATOR_GTGTEQ},
		{">>>=", Token::Operator_type::OPERATOR_GTGTGTEQ}, {">>>", Token::Operator_type::OPERATOR_GTGTGT},
		{">>", Token::Operator_type::OPERATOR_GTGT}, {">", Token::Operator_type::OPERATOR_GT},
		{"?", Token::Operator_type::OPERATOR_QUESTION}, {"%=", Token::Operator_type::OPERATOR_MODEQ},
		{"%", Token::Operator_type::OPERATOR_MOD}, {"&=", Token::Operator_type::OPERATOR_ANDEQ},
		{"&&", Token::Operator_type::OPERATOR_ANDAND}, {"&", Token::Operator_type::OPERATOR_AND},
		{"[", Token::Operator_type::OPERATOR_LBRACKET}, {"]", Token::Operator_type::OPERATOR_RBRACKET},
		{"^=", Token::Operator_type::OPERATOR_XOREQ}, {"^", Token::Operator_type::OPERATOR_XOR},
		{"{", Token::Operator_type::OPERATOR_LBRACE}, {"}", Token::Operator_type::OPERATOR_RBRACE},
		{"~", Token::Operator_type::OPERATOR_NEG}, {"|=", Token::Operator_type::OPERATOR_OREQ},
		{"||", Token::Operator_type::OPERATOR_OROR}, {"|", Token::Operator_type::OPERATOR_OR}
	};
}

bool Stateomat::state_is_accepting(unsigned int state)
{
	return non_accepting_states.find(state) == non_accepting_states.end();
}

void Stateomat::dump_graph(std::string const& file_name)
{
	std::ofstream f(file_name, std::ios::out | std::ios::binary | std::ios::trunc);

	if (!f.is_open())
		throw 42;

	auto write_line = [&f](int indent_level, std::string const & s)
	{
		for (int i = 0; i < indent_level; i++)
			f << "    ";

		f << s << std::endl;
	};
	write_line(0, "graph [");

	for (unsigned state = 0; state < transitions.size(); state++)
	{
		auto label = [&state]()
		{
			return state == STATE_STOP ? "STOP" : std::to_string(state);
		};
		auto color = [this, &state]()
		{
			if (state == STATE_STOP)
				return "#6666FF";
			else if (state_is_accepting(state))
				return "#66FF66";
			else
				return "#FF6666";
		};
		write_line(1, "node [");
		write_line(2, "id " + std::to_string(state));
		write_line(2, "label \"" + label() + "\"");
		write_line(2, "graphics [");
		write_line(3, std::string("fill \"") + color() + std::string("\""));
		write_line(2, "]");
		write_line(1, "]");
	}

	for (unsigned state = 0; state < transitions.size(); state++)
	{
		std::set<unsigned> reached_states;

		for (auto reached_state : transitions[state])
			reached_states.insert(reached_state);

		reached_states.erase(STATE_STOP);

		for (auto reached_state : reached_states)
		{
			std::vector<unsigned> label_chars;

			for (unsigned i = 0; i < transitions[state].size(); i++)
			{
				if (transitions[state][i] == reached_state)
					label_chars.push_back(i);
			}

			std::map<unsigned, std::string> foo;

			for (unsigned i = 0; i <= 128; i++)
			{
				foo.insert({i, "0d" + std::to_string(i)});
			}

			for (unsigned i = '!'; i <= '~'; i++)
				foo[i] = std::string(1, i);

			foo['&']  = "&amp;";
			foo['"']  = "&quot;";
			foo[128]  = "EOF";
			foo['\r'] = "&#92;r";
			foo['\n'] = "&#92;n";
			foo['\t'] = "&#92;t";
			std::string label;

			for (auto label_char : label_chars)
				label += foo[label_char];

			auto replace = [&label](std::string const & from, std::string const & to)
			{
				std::size_t i = label.find(from);

				if (i != std::string::npos)
					label = label.replace(i, from.length(), to);
			};
			replace("ABCDEFGHIJKLMNOPQRSTUVWXYZ", "[A-Z]");
			replace("abcdefghijklmnopqrstuvwxyz", "[a-z]");
			replace("0123456789", "[0-9]");
			replace("123456789", "[1-9]");

			if (label.length() > 20)
				label = "...";

			write_line(1, "edge [");
			write_line(2, "source " + std::to_string(state));
			write_line(2, "target " + std::to_string(reached_state));
			write_line(2, "label \"" + label + "\"");
			write_line(1, "]");
		}
	}

	write_line(0, "]");
	f.close();
}
