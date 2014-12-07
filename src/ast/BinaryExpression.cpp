#include "../globals.hpp"
#include "BinaryExpression.hpp"

namespace ast
{
	namespace be
	{

		BinaryExpression::BinaryExpression(shptr<Expression> leftChild, shptr<Expression> rightChild)
			: Expression(leftChild->getPosition()), leftChild(leftChild), rightChild(rightChild)
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

		shptr<Expression> BinaryExpression::getLeftChild() const
		{
			return this->leftChild;
		}
		shptr<Expression> BinaryExpression::getRightChild() const
		{
			return this->rightChild;
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


		bool BinaryExpression::standalone() const
		{
			return false;
		}


		std::pair<bool, bool> BinaryExpression::constBool() const
		{
			return {false, false};
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
				if (leftType->isSpecialType() || rightType->isSpecialType()) {
					//System or System.out are our pseudo-classes
					sa.reportError("Can't compare non-existing values of System or System.out.", leftChild);
					return shptr<Type>();
				}
				
				if (*leftType == *rightType && leftType->getPrimitiveType() != Type::Primitive_type::INT_NEG_ONLY)
					return std::make_shared<Type>(Type::Primitive_type::BOOLEAN);
				else
				{
					if (leftType->getPrimitiveType() == Type::Primitive_type::NULL_TYPE
					        && rightType->isRefType(true))
						return std::make_shared<Type>(Type::Primitive_type::BOOLEAN);

					if (rightType->getPrimitiveType() == Type::Primitive_type::NULL_TYPE
					        && leftType->isRefType(true))
						return std::make_shared<Type>(Type::Primitive_type::BOOLEAN);

					sa.reportError("Incompatible types for not-equal comparison.", leftChild);
				}
			}

			return shptr<Type>();
		}


		std::pair<bool, bool> NotEq::constBool() const
		{
			auto left = leftChild->constBool();
			auto right = rightChild->constBool();

			if (left.first && right.first) return {true, left.second != right.second};

			return {false, false};
		}

		void NotEq::accept(ASTVisitor& visitor) const
		{
			visitor.visit(std::static_pointer_cast<NotEq const>(shared_from_this()));
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
					sa.reportError("At least one mult variable is a non-integer.", leftChild);
			}

			return shptr<Type>();
		}

		void Mult::accept(ASTVisitor& visitor) const
		{
			visitor.visit(std::static_pointer_cast<Mult const>(shared_from_this()));
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
					sa.reportError("At least one addition variable is a non-integer.", leftChild);
			}

			return shptr<Type>();
		}

		void Plus::accept(ASTVisitor& visitor) const
		{
			visitor.visit(std::static_pointer_cast<Plus const>(shared_from_this()));
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
					sa.reportError("At least one subtraction variable is a non-integer.", leftChild);
			}

			return shptr<Type>();
		}

		void Minus::accept(ASTVisitor& visitor) const
		{
			visitor.visit(std::static_pointer_cast<Minus const>(shared_from_this()));
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
					sa.reportError("At least one division variable is a non-integer.", leftChild);
			}

			return shptr<Type>();
		}

		void Slash::accept(ASTVisitor& visitor) const
		{
			visitor.visit(std::static_pointer_cast<Slash const>(shared_from_this()));
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
					sa.reportError("At least one variable is a non-integer.", leftChild);
			}

			return shptr<Type>();
		}

		void LessThanEq::accept(ASTVisitor& visitor) const
		{
			visitor.visit(std::static_pointer_cast<LessThanEq const>(shared_from_this()));
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
					sa.reportError("At least one variable is a non-integer.", leftChild);
			}

			return shptr<Type>();
		}

		void LessThan::accept(ASTVisitor& visitor) const
		{
			visitor.visit(std::static_pointer_cast<LessThan const>(shared_from_this()));
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
				if (leftType->isSpecialType() || rightType->isSpecialType()) {
					//System or System.out are our pseudo-classes
					sa.reportError("Can't compare non-existing values of System or System.out.", leftChild);
					return shptr<Type>();
				}
				
				if (*leftType == *rightType && leftType->getPrimitiveType() != Type::Primitive_type::INT_NEG_ONLY)
					return std::make_shared<Type>(Type::Primitive_type::BOOLEAN);
				else
				{
					if (leftType->getPrimitiveType() == Type::Primitive_type::NULL_TYPE
					        && rightType->isRefType(true))
						return std::make_shared<Type>(Type::Primitive_type::BOOLEAN);

					if (rightType->getPrimitiveType() == Type::Primitive_type::NULL_TYPE
					        && leftType->isRefType(true))
						return std::make_shared<Type>(Type::Primitive_type::BOOLEAN);

					sa.reportError("Incompatible types for equal comparison.", leftChild);
				}
			}

			return shptr<Type>();
		}


		std::pair<bool, bool> EqEq::constBool() const
		{
			auto left = leftChild->constBool();
			auto right = rightChild->constBool();

			if (left.first && right.first) return {true, left.second == right.second};

			return {false, false};
		}

		void EqEq::accept(ASTVisitor& visitor) const
		{
			visitor.visit(std::static_pointer_cast<EqEq const>(shared_from_this()));
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
				if (leftType->isSpecialType() || rightType->isSpecialType()) {
					//System or System.out are our pseudo-classes
					sa.reportError("Can't assign to non-existing values of System or System.out.", leftChild);
					return shptr<Type>();
				}

				if (leftChild->isLValue())
				{
					if (rightType->getPrimitiveType() == Type::Primitive_type::NULL_TYPE)
					{
						if (leftType->isRefType())
							return leftType;
						else
							sa.reportError("Cannot assign null to non-reference type $type{" + leftType->getName() + "}", leftChild);
					}
					else
					{
						if (*leftType == *rightType)
							return leftType;
						else
							sa.reportError("Mismatched Types: $type{" + leftType->getName() + "} and $type{" + rightType->getName() + "}", leftChild);
					}
				}
				else
					sa.reportError("Cannot assign to an expression that is not an l-value", leftChild);
			}

			return shptr<Type>();
		}


		bool Eq::standalone() const
		{
			return true;
		}

		void Eq::accept(ASTVisitor& visitor) const
		{
			visitor.visit(std::static_pointer_cast<Eq const>(shared_from_this()));
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
					sa.reportError("At least one variable is a non-integer.", leftChild);
			}

			return shptr<Type>();
		}

		void GreaterThanEq::accept(ASTVisitor& visitor) const
		{
			visitor.visit(std::static_pointer_cast<GreaterThanEq const>(shared_from_this()));
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
					sa.reportError("At least one variable is a non-integer.", leftChild);
			}

			return shptr<Type>();
		}

		void GreaterThan::accept(ASTVisitor& visitor) const
		{
			visitor.visit(std::static_pointer_cast<GreaterThan const>(shared_from_this()));
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
					sa.reportError("At least one variable is a non-integer.", leftChild);
			}

			return shptr<Type>();
		}

		void Mod::accept(ASTVisitor& visitor) const
		{
			visitor.visit(std::static_pointer_cast<Mod const>(shared_from_this()));
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
					sa.reportError("At least one variable is a non-integer.", leftChild);
			}

			return shptr<Type>();
		}


		std::pair<bool, bool> AndAnd::constBool() const
		{
			auto left = leftChild->constBool();
			auto right = rightChild->constBool();

			if (left.first && right.first) return {true, left.second&&  right.second};

			return {false, false};
		}

		void AndAnd::accept(ASTVisitor& visitor) const
		{
			visitor.visit(std::static_pointer_cast<AndAnd const>(shared_from_this()));
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
					sa.reportError("At least one variable is a non-integer.", leftChild);
			}

			return shptr<Type>();
		}

		std::pair<bool, bool> OrOr::constBool() const
		{
			auto left = leftChild->constBool();
			auto right = rightChild->constBool();

			if (left.first && right.first) return {true, left.second || right.second};

			return {false, false};
		}

		void OrOr::accept(ASTVisitor& visitor) const
		{
			visitor.visit(std::static_pointer_cast<OrOr const>(shared_from_this()));
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

		void Invalid::accept(ASTVisitor& visitor) const
		{
			visitor.visit(std::static_pointer_cast<Invalid const>(shared_from_this()));
		}

	}

}
