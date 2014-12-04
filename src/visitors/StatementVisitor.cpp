#include "StatementVisitor.hpp"
#include "ExpressionVisitor.hpp"

StatementVisitor::StatementVisitor(MemberVisitor& memberVisitor): memberVisitor(memberVisitor)
{
	setOwner(memberVisitor.getOwner());
}

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
	ir_node* trueNode = getResultNode();
	exitTarget.jumpFromBlock(trueNode);

	set_cur_block(falseTarget->targetNode);

	if (ifStatement->getElseStatement())
	{
		ifStatement->getElseStatement()->accept(*this); // TODO: Implement accept(StatementVisitor) for Statement subclasses
		ir_node* falseNode = getResultNode();
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

void StatementVisitor::visit(shptr<const ast::Block> blockStmt)
{

	// save old block; is this even necessary?
	//auto oldBlock = get_cur_block();

	// create new Block and set it as current; is this even necessary?
	ir_node* block = new_r_immBlock(get_current_ir_graph());
	set_cur_block(block);
	// iterate over statements in the block and convert them to firm
	// the statements are responsible to attach themselves to the current block
	auto stmts = blockStmt->getStatements();

	for (auto& stmt : *stmts)
		stmt->accept(*this);

	//restore old block; is this even necessary?
	//set_cur_block(oldBlock);
}

void StatementVisitor::visit(shptr<const ast::ExpressionStatement> exprStmt)
{
	// an ExpressionStatement is just an expression, that can stand alone, so visit the expression node
	ExpressionVisitor expr_visitor;
	exprStmt->getExpression()->accept(expr_visitor);
}

void StatementVisitor::visit(shptr<const ast::LVDStatement> lvdStmt)
{
	// get the variable position from the map
	// evaluate the expression determining the value, if present-
}