#ifndef STATEMENT_VISITOR_HPP
#define STATEMENT_VISITOR_HPP

#include "FirmVisitor.hpp"
#include "../ast/IfStatement.hpp"
#include "../ast/WhileStatement.hpp"
#include "../ast/ReturnStatement.hpp"
#include "../ast/Block.hpp"
#include "../ast/ExpressionStatement.hpp"
#include "../ast/LVDStatement.hpp"

class StatementVisitor : public FirmVisitor
{
	protected:
		ir_node* resultNode;
	public:
		virtual void visit(shptr<const ast::IfStatement> ifStatement);
		virtual void visit(shptr<const ast::WhileStatement> whileStmt);
		virtual void visit(shptr<const ast::ReturnStatement> returnStmt);

		// Not implemented yet:
		virtual void visit(shptr<const ast::Block> blockStmt);
		virtual void visit(shptr<const ast::ExpressionStatement> exprStmt);
		virtual void visit(shptr<const ast::LVDStatement> lvdStmt);

		virtual ir_node* getResultNode() const;
};

#endif
