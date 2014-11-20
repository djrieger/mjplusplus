#include "../globals.hpp"
#include "BinaryExpression.hpp"

namespace ast
{
	namespace be
	{

		BinaryExpression::BinaryExpression(shptr<Expression> leftChild, shptr<Expression> rightChild)
			: leftChild(leftChild), rightChild(rightChild)
		{
			;
		}

		void BinaryExpression::toString(std::ostream& out, unsigned int indent, std::string op, bool special) const
		{
			if (!special)
				out << '(';

			leftChild->toString(out, indent);
			out << ' ' << op << ' ';
			rightChild->toString(out, indent);

			if (!special)
				out << ')';
		}

		shptr<BinaryExpression> BinaryExpression::createBinaryExpr(shptr<Expression> leftChild, shptr<Expression> rightChild, lexer::Token::Token_type operator_type)
		{
			shptr<BinaryExpression> binaryExpr;

			switch (operator_type)
			{
				case lexer::Token::Token_type::OPERATOR_NOTEQ:
					binaryExpr = std::make_shared<NotEq>(leftChild, rightChild);
					break;

				case lexer::Token::Token_type::OPERATOR_MULT:
					binaryExpr = std::make_shared<Mult>(leftChild, rightChild);
					break;

				case lexer::Token::Token_type::OPERATOR_PLUS:
					binaryExpr = std::make_shared<Plus>(leftChild, rightChild);
					break;

				case lexer::Token::Token_type::OPERATOR_MINUS:
					binaryExpr = std::make_shared<Minus>(leftChild, rightChild);
					break;

				case lexer::Token::Token_type::OPERATOR_SLASH:
					binaryExpr = std::make_shared<Slash>(leftChild, rightChild);
					break;

				case lexer::Token::Token_type::OPERATOR_LTEQ:
					binaryExpr = std::make_shared<LessThanEq>(leftChild, rightChild);
					break;

				case lexer::Token::Token_type::OPERATOR_LT:
					binaryExpr = std::make_shared<LessThan>(leftChild, rightChild);
					break;

				case lexer::Token::Token_type::OPERATOR_EQEQ:
					binaryExpr = std::make_shared<EqEq>(leftChild, rightChild);
					break;

				case lexer::Token::Token_type::OPERATOR_EQ:
					binaryExpr = std::make_shared<Eq>(leftChild, rightChild);
					break;

				case lexer::Token::Token_type::OPERATOR_GTEQ:
					binaryExpr = std::make_shared<GreaterThanEq>(leftChild, rightChild);
					break;

				case lexer::Token::Token_type::OPERATOR_GT:
					binaryExpr = std::make_shared<GreaterThan>(leftChild, rightChild);
					break;

				case lexer::Token::Token_type::OPERATOR_MOD:
					binaryExpr = std::make_shared<Mod>(leftChild, rightChild);
					break;

				case lexer::Token::Token_type::OPERATOR_ANDAND:
					binaryExpr = std::make_shared<AndAnd>(leftChild, rightChild);
					break;

				case lexer::Token::Token_type::OPERATOR_OROR:
					binaryExpr = std::make_shared<OrOr>(leftChild, rightChild);
					break;

				default:
					binaryExpr = std::make_shared<Invalid>(leftChild, rightChild);
					break;
			}

			return binaryExpr;
		}

		NotEq::NotEq(shptr<Expression> leftChild, shptr<Expression> rightChild)
			: BinaryExpression::BinaryExpression(leftChild, rightChild)
		{
			;
		}

		void NotEq::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "!=", special);
		}


		Mult::Mult(shptr<Expression> leftChild, shptr<Expression> rightChild)
			: BinaryExpression::BinaryExpression(leftChild, rightChild)
		{
			;
		}

		void Mult::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "*", special);
		}

		Plus::Plus(shptr<Expression> leftChild, shptr<Expression> rightChild)
			: BinaryExpression::BinaryExpression(leftChild, rightChild)
		{
			;
		}

		void Plus::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "+", special);
		}

		Minus::Minus(shptr<Expression> leftChild, shptr<Expression> rightChild)
			: BinaryExpression::BinaryExpression(leftChild, rightChild)
		{
			;
		}

		void Minus::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "-", special);
		}

		Slash::Slash(shptr<Expression> leftChild, shptr<Expression> rightChild)
			: BinaryExpression::BinaryExpression(leftChild, rightChild)
		{
			;
		}

		void Slash::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "/", special);
		}

		LessThanEq::LessThanEq(shptr<Expression> leftChild, shptr<Expression> rightChild)
			: BinaryExpression::BinaryExpression(leftChild, rightChild)
		{
			;
		}

		void LessThanEq::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "<=", special);
		}

		LessThan::LessThan(shptr<Expression> leftChild, shptr<Expression> rightChild)
			: BinaryExpression::BinaryExpression(leftChild, rightChild)
		{
			;
		}

		void LessThan::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "<", special);
		}

		EqEq::EqEq(shptr<Expression> leftChild, shptr<Expression> rightChild)
			: BinaryExpression::BinaryExpression(leftChild, rightChild)
		{
			;
		}

		void EqEq::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "==", special);
		}

		Eq::Eq(shptr<Expression> leftChild, shptr<Expression> rightChild)
			: BinaryExpression::BinaryExpression(leftChild, rightChild)
		{
			;
		}

		void Eq::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "=", special);
		}

		GreaterThanEq::GreaterThanEq(shptr<Expression> leftChild, shptr<Expression> rightChild)
			: BinaryExpression::BinaryExpression(leftChild, rightChild)
		{
			;
		}

		void GreaterThanEq::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, ">=", special);
		}


		GreaterThan::GreaterThan(shptr<Expression> leftChild, shptr<Expression> rightChild)
			: BinaryExpression::BinaryExpression(leftChild, rightChild)
		{
			;
		}

		void GreaterThan::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, ">", special);
		}

		Mod::Mod(shptr<Expression> leftChild, shptr<Expression> rightChild)
			: BinaryExpression::BinaryExpression(leftChild, rightChild)
		{
			;
		}

		void Mod::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "%", special);
		}

		AndAnd::AndAnd(shptr<Expression> leftChild, shptr<Expression> rightChild)
			: BinaryExpression::BinaryExpression(leftChild, rightChild)
		{
			;
		}

		void AndAnd::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "&&", special);
		}

		OrOr::OrOr(shptr<Expression> leftChild, shptr<Expression> rightChild)
			: BinaryExpression::BinaryExpression(leftChild, rightChild)
		{
			;
		}

		void OrOr::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "||", special);
		}

		Invalid::Invalid(shptr<Expression> leftChild, shptr<Expression> rightChild)
			: BinaryExpression::BinaryExpression(leftChild, rightChild)
		{
			;
		}

		void Invalid::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "NO_VALID_OPERATOR", special);
		}

	}

}
