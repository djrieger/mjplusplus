#include "../globals.hpp"
#include "PostfixExpression.hpp"

namespace ast
{
	namespace po
	{
		PostfixExpression::PostfixExpression(shptr<Expression> child, shptr<vec<shptr<PostfixOp>>> postfix_op) : Expression(child->getPosition()), child(child), postfix_op(postfix_op)
		{

		}

		void PostfixExpression::toString(std::ostream& out, unsigned int indent, bool special) const
		{
			if (!special && !postfix_op->empty())
				out << '(';

			if (postfix_op->size() > 1)
				out << std::string(postfix_op->size() - 1, '(');

			child->toString(out, indent);

			for (auto it = postfix_op->begin(); it != postfix_op->end();)
			{
				(**it).toString(out, indent);

				if (++it != postfix_op->end())
					out << ')';
			}

			if (!special && !postfix_op->empty())
				out << ')';
		}

		shptr<ast::Type> PostfixExpression::get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const
		{
			auto callingType = child->get_type(sa, symbolTable);

			if (callingType)
			{
				for (auto& it : *postfix_op)
				{
					callingType = it->get_type(sa, symbolTable, callingType);

					if (!callingType)
						break;
				}
			}

			return callingType;
		}

		bool PostfixExpression::isLValue() const
		{
			return postfix_op->size() > 0 && postfix_op->back()->lValueHelp();
		}


		bool PostfixExpression::standalone() const
		{
			//we have a method invocation iff we don't have an l-value at the end
			return postfix_op->size() > 0 && !postfix_op->back()->lValueHelp();
		}


		std::pair<bool, bool> PostfixExpression::constBool() const
		{
			return {false, false};
		}


		void PostfixExpression::accept(ASTVisitor& visitor) const
		{
			visitor.visit(std::static_pointer_cast<PostfixExpression const>(shared_from_this()));
		}

		shptr<Expression> PostfixExpression::getChild() const
		{
			return this->child;
		}

		shptr<vec<shptr<PostfixOp>>> PostfixExpression::getPostfixOps() const
		{
			return this->postfix_op;
		}
	}
}
