#ifndef PRECEDENCES_HPP
#define PRECEDENCES_HPP

#include <tuple>
#include <map>
#include "../lexer/token.hpp"

//pair = (precedence, left_associative)
std::map<Token::Token_type, std::tuple<int,bool> > operator_precs =
	{{Token::Token_type::OPERATOR_EQ, std::make_pair(1,false)},
	 {Token::Token_type::OPERATOR_OROR, std::make_pair(2,true)},
	 {Token::Token_type::OPERATOR_ANDAND, std::make_pair(3,true)},
	 {Token::Token_type::OPERATOR_EQEQ, std::make_pair(4,true)},
	 {Token::Token_type::OPERATOR_NOTEQ, std::make_pair(4,true)},
	 {Token::Token_type::OPERATOR_LT, std::make_pair(5,true)},
	 {Token::Token_type::OPERATOR_LTEQ, std::make_pair(5,true)},
	 {Token::Token_type::OPERATOR_GT, std::make_pair(5,true)},
	 {Token::Token_type::OPERATOR_GTEQ, std::make_pair(5,true)},
	 {Token::Token_type::OPERATOR_PLUS, std::make_pair(6,true)},
	 {Token::Token_type::OPERATOR_MINUS, std::make_pair(6,true)},
	 {Token::Token_type::OPERATOR_MULT, std::make_pair(7,true)},
	 {Token::Token_type::OPERATOR_SLASH, std::make_pair(7,true)},
	 {Token::Token_type::OPERATOR_MOD, std::make_pair(7,true)}};

#endif
