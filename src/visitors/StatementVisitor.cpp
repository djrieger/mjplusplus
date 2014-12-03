#include "StatementVisitor.hpp"
#include "ExpressionVisitor.hpp"

ir_node* StatementVisitor::getResultNode() const
{
	return this->resultNode;
}

void StatementVisitor::visit(shptr<const ast::IfStatement> ifStatement)
{
	shptr<JumpTarget> trueTarget;
	shptr<JumpTarget> falseTarget;
	ExpressionVisitor condVisitor(trueTarget, falseTarget);
	ifStatement->getCondition()->accept(condVisitor); // TODO: Implement accept(ExpressionVisitor) for Expression subclasses
	
	JumpTarget exitTarget;
	set_cur_block(trueTarget->targetNode);

	ifStatement->getThenStatement()->accept(*this); // TODO: Implement accept(StatementVisitor) for Statement subclasses
	ir_node *trueNode = getResultNode();
	exitTarget.jumpFromBlock(trueNode);

	set_cur_block(falseTarget->targetNode);
	if (ifStatement->getElseStatement())
	{
		ifStatement->getElseStatement()->accept(*this); // TODO: Implement accept(StatementVisitor) for Statement subclasses
		ir_node *falseNode = getResultNode();
		exitTarget.jumpFromBlock(falseNode);
	}
	
	set_cur_block(exitTarget.targetNode);
	
	this->resultNode = exitTarget.targetNode;
}

void StatementVisitor::visit(shptr<const ast::WhileStatement> whileStmt)
{

}

void StatementVisitor::visit(shptr<const ast::ReturnStatement> returnStmt)
{

}

void StatementVisitor::visit(shptr<const ast::Block> blockStmt) {
	;
}

void StatementVisitor::visit(shptr<const ast::ExpressionStatement> exprStmt) {
	;
}

void StatementVisitor::visit(shptr<const ast::LVDStatement> lvdStmt) {
	;
}