#include "../globals.hpp"
#include "BinaryExpression.hpp"

namespace ast
{
	namespace be
	{

		BinaryExpression::BinaryExpression(uptr<Expression> leftChild, uptr<Expression> rightChild)
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

		uptr<BinaryExpression> BinaryExpression::createBinaryExpr(uptr<Expression> leftChild, uptr<Expression> rightChild, Token::Token_type operator_type)
		{
			uptr<BinaryExpression> binaryExpr;

			switch (operator_type)
			{
				case Token::Token_type::OPERATOR_NOTEQ:
					binaryExpr = std::make_unique<NotEq>(std::move(leftChild), std::move(rightChild));
					break;

				case Token::Token_type::OPERATOR_MULT:
					binaryExpr = std::make_unique<Mult>(std::move(leftChild), std::move(rightChild));
					break;

				case Token::Token_type::OPERATOR_PLUS:
					binaryExpr = std::make_unique<Plus>(std::move(leftChild), std::move(rightChild));
					break;

				case Token::Token_type::OPERATOR_MINUS:
					binaryExpr = std::make_unique<Minus>(std::move(leftChild), std::move(rightChild));
					break;

				case Token::Token_type::OPERATOR_SLASH:
					binaryExpr = std::make_unique<Slash>(std::move(leftChild), std::move(rightChild));
					break;

				case Token::Token_type::OPERATOR_LTEQ:
					binaryExpr = std::make_unique<LessThanEq>(std::move(leftChild), std::move(rightChild));
					break;

				case Token::Token_type::OPERATOR_LT:
					binaryExpr = std::make_unique<LessThan>(std::move(leftChild), std::move(rightChild));
					break;

				case Token::Token_type::OPERATOR_EQEQ:
					binaryExpr = std::make_unique<EqEq>(std::move(leftChild), std::move(rightChild));
					break;

				case Token::Token_type::OPERATOR_EQ:
					binaryExpr = std::make_unique<Eq>(std::move(leftChild), std::move(rightChild));
					break;

				case Token::Token_type::OPERATOR_GTEQ:
					binaryExpr = std::make_unique<GreaterThanEq>(std::move(leftChild), std::move(rightChild));
					break;

				case Token::Token_type::OPERATOR_GT:
					binaryExpr = std::make_unique<GreaterThan>(std::move(leftChild), std::move(rightChild));
					break;

				case Token::Token_type::OPERATOR_MOD:
					binaryExpr = std::make_unique<Mod>(std::move(leftChild), std::move(rightChild));
					break;

				case Token::Token_type::OPERATOR_ANDAND:
					binaryExpr = std::make_unique<AndAnd>(std::move(leftChild), std::move(rightChild));
					break;

				case Token::Token_type::OPERATOR_OROR:
					binaryExpr = std::make_unique<OrOr>(std::move(leftChild), std::move(rightChild));
					break;

				default:
					binaryExpr = std::make_unique<Invalid>(std::move(leftChild), std::move(rightChild));
					break;
			}

			return binaryExpr;
		}

		NotEq::NotEq(uptr<Expression> leftChild, uptr<Expression> rightChild)
			: BinaryExpression::BinaryExpression(std::move(leftChild), std::move(rightChild))
		{
			;
		}

		void NotEq::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "!=", special);
		}


		Mult::Mult(uptr<Expression> leftChild, uptr<Expression> rightChild)
			: BinaryExpression::BinaryExpression(std::move(leftChild), std::move(rightChild))
		{
			;
		}

		void Mult::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "*", special);
		}

		Plus::Plus(uptr<Expression> leftChild, uptr<Expression> rightChild)
			: BinaryExpression::BinaryExpression(std::move(leftChild), std::move(rightChild))
		{
			;
		}

		void Plus::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "+", special);
		}

		Minus::Minus(uptr<Expression> leftChild, uptr<Expression> rightChild)
			: BinaryExpression::BinaryExpression(std::move(leftChild), std::move(rightChild))
		{
			;
		}

		void Minus::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "-", special);
		}

		Slash::Slash(uptr<Expression> leftChild, uptr<Expression> rightChild)
			: BinaryExpression::BinaryExpression(std::move(leftChild), std::move(rightChild))
		{
			;
		}

		void Slash::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "/", special);
		}

		LessThanEq::LessThanEq(uptr<Expression> leftChild, uptr<Expression> rightChild)
			: BinaryExpression::BinaryExpression(std::move(leftChild), std::move(rightChild))
		{
			;
		}

		void LessThanEq::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "<=", special);
		}

		LessThan::LessThan(uptr<Expression> leftChild, uptr<Expression> rightChild)
			: BinaryExpression::BinaryExpression(std::move(leftChild), std::move(rightChild))
		{
			;
		}

		void LessThan::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "<", special);
		}

		EqEq::EqEq(uptr<Expression> leftChild, uptr<Expression> rightChild)
			: BinaryExpression::BinaryExpression(std::move(leftChild), std::move(rightChild))
		{
			;
		}

		void EqEq::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "==", special);
		}

		Eq::Eq(uptr<Expression> leftChild, uptr<Expression> rightChild)
			: BinaryExpression::BinaryExpression(std::move(leftChild), std::move(rightChild))
		{
			;
		}

		void Eq::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "=", special);
		}

		GreaterThanEq::GreaterThanEq(uptr<Expression> leftChild, uptr<Expression> rightChild)
			: BinaryExpression::BinaryExpression(std::move(leftChild), std::move(rightChild))
		{
			;
		}

		void GreaterThanEq::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, ">=", special);
		}


		GreaterThan::GreaterThan(uptr<Expression> leftChild, uptr<Expression> rightChild)
			: BinaryExpression::BinaryExpression(std::move(leftChild), std::move(rightChild))
		{
			;
		}

		void GreaterThan::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, ">", special);
		}

		Mod::Mod(uptr<Expression> leftChild, uptr<Expression> rightChild)
			: BinaryExpression::BinaryExpression(std::move(leftChild), std::move(rightChild))
		{
			;
		}

		void Mod::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "%", special);
		}

		AndAnd::AndAnd(uptr<Expression> leftChild, uptr<Expression> rightChild)
			: BinaryExpression::BinaryExpression(std::move(leftChild), std::move(rightChild))
		{
			;
		}

		void AndAnd::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "&&", special);
		}

		OrOr::OrOr(uptr<Expression> leftChild, uptr<Expression> rightChild)
			: BinaryExpression::BinaryExpression(std::move(leftChild), std::move(rightChild))
		{
			;
		}

		void OrOr::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			BinaryExpression::toString(out, indent, "||", special);
		}

		Invalid::Invalid(uptr<Expression> leftChild, uptr<Expression> rightChild)
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

