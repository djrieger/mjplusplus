#ifndef PRECEDENCES_HPP
#define PRECEDENCES_HPP

#include <tuple>
#include <map>
#include "../lexer/token.hpp"

//tuple = (precedence, left_associative)
static std::map<Token::Token_type, std::tuple<int,bool> > operator_precs =
			{{Token::Token_type::OPERATOR_EQ, std::make_tuple(1,false)},
			 {Token::Token_type::OPERATOR_OROR, std::make_tuple(2,true)},
			 {Token::Token_type::OPERATOR_ANDAND, std::make_tuple(3,true)},
			 {Token::Token_type::OPERATOR_EQEQ, std::make_tuple(4,true)},
			 {Token::Token_type::OPERATOR_NOTEQ, std::make_tuple(4,true)},
			 {Token::Token_type::OPERATOR_LT, std::make_tuple(5,true)},
			 {Token::Token_type::OPERATOR_LTEQ, std::make_tuple(5,true)},
			 {Token::Token_type::OPERATOR_GT, std::make_tuple(5,true)},
			 {Token::Token_type::OPERATOR_GTEQ, std::make_tuple(5,true)},
			 {Token::Token_type::OPERATOR_PLUS, std::make_tuple(6,true)},
			 {Token::Token_type::OPERATOR_MINUS, std::make_tuple(6,true)},
			 {Token::Token_type::OPERATOR_MULT, std::make_tuple(7,true)},
			 {Token::Token_type::OPERATOR_SLASH, std::make_tuple(7,true)},
			 {Token::Token_type::OPERATOR_MOD, std::make_tuple(7,true)}};
#endif
