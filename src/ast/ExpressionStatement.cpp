#include "../globals.hpp"
#include "ExpressionStatement.hpp"

namespace ast
{
	namespace stmt
	{
		ExpressionStatement::ExpressionStatement(shptr<Expression> expr) : expression(expr)
		{

		}

		void ExpressionStatement::toString(std::ostream& out, unsigned int indent, bool) const
		{
			out << std::string(indent, '\t');
			expression->toString(out, indent, true);
			out << ";\n";
		}

		bool ExpressionStatement::analyze(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable) const
		{
			expression->get_type(sa, symbolTable);

			if (!expression->standalone())
				sa.reportError("Not a statement", expression);

			return false;
		}

		void ExpressionStatement::accept(ASTVisitor& visitor) const
		{
			visitor.visit(std::static_pointer_cast<ExpressionStatement const>(shared_from_this()));
		}

		shptr<ast::Expression> ExpressionStatement::getExpression() const
		{
			return this->expression;
		}
	}
}
