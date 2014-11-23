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


		bool BinaryExpression::isLValue() const
		{
			return false;
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

		shptr<Type> NotEq::get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const
		{
			auto leftType = leftChild->get_type(sa, symbolTable);
			auto rightType = rightChild->get_type(sa, symbolTable);

			if (leftType && rightType)
			{
				if (*leftType == *rightType)
					return std::make_shared<Type>(Type::Primitive_type::BOOLEAN);
				else
				{
					if (leftType->getPrimitiveType() == Type::Primitive_type::NULL_TYPE
					        && rightType->isRefType(true))
						return std::make_shared<Type>(Type::Primitive_type::BOOLEAN);

					if (rightType->getPrimitiveType() == Type::Primitive_type::NULL_TYPE
					        && leftType->isRefType(true))
						return std::make_shared<Type>(Type::Primitive_type::BOOLEAN);

					sa.printError("Incompatible types for not-equal comparison.");
				}
			}

			return shptr<Type>();
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

		shptr<Type> Mult::get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const
		{
			auto leftType = leftChild->get_type(sa, symbolTable);
			auto rightType = rightChild->get_type(sa, symbolTable);

			if (leftType && rightType)
			{
				if (leftType->isInteger() && rightType->isInteger())
					return std::make_shared<Type>(Type::Primitive_type::INT);
				else
					sa.printError("At least one mult variable is a non-integer.");
			}

			return shptr<Type>();
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

		shptr<Type> Plus::get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const
		{
			auto leftType = leftChild->get_type(sa, symbolTable);
			auto rightType = rightChild->get_type(sa, symbolTable);

			if (leftType && rightType)
			{
				if (leftType->isInteger() && rightType->isInteger())
					return std::make_shared<Type>(Type::Primitive_type::INT);
				else
					sa.printError("At least one addition variable is a non-integer.");
			}

			return shptr<Type>();
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

		shptr<Type> Minus::get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const
		{
			auto leftType = leftChild->get_type(sa, symbolTable);
			auto rightType = rightChild->get_type(sa, symbolTable);

			if (leftType && rightType)
			{
				if (leftType->isInteger() && rightType->isInteger())
					return std::make_shared<Type>(Type::Primitive_type::INT);
				else
					sa.printError("At least one subtraction variable is a non-integer.");
			}

			return shptr<Type>();
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

		shptr<Type> Slash::get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const
		{
			auto leftType = leftChild->get_type(sa, symbolTable);
			auto rightType = rightChild->get_type(sa, symbolTable);

			if (leftType && rightType)
			{
				if (leftType->isInteger() && rightType->isInteger())
					return std::make_shared<Type>(Type::Primitive_type::INT);
				else
					sa.printError("At least one division variable is a non-integer.");
			}

			return shptr<Type>();
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

		shptr<Type> LessThanEq::get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const
		{
			auto leftType = leftChild->get_type(sa, symbolTable);
			auto rightType = rightChild->get_type(sa, symbolTable);

			if (leftType && rightType)
			{
				if (leftType->isInteger() && rightType->isInteger())
					return std::make_shared<Type>(Type::Primitive_type::BOOLEAN);
				else
					sa.printError("At least one variable is a non-integer.");
			}

			return shptr<Type>();
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

		shptr<Type> LessThan::get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const
		{
			auto leftType = leftChild->get_type(sa, symbolTable);
			auto rightType = rightChild->get_type(sa, symbolTable);

			if (leftType && rightType)
			{
				if (leftType->isInteger() && rightType->isInteger())
					return std::make_shared<Type>(Type::Primitive_type::BOOLEAN);
				else
					sa.printError("At least one variable is a non-integer.");
			}

			return shptr<Type>();
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

		shptr<Type> EqEq::get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const
		{
			auto leftType = leftChild->get_type(sa, symbolTable);
			auto rightType = rightChild->get_type(sa, symbolTable);

			if (leftType && rightType)
			{
				if (*leftType == *rightType)
					return std::make_shared<Type>(Type::Primitive_type::BOOLEAN);
				else
				{
					if (leftType->getPrimitiveType() == Type::Primitive_type::NULL_TYPE
					        && rightType->isRefType(true))
						return std::make_shared<Type>(Type::Primitive_type::BOOLEAN);

					if (rightType->getPrimitiveType() == Type::Primitive_type::NULL_TYPE
					        && leftType->isRefType(true))
						return std::make_shared<Type>(Type::Primitive_type::BOOLEAN);

					sa.printError("Incompatible types for equal comparison.");
				}
			}

			return shptr<Type>();
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

		shptr<Type> Eq::get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const
		{
			auto leftType = leftChild->get_type(sa, symbolTable);
			auto rightType = rightChild->get_type(sa, symbolTable);

			if (leftType && rightType)
			{

				if (leftChild->isLValue())
				{
					if (rightType->getPrimitiveType() == Type::Primitive_type::NULL_TYPE)
					{
						if (leftType->isRefType())
							return leftType;
						else
							sa.printError("");
					}
					else
					{
						if (*leftType == *rightType)
							return leftType;
						else
							sa.printError("");
					}
				}
				else
					sa.printError("");
			}

			return shptr<Type>();
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

		shptr<Type> GreaterThanEq::get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const
		{
			auto leftType = leftChild->get_type(sa, symbolTable);
			auto rightType = rightChild->get_type(sa, symbolTable);

			if (leftType && rightType)
			{
				if (leftType->isInteger() && rightType->isInteger())
					return std::make_shared<Type>(Type::Primitive_type::BOOLEAN);
				else
					sa.printError("At least one variable is a non-integer.");
			}

			return shptr<Type>();
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

		shptr<Type> GreaterThan::get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const
		{
			auto leftType = leftChild->get_type(sa, symbolTable);
			auto rightType = rightChild->get_type(sa, symbolTable);

			if (leftType && rightType)
			{
				if (leftType->isInteger() && rightType->isInteger())
					return std::make_shared<Type>(Type::Primitive_type::BOOLEAN);
				else
					sa.printError("At least one variable is a non-integer.");
			}

			return shptr<Type>();
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

		shptr<Type> Mod::get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const
		{
			auto leftType = leftChild->get_type(sa, symbolTable);
			auto rightType = rightChild->get_type(sa, symbolTable);

			if (leftType && rightType)
			{
				if (leftType->isInteger() && rightType->isInteger())
					return std::make_shared<Type>(Type::Primitive_type::INT);
				else
					sa.printError("At least one variable is a non-integer.");
			}

			return shptr<Type>();
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

		shptr<Type> AndAnd::get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const
		{
			auto leftType = leftChild->get_type(sa, symbolTable);
			auto rightType = rightChild->get_type(sa, symbolTable);

			if (leftType && rightType)
			{
				if (leftType->isBool() && rightType->isBool())
					return std::make_shared<Type>(Type::Primitive_type::BOOLEAN);
				else
					sa.printError("At least one variable is a non-integer.");
			}

			return shptr<Type>();
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

		shptr<Type> OrOr::get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const
		{
			auto leftType = leftChild->get_type(sa, symbolTable);
			auto rightType = rightChild->get_type(sa, symbolTable);

			if (leftType && rightType)
			{
				if (leftType->isBool() && rightType->isBool())
					return std::make_shared<Type>(Type::Primitive_type::BOOLEAN);
				else
					sa.printError("At least one variable is a non-integer.");
			}

			return shptr<Type>();
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

		shptr<Type> Invalid::get_type(SemanticAnalysis&, shptr<SymbolTable>) const
		{
			//Our Parser should never actually allow us to create an Invalid BinaryExpression...
			return shptr<Type>();
		}

	}

}
