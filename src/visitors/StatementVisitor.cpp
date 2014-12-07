#include "StatementVisitor.hpp"
#include "ExpressionVisitor.hpp"

StatementVisitor::StatementVisitor(MemberVisitor& memberVisitor): memberVisitor(memberVisitor)
{
	setOwner(memberVisitor.getOwner());
}

void StatementVisitor::visitThenOrElse(shptr<const ast::Statement> thenOrElseStmt, ir_node* precedingProjection, ir_node* exitBlock)
{
	ir_node* thenOrElseBlock = new_immBlock();
	add_immBlock_pred(thenOrElseBlock, precedingProjection);
	mature_immBlock(thenOrElseBlock);
	set_cur_block(thenOrElseBlock);

	thenOrElseStmt->accept(*this);

	add_immBlock_pred(exitBlock, new_Jmp());
}

void StatementVisitor::visit(shptr<const ast::IfStatement> ifStatement)
{
	auto trueTarget = std::make_shared<JumpTarget>();
	auto falseTarget = std::make_shared<JumpTarget>();
	auto exitTarget = std::make_shared<JumpTarget>();
	ExpressionVisitor condVisitor(ifStatement->getThenStatement() ? trueTarget : exitTarget,
	                              ifStatement->getElseStatement() ? falseTarget : exitTarget
	                             );

	ifStatement->getCondition()->accept(condVisitor);
	ir_node* compareNode = condVisitor.getResultNode();

	ir_node* cond = new_Cond(compareNode);
	ir_node* projTrue = new_Proj(cond, get_modeX(), pn_Cond_true);
	ir_node* projFalse = new_Proj(cond, get_modeX(), pn_Cond_false);

	ir_node* exitBlock = new_immBlock();

	if (ifStatement->getThenStatement())
		visitThenOrElse(ifStatement->getThenStatement(), projTrue, exitBlock);

	if (ifStatement->getElseStatement())
		visitThenOrElse(ifStatement->getElseStatement(), projFalse, exitBlock);

	mature_immBlock(exitBlock);
	set_cur_block(exitBlock);
	this->resultNode = exitBlock;
}

void StatementVisitor::visit(shptr<const ast::WhileStatement> whileStmt)
{
	std::cout << "visiting while stmt" << std::endl;
	auto headTarget = std::make_shared<JumpTarget>();
	auto loopTarget = std::make_shared<JumpTarget>();
	auto exitTarget = std::make_shared<JumpTarget>();

	// before visiting the condition expression, we want to create and set a new imm_block
	// as we want the condition to be in its own block as we want to jump there after the loopStatement
	ExpressionVisitor condVisitor(whileStmt->getLoopStatement() ? loopTarget : headTarget, exitTarget);
	whileStmt->getCondition()->accept(condVisitor); // TODO: Implement accept(ExpressionVisitor) for Expression subclasses
	ir_node* compareNode = condVisitor.getResultNode();

	ir_node* cond = new_Cond(compareNode);
	ir_node* projTrue = new_Proj(cond, get_modeX(), pn_Cond_true);
	ir_node* projFalse = new_Proj(cond, get_modeX(), pn_Cond_false);

	ir_node* exitBlock = new_immBlock();

	if (whileStmt->getLoopStatement())
	{
		// do not jump to the exitBlock, change this to be the block of loop condition
		visitThenOrElse(whileStmt->getLoopStatement(), projTrue, exitBlock);
	}
	else
	{
		// if the LoopStatement doesn't exist, we probably want to jump to the exit block?
	}

	// if the loop condition evaluates to false, we want to jump to the exitBlock directly
	add_immBlock_pred(exitBlock, projFalse);

	mature_immBlock(exitBlock);
	set_cur_block(exitBlock);
	this->resultNode = exitTarget->targetNode;
}

void StatementVisitor::visit(shptr<const ast::ReturnStatement> returnStmt)
{
	ir_node* ret;

	if (returnStmt->getExpression())
	{
		std::cout << "Visiting return stmt with expression" << std::endl;
		ExpressionVisitor expr_visitor;
		returnStmt->getExpression()->accept(expr_visitor);
		ir_node* currentMemState = get_store();
		ir_node* ret_expr = expr_visitor.getResultNode();

		ret = new_Return(currentMemState, 1, &ret_expr);
	}
	else
	{
		std::cout << "Visiting return stmt without expr" << std::endl;
		ir_node* currentMemState = get_store();
		ret = new_Return(currentMemState, 0, NULL);
	}

	ir_graph* irg = get_current_ir_graph();
	add_immBlock_pred(get_irg_end_block(irg), ret);

	set_cur_block(NULL);
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
	std::cout << "visiting ExpressionStatement" << std::endl;
	ExpressionVisitor expr_visitor;
	exprStmt->getExpression()->accept(expr_visitor);
}

void StatementVisitor::visit(shptr<const ast::LVDStatement> lvdStmt)
{
	// get the variable position from the map
	// evaluate the expression determining the value, if present-
	int v = (*FirmInterface::getInstance().getVarMap())[lvdStmt->getIdent()->getName()];
	std::cout << "visiting lvdStmt, name=" << lvdStmt->getIdent()->getName() << ", varNum=" << v << std::endl;

	if (lvdStmt->getInitialization())
	{
		std::cout << "found variable initialization" << std::endl;
		ExpressionVisitor expr_visitor;
		lvdStmt->getInitialization()->accept(expr_visitor);
		set_value(v, expr_visitor.getResultNode());
	}
	else
	{
		std::cout << "var decl without init" << std::endl;
		ir_node* null = new_Const_long(FirmInterface::getInstance().getMode(lvdStmt->getDeclType()), 0);
		set_value(v, null);
	}
}
