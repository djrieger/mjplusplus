#include "../globals.hpp"
#include "UnaryExpression.hpp"

namespace ast
{
	namespace ue
	{
		UnaryExpression::UnaryExpression(shptr<Expression> child, int size)
			: child(std::move(child)), size(size)
		{

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
							child = std::make_shared<Neg>(std::move(child), size);
						else if (last_type == lexer::Token::Token_type::OPERATOR_NOT)
							child = std::make_shared<Not>(std::move(child), size);

						size = 1;
						last_type = current_type;
					}
				}

				if (last_type == lexer::Token::Token_type::OPERATOR_MINUS)
					child = std::make_shared<Neg>(std::move(child), size);
				else if (last_type == lexer::Token::Token_type::OPERATOR_NOT)
					child = std::make_shared<Not>(std::move(child), size);
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

		Not::Not(shptr<Expression> child, int size)
			: UnaryExpression::UnaryExpression(std::move(child), size)
		{
			;
		}

		void Not::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			UnaryExpression::toString(out, indent, "!", special);
		}

		Neg::Neg(shptr<Expression> child, int size)
			: UnaryExpression::UnaryExpression(std::move(child), size)
		{
			;
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
