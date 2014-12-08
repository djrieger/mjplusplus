#include "StatementVisitor.hpp"
#include "BoolExpressionVisitor.hpp"
#include "ExpressionVisitor.hpp"

StatementVisitor::StatementVisitor(MemberVisitor& memberVisitor): memberVisitor(memberVisitor)
{
	setOwner(memberVisitor.getOwner());
}

void StatementVisitor::visitThenOrElse(ir_node* thenOrElseBlock, shptr<const ast::Statement> thenOrElseStmt, ir_node* exitBlock)
{
	mature_immBlock(thenOrElseBlock);
	set_cur_block(thenOrElseBlock);

	thenOrElseStmt->accept(*this);

	if (get_cur_block())
		add_immBlock_pred(exitBlock, new_Jmp());
}

void StatementVisitor::visit(shptr<const ast::IfStatement> ifStatement)
{
	ir_node* thenBlock = new_immBlock();
	ir_node* elseBlock = new_immBlock();
	ir_node* exitBlock = new_immBlock();

	BoolExpressionVisitor condVisitor(ifStatement->getThenStatement() ? thenBlock : exitBlock,
	                                  ifStatement->getElseStatement() ? elseBlock : exitBlock
	                                 );

	ifStatement->getCondition()->accept(condVisitor);

	unsigned int returns = 0;

	if (ifStatement->getThenStatement())
	{
		visitThenOrElse(thenBlock, ifStatement->getThenStatement(), exitBlock);

		if (!get_cur_block())
			returns++;
	}

	if (ifStatement->getElseStatement())
	{
		visitThenOrElse(elseBlock, ifStatement->getElseStatement(), exitBlock);

		if (!get_cur_block())
			returns++;
	}

	if (returns == 2)
	{
		std::cout << "both branches return" << std::endl;
		//I could remove the useless exitBlock if I knew how
		//looks like libFirm doesn't care
	}
	else
	{
		mature_immBlock(exitBlock);
		set_cur_block(exitBlock);
	}
}

void StatementVisitor::visit(shptr<const ast::WhileStatement> whileStmt)
{
	ir_node* whileCondBlock = new_immBlock();
	ir_node* whileBodyBlock = NULL;

	if (whileStmt->getLoopStatement())
		whileBodyBlock = new_immBlock();

	ir_node* exitBlock = new_immBlock();

	// this is necessary for correctly handling infinite loops
	keep_alive(whileCondBlock);

	add_immBlock_pred(whileCondBlock, new_Jmp());

	// create while condition
	BoolExpressionVisitor condVisitor(whileStmt->getLoopStatement() ? whileBodyBlock : whileCondBlock, exitBlock);
	set_cur_block(whileCondBlock);
	whileStmt->getCondition()->accept(condVisitor);

	get_store();

	if (whileStmt->getLoopStatement())
	{
		mature_immBlock(whileBodyBlock);

		// create while body
		set_cur_block(whileBodyBlock);
		whileStmt->getLoopStatement()->accept(*this);

		// append while body to while condition
		if (get_cur_block())
			add_immBlock_pred(whileCondBlock, new_Jmp());
	}

	mature_immBlock(whileCondBlock);

	// finalize
	set_cur_block(exitBlock);
	mature_immBlock(exitBlock);
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
	{
		stmt->accept(*this);

		if (!get_cur_block())
			break;
	}

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
