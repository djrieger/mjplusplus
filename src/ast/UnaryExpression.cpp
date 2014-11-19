#include "../globals.hpp"
#include "UnaryExpression.hpp"

namespace ast
{
	namespace ue
	{
		UnaryExpression::UnaryExpression(uptr<Expression> child, int size)
			: child(std::move(child)), size(size)
		{

		}

		uptr<Expression> UnaryExpression::createUnaryExpr(uptr<Expression> child, uptr<vec<Token::Token_type>> operator_types)
		{
			if (!operator_types->empty())
			{
				Token::Token_type last_type = Token::Token_type::TOKEN_ERROR;
				int size = 0;

				for (auto rit = operator_types->rbegin(); rit != operator_types->rend(); ++rit)
				{

					Token::Token_type current_type = *rit;

					if (current_type == last_type)
						size++;
					else if (size == 0)
					{
						last_type = current_type;
						size++;
					}
					else
					{
						if (last_type == Token::Token_type::OPERATOR_MINUS)
							child = std::make_unique<Neg>(std::move(child), size);
						else if (last_type == Token::Token_type::OPERATOR_NOT)
							child = std::make_unique<Not>(std::move(child), size);

						size = 1;
						last_type = current_type;
					}
				}

				if (last_type == Token::Token_type::OPERATOR_MINUS)
					child = std::make_unique<Neg>(std::move(child), size);
				else if (last_type == Token::Token_type::OPERATOR_NOT)
					child = std::make_unique<Not>(std::move(child), size);
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

		Not::Not(uptr<Expression> child, int size)
			: UnaryExpression::UnaryExpression(std::move(child), size)
		{
			;
		}

		void Not::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			UnaryExpression::toString(out, indent, "!", special);
		}

		Neg::Neg(uptr<Expression> child, int size)
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
