#include "../globals.hpp"
#include "BinaryExpression.hpp"

ast::BinaryExpression::BinaryExpression(uptr<Expression>& leftChild, uptr<Expression>& rightChild, Token::Token_type const& operator_type)
	: leftChild(std::move(leftChild)), rightChild(std::move(rightChild)), operator_type(operator_type)
{

}

void ast::BinaryExpression::toString(std::ostream& out, unsigned int indent, bool special) const
{
	std::string op = "";

	switch (operator_type)
	{
		case Token::Token_type::OPERATOR_NOTEQ:
			op = "!=";
			break;

		case Token::Token_type::OPERATOR_NOT:
			op = "!";
			break;

		case Token::Token_type::OPERATOR_LPAREN:
			op = "(";
			break;

		case Token::Token_type::OPERATOR_RPAREN:
			op = ")";
			break;

		case Token::Token_type::OPERATOR_MULTEQ:
			op = "*=";
			break;

		case Token::Token_type::OPERATOR_MULT:
			op = "*";
			break;

		case Token::Token_type::OPERATOR_PLUPLUS:
			op = "++";
			break;

		case Token::Token_type::OPERATOR_PLUSEQ:
			op = "+=";
			break;

		case Token::Token_type::OPERATOR_PLUS:
			op = "+";
			break;

		case Token::Token_type::OPERATOR_COMMA:
			op = ",";
			break;

		case Token::Token_type::OPERATOR_MINUSEQ:
			op = "-=";
			break;

		case Token::Token_type::OPERATOR_MINUSMINUS:
			op = "--";
			break;

		case Token::Token_type::OPERATOR_MINUS:
			op = "-";
			break;

		case Token::Token_type::OPERATOR_DOT:
			op = ".";
			break;

		case Token::Token_type::OPERATOR_SLASHEQ:
			op = "/=";
			break;

		case Token::Token_type::OPERATOR_SLASH:
			op = "/";
			break;

		case Token::Token_type::OPERATOR_COLON:
			op = ":";
			break;

		case Token::Token_type::OPERATOR_SEMICOLON:
			op = ";";
			break;

		case Token::Token_type::OPERATOR_LTLTEQ:
			op = "<<=";
			break;

		case Token::Token_type::OPERATOR_LTLT:
			op = "<<";
			break;

		case Token::Token_type::OPERATOR_LTEQ:
			op = "<=";
			break;

		case Token::Token_type::OPERATOR_LT:
			op = "<";
			break;

		case Token::Token_type::OPERATOR_EQEQ:
			op = "==";
			break;

		case Token::Token_type::OPERATOR_EQ:
			op = "=";
			break;

		case Token::Token_type::OPERATOR_GTEQ:
			op = ">=";
			break;

		case Token::Token_type::OPERATOR_GTGTEQ:
			op = ">>=";
			break;

		case Token::Token_type::OPERATOR_GTGTGTEQ:
			op = ">>>=";
			break;

		case Token::Token_type::OPERATOR_GTGTGT:
			op = ">>>";
			break;

		case Token::Token_type::OPERATOR_GTGT:
			op = ">>";
			break;

		case Token::Token_type::OPERATOR_GT:
			op = ">";
			break;

		case Token::Token_type::OPERATOR_QUESTION:
			op = "?";
			break;

		case Token::Token_type::OPERATOR_MODEQ:
			op = "%=";
			break;

		case Token::Token_type::OPERATOR_MOD:
			op = "%";
			break;

		case Token::Token_type::OPERATOR_ANDEQ:
			op = "&=";
			break;

		case Token::Token_type::OPERATOR_ANDAND:
			op = "&&";
			break;

		case Token::Token_type::OPERATOR_AND:
			op = "&";
			break;

		case Token::Token_type::OPERATOR_LBRACKET:
			op = "[";
			break;

		case Token::Token_type::OPERATOR_RBRACKET:
			op = "]";
			break;

		case Token::Token_type::OPERATOR_XOREQ:
			op = "^=";
			break;

		case Token::Token_type::OPERATOR_XOR:
			op = "^";
			break;

		case Token::Token_type::OPERATOR_LBRACE:
			op = "{";
			break;

		case Token::Token_type::OPERATOR_RBRACE:
			op = "}";
			break;

		case Token::Token_type::OPERATOR_NEG:
			op = "-";
			break;

		case Token::Token_type::OPERATOR_OREQ:
			op = "|=";
			break;

		case Token::Token_type::OPERATOR_OROR:
			op = "||";
			break;

		case Token::Token_type::OPERATOR_OR:
			op = "|";
			break;

		default:
			op = "NO_VALID_OPERATOR";
	}

	if (!special)
		out << '(';

	leftChild->toString(out, indent);
	out << ' ' << op << ' ';
	rightChild->toString(out, indent);

	if (!special)
		out << ')';
}
