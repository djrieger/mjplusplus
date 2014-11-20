#include "../globals.hpp"
#include "BinaryExpression.hpp"

namespace ast
{
	namespace be
	{

		BinaryExpression::BinaryExpression(shptr<Expression> leftChild, shptr<Expression> rightChild)
			: leftChild(std::move(leftChild)), rightChild(std::move(rightChild))
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
					binaryExpr = std::make_shared<NotEq>(std::move(leftChild), std::move(rightChild));
					break;

				case lexer::Token::Token_type::OPERATOR_MULT:
					binaryExpr = std::make_shared<Mult>(std::move(leftChild), std::move(rightChild));
					break;

				case lexer::Token::Token_type::OPERATOR_PLUS:
					binaryExpr = std::make_shared<Plus>(std::move(leftChild), std::move(rightChild));
					break;

				case lexer::Token::Token_type::OPERATOR_MINUS:
					binaryExpr = std::make_shared<Minus>(std::move(leftChild), std::move(rightChild));
					break;

				case lexer::Token::Token_type::OPERATOR_SLASH:
					binaryExpr = std::make_shared<Slash>(std::move(leftChild), std::move(rightChild));
					break;

				case lexer::Token::Token_type::OPERATOR_LTEQ:
					binaryExpr = std::make_shared<LessThanEq>(std::move(leftChild), std::move(rightChild));
					break;

				case lexer::Token::Token_type::OPERATOR_LT:
					binaryExpr = std::make_shared<LessThan>(std::move(leftChild), std::move(rightChild));
					break;

				case lexer::Token::Token_type::OPERATOR_EQEQ:
					binaryExpr = std::make_shared<EqEq>(std::move(leftChild), std::move(rightChild));
					break;

				case lexer::Token::Token_type::OPERATOR_EQ:
					binaryExpr = std::make_shared<Eq>(std::move(leftChild), std::move(rightChild));
					break;

				case lexer::Token::Token_type::OPERATOR_GTEQ:
					binaryExpr = std::make_shared<GreaterThanEq>(std::move(leftChild), std::move(rightChild));
					break;

				case lexer::Token::Token_type::OPERATOR_GT:
					binaryExpr = std::make_shared<GreaterThan>(std::move(leftChild), std::move(rightChild));
					break;

				case lexer::Token::Token_type::OPERATOR_MOD:
					binaryExpr = std::make_shared<Mod>(std::move(leftChild), std::move(rightChild));
					break;

				case lexer::Token::Token_type::OPERATOR_ANDAND:
					binaryExpr = std::make_shared<AndAnd>(std::move(leftChild), std::move(rightChild));
					break;

				case lexer::Token::Token_type::OPERATOR_OROR:
					binaryExpr = std::make_shared<OrOr>(std::move(leftChild), std::move(rightChild));
					break;

				default:
					binaryExpr = std::make_shared<Invalid>(std::move(leftChild), std::move(rightChild));
					break;
			}

			return binaryExpr;
		}

		NotEq::NotEq(shptr<Expression> leftChild, shptr<Expression> rightChild)
			: BinaryExpression::BinaryExpression(std::move(leftChild), std::move(rightChild))
		{
			;
		}

		void NotEq::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "!=", special);
		}


		Mult::Mult(shptr<Expression> leftChild, shptr<Expression> rightChild)
			: BinaryExpression::BinaryExpression(std::move(leftChild), std::move(rightChild))
		{
			;
		}

		void Mult::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "*", special);
		}

		Plus::Plus(shptr<Expression> leftChild, shptr<Expression> rightChild)
			: BinaryExpression::BinaryExpression(std::move(leftChild), std::move(rightChild))
		{
			;
		}

		void Plus::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "+", special);
		}

		Minus::Minus(shptr<Expression> leftChild, shptr<Expression> rightChild)
			: BinaryExpression::BinaryExpression(std::move(leftChild), std::move(rightChild))
		{
			;
		}

		void Minus::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "-", special);
		}

		Slash::Slash(shptr<Expression> leftChild, shptr<Expression> rightChild)
			: BinaryExpression::BinaryExpression(std::move(leftChild), std::move(rightChild))
		{
			;
		}

		void Slash::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "/", special);
		}

		LessThanEq::LessThanEq(shptr<Expression> leftChild, shptr<Expression> rightChild)
			: BinaryExpression::BinaryExpression(std::move(leftChild), std::move(rightChild))
		{
			;
		}

		void LessThanEq::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "<=", special);
		}

		LessThan::LessThan(shptr<Expression> leftChild, shptr<Expression> rightChild)
			: BinaryExpression::BinaryExpression(std::move(leftChild), std::move(rightChild))
		{
			;
		}

		void LessThan::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "<", special);
		}

		EqEq::EqEq(shptr<Expression> leftChild, shptr<Expression> rightChild)
			: BinaryExpression::BinaryExpression(std::move(leftChild), std::move(rightChild))
		{
			;
		}

		void EqEq::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "==", special);
		}

		Eq::Eq(shptr<Expression> leftChild, shptr<Expression> rightChild)
			: BinaryExpression::BinaryExpression(std::move(leftChild), std::move(rightChild))
		{
			;
		}

		void Eq::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "=", special);
		}

		GreaterThanEq::GreaterThanEq(shptr<Expression> leftChild, shptr<Expression> rightChild)
			: BinaryExpression::BinaryExpression(std::move(leftChild), std::move(rightChild))
		{
			;
		}

		void GreaterThanEq::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, ">=", special);
		}


		GreaterThan::GreaterThan(shptr<Expression> leftChild, shptr<Expression> rightChild)
			: BinaryExpression::BinaryExpression(std::move(leftChild), std::move(rightChild))
		{
			;
		}

		void GreaterThan::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, ">", special);
		}

		Mod::Mod(shptr<Expression> leftChild, shptr<Expression> rightChild)
			: BinaryExpression::BinaryExpression(std::move(leftChild), std::move(rightChild))
		{
			;
		}

		void Mod::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "%", special);
		}

		AndAnd::AndAnd(shptr<Expression> leftChild, shptr<Expression> rightChild)
			: BinaryExpression::BinaryExpression(std::move(leftChild), std::move(rightChild))
		{
			;
		}

		void AndAnd::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "&&", special);
		}

		OrOr::OrOr(shptr<Expression> leftChild, shptr<Expression> rightChild)
			: BinaryExpression::BinaryExpression(std::move(leftChild), std::move(rightChild))
		{
			;
		}

		void OrOr::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "||", special);
		}

		Invalid::Invalid(shptr<Expression> leftChild, shptr<Expression> rightChild)
			: BinaryExpression::BinaryExpression(std::move(leftChild), std::move(rightChild))
		{
			;
		}

		void Invalid::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "NO_VALID_OPERATOR", special);
		}

	}

}
