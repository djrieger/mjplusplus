#ifndef STATEOMAT_HPP
#define STATEOMAT_HPP

#include <unordered_map>
#include <map>
#include <set>
#include <string>
#include <vector>

#include "token.hpp"

#define STATE_START 0
#define STATE_STOP 19

class Stateomat
{
	public:
		Stateomat();
		bool state_is_accepting(unsigned int state);
		void dump_graph(std::string const& file_name);

		//protected:
		std::vector<std::vector<unsigned int>> transitions;
		std::vector<Token::Token_type> state_type;

		std::unordered_map<std::string, Token::Token_type> keywords;
		std::unordered_map<std::string, Token::Token_type> operators[24] =
		{
			{},
			{},
			{},
			{},
			{{"/", Token::Token_type::OPERATOR_SLASH}},
			{
				{"++", Token::Token_type::OPERATOR_PLUPLUS},
				{"+=", Token::Token_type::OPERATOR_PLUSEQ},
				{"-=", Token::Token_type::OPERATOR_MINUSEQ},
				{"--", Token::Token_type::OPERATOR_MINUSMINUS},
				{"/=", Token::Token_type::OPERATOR_SLASHEQ},
				{":", Token::Token_type::OPERATOR_COLON},
				{"<<=", Token::Token_type::OPERATOR_LTLTEQ},
				{">>=", Token::Token_type::OPERATOR_GTGTEQ},
				{">>>=", Token::Token_type::OPERATOR_GTGTGTEQ},
				{"?", Token::Token_type::OPERATOR_QUESTION},
				{"&=", Token::Token_type::OPERATOR_ANDEQ},
				{"~", Token::Token_type::OPERATOR_NEG},
				{"|=", Token::Token_type::OPERATOR_OREQ},
			},
			{	{"!", Token::Token_type::OPERATOR_NOT},
				{"%", Token::Token_type::OPERATOR_MOD},
				{"*", Token::Token_type::OPERATOR_MULT},
				{"=", Token::Token_type::OPERATOR_EQ},
				{"^", Token::Token_type::OPERATOR_XOR},
			},
			{{"&", Token::Token_type::OPERATOR_AND}},
			{{"|", Token::Token_type::OPERATOR_OR}},
			{{"+", Token::Token_type::OPERATOR_PLUS}},
			{{"-", Token::Token_type::OPERATOR_MINUS}},
			{{"<", Token::Token_type::OPERATOR_LT}},
			{{"<<", Token::Token_type::OPERATOR_LTLT}},
			{{">", Token::Token_type::OPERATOR_GT}},
			{{">>", Token::Token_type::OPERATOR_GTGT}},
			{{">>>", Token::Token_type::OPERATOR_GTGTGT}},
			{},
			{},
			{},
			{},
			{},
			{
				{"{", Token::Token_type::OPERATOR_LBRACE},
				{"}", Token::Token_type::OPERATOR_RBRACE},
				{"(", Token::Token_type::OPERATOR_LPAREN},
				{")", Token::Token_type::OPERATOR_RPAREN},
				{"[", Token::Token_type::OPERATOR_LBRACKET},
				{"]", Token::Token_type::OPERATOR_RBRACKET},
				{",", Token::Token_type::OPERATOR_COMMA},
				{";", Token::Token_type::OPERATOR_SEMICOLON},
				{".", Token::Token_type::OPERATOR_DOT},
			},
			{
				{"!=", Token::Token_type::OPERATOR_NOTEQ},
				{"*=", Token::Token_type::OPERATOR_MULTEQ},
				{"%=", Token::Token_type::OPERATOR_MODEQ},
				{"==", Token::Token_type::OPERATOR_EQEQ},
				{"^=", Token::Token_type::OPERATOR_XOREQ},
			},
			{
				{"||", Token::Token_type::OPERATOR_OROR},
				{"&&", Token::Token_type::OPERATOR_ANDAND},
				{"<=", Token::Token_type::OPERATOR_LTEQ},
				{">=", Token::Token_type::OPERATOR_GTEQ},
			},
		};

		std::map<Token::Token_type, std::string> reverse;
};

#endif
