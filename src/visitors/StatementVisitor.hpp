#ifndef STATEMENT_VISITOR_HPP
#define STATEMENT_VISITOR_HPP

#include "FirmVisitor.hpp"
#include "../ast/ClassDeclaration.hpp"

class StatementVisitor : public FirmVisitor
{
	public:
		virtual void visit(shptr<const ast::IfStatement> ifStmt);
		virtual void visit(shptr<const ast::WhileStatement> whileStmt);
		virtual void visit(shptr<const ast::ReturnStatement> returnStmt);
};

#endif
