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
	auto trueTarget = std::make_shared<JumpTarget>();
	auto falseTarget = std::make_shared<JumpTarget>();
	auto exitTarget = std::make_shared<JumpTarget>();
	ExpressionVisitor condVisitor(ifStatement->getThenStatement() ? trueTarget : exitTarget,
	                              ifStatement->getElseStatement() ? falseTarget : exitTarget
	                             );
	ifStatement->getCondition()->accept(condVisitor); // TODO: Implement accept(ExpressionVisitor) for Expression subclasses


	if (ifStatement->getThenStatement())
	{
		set_cur_block(trueTarget->targetNode);
		ifStatement->getThenStatement()->accept(*this); // TODO: Implement accept(StatementVisitor) for Statement subclasses
		ir_node* trueNode = getResultNode();
		exitTarget->jumpFromBlock(trueNode);
	}

	if (ifStatement->getElseStatement())
	{
		set_cur_block(falseTarget->targetNode);
		ifStatement->getElseStatement()->accept(*this); // TODO: Implement accept(StatementVisitor) for Statement subclasses
		ir_node* falseNode = getResultNode();
		exitTarget->jumpFromBlock(falseNode);
	}

	set_cur_block(exitTarget->targetNode);
	this->resultNode = exitTarget->targetNode;
}

void StatementVisitor::visit(shptr<const ast::WhileStatement> whileStmt)
{
	auto headTarget = std::make_shared<JumpTarget>();
	auto loopTarget = std::make_shared<JumpTarget>();
	auto exitTarget = std::make_shared<JumpTarget>();

	set_cur_block(headTarget->targetNode);

	ExpressionVisitor condVisitor(whileStmt->getLoopStatement() ? loopTarget : headTarget, exitTarget);
	whileStmt->getCondition()->accept(condVisitor); // TODO: Implement accept(ExpressionVisitor) for Expression subclasses

	if (whileStmt->getLoopStatement())
	{
		set_cur_block(loopTarget->targetNode);
		whileStmt->getLoopStatement()->accept(*this); // TODO: Implement accept(StatementVisitor) for Statement subclasses
		ir_node* loopNode = getResultNode();
		headTarget->jumpFromBlock(loopNode);
	}
	else
		headTarget->jumpFromBlock(headTarget->targetNode);

	set_cur_block(exitTarget->targetNode);
	this->resultNode = exitTarget->targetNode;
}

void StatementVisitor::visit(shptr<const ast::ReturnStatement> returnStmt)
{
	ir_node* ret;

	if (returnStmt->getExpression())
	{
		ExpressionVisitor expr_visitor;
		returnStmt->getExpression()->accept(expr_visitor);
		ir_node* currentMemState = get_store();
		ir_node* ret_expr = expr_visitor.getResultNode();
		ret = new_Return(currentMemState, 1, &ret_expr);
	}
	else
	{
		ir_node* currentMemState = get_store();
		ret = new_Return(currentMemState, 0, NULL);
	}

	ir_graph* irg = get_current_ir_graph();
	add_immBlock_pred(get_irg_end_block(irg), ret);
}

void StatementVisitor::visit(shptr<const ast::Block> blockStmt)
{

	// save old block; is this even necessary? -- i don't think so
	//auto oldBlock = get_cur_block();

	// create new Block and set it as current; is this even necessary?
	//ir_node* block = new_r_immBlock(get_current_ir_graph());
	//set_cur_block(block);
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
	int v = (*FirmInterface::getInstance().getVarMap())[lvdStmt->getIdent()->getName()];

	if (lvdStmt->getInitialization())
	{
		ExpressionVisitor expr_visitor;
		lvdStmt->getInitialization()->accept(expr_visitor);
		set_value(v, expr_visitor.getResultNode());
	}
}
