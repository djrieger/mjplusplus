#include "StatementVisitor.hpp"

void StatementVisitor::visit(shptr<const ast::IfStatement> ifStatement)
{
	JumpTarget trueTarget, falseTarget;
	//ifStatement->getCondition()->visit(&trueTarget, &falseTarget);
	/*
	JumpTarget exitTarget;
	set_cur_block(trueTarget.targetNode);
	trueStatement->visit();
	ir_node *trueNode = ifStatement->getThenStatement()->getResultNode();
	exitTarget.jumpFromBlock(trueNode);

	set_cur_block(falseTarget.targetNode);
	if (ifStatement->getElseStatement())
	{
		elseStatement->visit();
		ir_node *falseNode = elseStatement->getResultNode();
	}
	exitTarget.jumpFromBlock(falseNode);

	set_cur_block(exitTarget.targetNode);
	*/
}

void StatementVisitor::visit(shptr<const ast::WhileStatement> whileStmt)
{

}

void StatementVisitor::visit(shptr<const ast::ReturnStatement> returnStmt)
{

}
