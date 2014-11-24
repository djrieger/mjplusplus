#include "../globals.hpp"
#include "UnaryExpression.hpp"

namespace ast
{
	namespace ue
	{
		UnaryExpression::UnaryExpression(shptr<Expression> child, int size)
			: child(child), size(size)
		{

		}


		std::pair<bool, bool> UnaryExpression::constBool() const
		{
			return {false, false};
		}

		shptr<Expression> UnaryExpression::createUnaryExpr(shptr<Expression> child, shptr<vec<lexer::Token::Token_type>> operator_types)
		{
			if (!operator_types->empty())
			{
				lexer::Token::Token_type last_type = lexer::Token::Token_type::TOKEN_ERROR;
				int size = 0;

				for (auto rit = operator_types->rbegin(); rit != operator_types->rend(); ++rit)
				{

					lexer::Token::Token_type current_type = *rit;

					if (current_type == last_type)
						size++;
					else if (size == 0)
					{
						last_type = current_type;
						size++;
					}
					else
					{
						if (last_type == lexer::Token::Token_type::OPERATOR_MINUS)
							child = std::make_shared<Neg>(child, size);
						else if (last_type == lexer::Token::Token_type::OPERATOR_NOT)
							child = std::make_shared<Not>(child, size);

						size = 1;
						last_type = current_type;
					}
				}

				if (last_type == lexer::Token::Token_type::OPERATOR_MINUS)
					child = std::make_shared<Neg>(child, size);
				else if (last_type == lexer::Token::Token_type::OPERATOR_NOT)
					child = std::make_shared<Not>(child, size);
			}

			return child;
		}

		void UnaryExpression::toString(std::ostream& out, unsigned int indent, std::string op, bool special) const
		{
			if (!special)
				out << '(';

			for (int i = 0; i < size; i++)
				out << op;

			child->toString(out, indent);

			if (!special)
				out << ')';
		}


		bool UnaryExpression::isLValue() const
		{
			return false;
		}

		bool UnaryExpression::standalone() const
		{
			return false;
		}

		Not::Not(shptr<Expression> child, int size)
			: UnaryExpression::UnaryExpression(child, size)
		{
			;
		}

		shptr<Type> Not::get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const
		{
			shptr<Type> child_type = child->get_type(sa, symbolTable);

			if (child_type)
			{
				if (child_type->isBool())
					return child_type;
				else
					sa.reportError(child_type->getName() + " is not boolean.");
			}

			return shptr<Type>();
		}

		void Not::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			UnaryExpression::toString(out, indent, "!", special);
		}

		std::pair<bool, bool> Not::constBool() const
		{
			auto childExpr = child->constBool();

			if (childExpr.first)
				return {true, size % 2 == 0 ? childExpr.second : !childExpr.second };

			return {false, false};
		}

		Neg::Neg(shptr<Expression> child, int size)
			: UnaryExpression::UnaryExpression(child, size)
		{
			;
		}

		shptr<Type> Neg::get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const
		{
			shptr<Type> child_type = child->get_type(sa, symbolTable);

			if (child_type)
			{
				if (child_type->isInteger())
					return child_type;
				else
					sa.reportError(child_type->getName() + " is not an integer.");
			}

			return shptr<Type>();
		}

		void Neg::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			if (!special)
				out << '(';

			for (int i = 0; i < size; i++)
			{
				if (i != 0)
					out << " -";
				else
					out << "-";
			}

			child->toString(out, indent);

			if (!special)
				out << ')';
		}

	}

}
