#ifndef STATEMENT_VISITOR_HPP
#define STATEMENT_VISITOR_HPP

#include "FirmVisitor.hpp"
#include "MemberVisitor.hpp"
#include "../../ast/IfStatement.hpp"
#include "../../ast/WhileStatement.hpp"
#include "../../ast/ReturnStatement.hpp"
#include "../../ast/Block.hpp"
#include "../../ast/ExpressionStatement.hpp"
#include "../../ast/LVDStatement.hpp"

namespace firm
{
	namespace visitor
	{
		class StatementVisitor : public FirmVisitor
		{
			protected:
				MemberVisitor& memberVisitor;

				/**
				 * Create, mature and set as current a firm block for a then or else statement
				 * @param thenOrElseStmt	The then or else statement of an ast::IfStatement
				 * @param precedingProjection	The Firm projection node to use as predecessor for the then/else block
				 * @param exitBlock	The exitBlock to use as successor for the Jump node
				 */
				void visitThenOrElse(ir_node* thenOrElseBlock, shptr<const ast::stmt::Statement> thenOrElseStmt, ir_node* exitBlock);
			public:
				StatementVisitor(MemberVisitor& memberVisitor);

				virtual void visit(shptr<const ast::stmt::Block> blockStmt);
				virtual void visit(shptr<const ast::stmt::IfStatement> ifStatement);
				virtual void visit(shptr<const ast::stmt::WhileStatement> whileStmt);
				virtual void visit(shptr<const ast::stmt::ReturnStatement> returnStmt);
				virtual void visit(shptr<const ast::stmt::ExpressionStatement> exprStmt);
				virtual void visit(shptr<const ast::stmt::LVDStatement> lvdStmt);
		};
	}
}
#endif
