#include "StatementVisitor.hpp"
#include "BoolExpressionVisitor.hpp"
#include "ExpressionVisitor.hpp"

namespace firm
{
	namespace visitor
	{
		StatementVisitor::StatementVisitor(MemberVisitor& memberVisitor): memberVisitor(memberVisitor)
		{
			setOwner(memberVisitor.getOwner());
		}

		void StatementVisitor::visitThenOrElse(ir_node* thenOrElseBlock, shptr<const ast::stmt::Statement> thenOrElseStmt, ir_node* exitBlock)
		{
			mature_immBlock(thenOrElseBlock);
			set_cur_block(thenOrElseBlock);

			thenOrElseStmt->accept(*this);

			if (get_cur_block())
				add_immBlock_pred(exitBlock, new_Jmp());
		}

		void StatementVisitor::visit(shptr<const ast::stmt::IfStatement> ifStatement)
		{
			ir_node* thenBlock = new_immBlock();
			ir_node* elseBlock = new_immBlock();
			ir_node* exitBlock = new_immBlock();

			BoolExpressionVisitor condVisitor(thenBlock, elseBlock);

			ifStatement->getCondition()->accept(condVisitor);

			unsigned int returns = 0;

			if (ifStatement->getThenStatement())
			{
				visitThenOrElse(thenBlock, ifStatement->getThenStatement(), exitBlock);

				if (!get_cur_block())
					returns++;
			}
			else
			{
				mature_immBlock(thenBlock);
				add_immBlock_pred(exitBlock, new_r_Jmp(thenBlock));
			}

			if (ifStatement->getElseStatement())
			{
				visitThenOrElse(elseBlock, ifStatement->getElseStatement(), exitBlock);

				if (!get_cur_block())
					returns++;
			}
			else
			{
				mature_immBlock(elseBlock);
				add_immBlock_pred(exitBlock, new_r_Jmp(elseBlock));
			}

			if (returns < 2)
			{
				mature_immBlock(exitBlock);
				set_cur_block(exitBlock);
			}

			// else both then and else statement returned: leave current block as NULL, no need to touch exitBlock
		}

		void StatementVisitor::visit(shptr<const ast::stmt::WhileStatement> whileStmt)
		{
			ir_node* whileCondBlock = new_immBlock();
			ir_node* whileBodyBlock = new_immBlock();
			ir_node* exitBlock = new_immBlock();

			// this is necessary for correctly handling infinite loops
			keep_alive(whileCondBlock);

			add_immBlock_pred(whileCondBlock, new_Jmp());

			// create while condition
			BoolExpressionVisitor condVisitor(whileBodyBlock, exitBlock);
			set_cur_block(whileCondBlock);
			whileStmt->getCondition()->accept(condVisitor);

			// prevent "No memory chain found in graph" Firm errors
			get_store();

			mature_immBlock(whileBodyBlock);

			if (whileStmt->getLoopStatement())
			{
				// create while body
				set_cur_block(whileBodyBlock);
				whileStmt->getLoopStatement()->accept(*this);

				// append while body to while condition
				if (get_cur_block())
					add_immBlock_pred(whileCondBlock, new_Jmp());
			}
			else
				add_immBlock_pred(whileCondBlock, new_r_Jmp(whileBodyBlock));

			mature_immBlock(whileCondBlock);

			// finalize
			set_cur_block(exitBlock);
			mature_immBlock(exitBlock);
		}

		void StatementVisitor::visit(shptr<const ast::stmt::ReturnStatement> returnStmt)
		{
			ir_node* ret;

			if (returnStmt->getExpression())
			{
				// "return EXPRESSION;"
				ExpressionVisitor expr_visitor;
				returnStmt->getExpression()->accept(expr_visitor);
				ir_node* currentMemState = get_store();
				ir_node* ret_expr = expr_visitor.getResultNode();

				ret = new_Return(currentMemState, 1, &ret_expr);
			}
			else
			{
				// "return;"
				ir_node* currentMemState = get_store();
				ret = new_Return(currentMemState, 0, NULL);
			}

			ir_graph* irg = get_current_ir_graph();
			add_immBlock_pred(get_irg_end_block(irg), ret);

			set_cur_block(NULL);
		}

		void StatementVisitor::visit(shptr<const ast::stmt::Block> blockStmt)
		{
			// iterate over statements in the block and convert them to firm
			// the statements are responsible to attach themselves to the current block
			auto stmts = blockStmt->getStatements();

			for (auto& stmt : *stmts)
			{
				stmt->accept(*this);

				if (!get_cur_block())
					break;
			}
		}

		void StatementVisitor::visit(shptr<const ast::stmt::ExpressionStatement> exprStmt)
		{
			// an ExpressionStatement is just an expression that can stand alone, so visit the expression node
			ExpressionVisitor expr_visitor;
			exprStmt->getExpression()->accept(expr_visitor);
		}

		void StatementVisitor::visit(shptr<const ast::stmt::LVDStatement> lvdStmt)
		{
			// get the variable position from the map
			// evaluate the expression determining the value, if present-
			int v = (*FirmInterface::getInstance().getVarMap())[lvdStmt->getIdent()->getName()];

			if (lvdStmt->getInitialization())
			{
				// found variable initialization
				ExpressionVisitor expr_visitor;
				lvdStmt->getInitialization()->accept(expr_visitor);
				set_value(v, expr_visitor.getResultNode());
			}
			else
			{
				// Variable declaration statement without initialization
				ir_node* null = new_Const_long(FirmInterface::getInstance().getMode(lvdStmt->getDeclType()), 0);
				set_value(v, null);
			}
		}
	}
}
