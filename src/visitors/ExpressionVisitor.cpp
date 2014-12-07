#include "ExpressionVisitor.hpp"
#include "PostfixOpsVisitor.hpp"
#include "VariableDeclVisitor.hpp"
#include <sstream>

ExpressionVisitor::ExpressionVisitor() : store_value(NULL), do_store(false) {}

ExpressionVisitor::ExpressionVisitor(shptr<JumpTarget> trueTarget, shptr<JumpTarget> falseTarget): store_value(NULL), do_store(false), trueTarget(trueTarget), falseTarget(falseTarget)
{
}

ir_type* ExpressionVisitor::getResultType() const
{
	return resultType;
}

void ExpressionVisitor::visitBinaryExpression(
    shptr<ast::be::BinaryExpression const> binExpr,
    std::function<ir_node* (ir_node*, ir_node*)> createResultNode)
{
	std::cout << "accepting be left child" << std::endl;
	binExpr->getLeftChild()->accept(*this);
	ir_node* left = this->resultNode;

	std::cout << "accepting be right child" << std::endl;
	binExpr->getRightChild()->accept(*this);
	ir_node* right = this->resultNode;

	this->resultNode = createResultNode(left, right);
}

void ExpressionVisitor::visitRelationalExpression(shptr<ast::be::BinaryExpression const> binaryExpression, ir_relation relation)
{
	std::cout << "visitRelationalExpression" << std::endl;
	visitBinaryExpression(binaryExpression, [relation] (ir_node * left, ir_node * right) -> ir_node *
	{
		return new_Cmp(left, right, relation);
	});
}

// primary expressions
void ExpressionVisitor::visit(shptr<ast::pe::Bool const> boolExpr)
{
	bool value = boolExpr->getValue();
	this->resultNode = FirmInterface::getInstance().createNodeForBooleanConstant(value);
}
void ExpressionVisitor::visit(shptr<ast::pe::Ident const> identExpr)
{
	// Param / Local Variable
	// Member
	// System

	auto decl = identExpr->getDeclaration();

	if (decl)
	{
		ir_node* current_this = get_value(0, mode_P);
		VariableDeclVisitor vdVisitor(current_this, (do_store && store_value) ? store_value : NULL);
		std::cout << "got declaration " << std::endl;
		decl->accept(vdVisitor);
		resultNode = vdVisitor.getResultNode();
		resultType = vdVisitor.getResultType();
	}
	else
	{
		std::cout << "got System " << std::endl;

		lexer::Token sit {lexer::Token::Token_type::TOKEN_IDENT, lexer::Token::getTableReference("$System"), { -1, 0}};
		auto si = std::make_shared<ast::Ident>(sit);
		auto s = std::make_shared<ast::Type>(si);
		resultType = FirmInterface::getInstance().getType(s);

		ir_entity* system_ent = FirmInterface::getInstance().getSystemNode();
		ir_node* system_addr = new_Address(system_ent);
		ir_node* load = new_Load(get_store(), system_addr, get_type_mode(resultType), resultType, cons_none);
		set_store(new_Proj(load, mode_M, pn_Load_M));
		resultNode = new_Proj(load, mode_P, pn_Load_res);
	}
}
void ExpressionVisitor::visit(shptr<ast::pe::Integer const> integerExpr)
{
	int64_t x = atoll(integerExpr->getStringValue().c_str());
	std::cout << "visiting integer with value " << x << std::endl;
	this->resultNode = FirmInterface::getInstance().createNodeForIntegerConstant(x);
}
void ExpressionVisitor::visit(shptr<ast::pe::MethodInvocation const> methodInvocationExpr)
{
	this->resultNode = FirmInterface::getInstance().createNodeForMethodCall(methodInvocationExpr);
}
void ExpressionVisitor::visit(shptr<ast::pe::NewArrayExpression const> newArrayExpr)
{
	newArrayExpr->getSize()->accept(*this);
	ir_type* t = FirmInterface::getInstance().getType(newArrayExpr->getType()->de_array());
	resultNode = FirmInterface::getInstance().createNodeForCallocCall(resultNode, get_type_size_bytes(t));
	resultType = FirmInterface::getInstance().getType(newArrayExpr->getType());
}

void ExpressionVisitor::visit(shptr<ast::pe::NewObjectExpression const> newObjectExpr)
{
	ir_node* one = FirmInterface::getInstance().createNodeForIntegerConstant(1);
	auto at = std::make_shared<ast::Type>(newObjectExpr->getIdent());
	resultType = FirmInterface::getInstance().getType(at);
	resultNode = FirmInterface::getInstance().createNodeForCallocCall(one, get_type_size_bytes(get_pointer_points_to_type(resultType)));
}

void ExpressionVisitor::visit(shptr<ast::pe::Object const> objectExpr)
{
	switch (objectExpr->getObjectType())
	{
		case ast::pe::Object::NULL_OBJECT:
		{
			this->resultNode = FirmInterface::getInstance().createNullPointerNode();
		}

		case ast::pe::Object::THIS_OBJECT:
		{
			this->resultNode = get_value(0, mode_P);
		}
	}
}

// unary expressions
void ExpressionVisitor::visit(shptr<ast::ue::Neg const> negExpr)
{
	shptr<ast::Expression> child = negExpr->getChild();
	child->accept(*this);

	if (negExpr->getSize() & 1)
		this->resultNode = new_Minus(this->resultNode, FirmInterface::getInstance().getIntegerMode());
}

void ExpressionVisitor::visit(shptr<ast::ue::Not const> notExpr)
{
	shptr<ast::Expression> child = notExpr->getChild();

	if (notExpr->getSize() & 1)
		trueTarget.swap(falseTarget);

	child->accept(*this);
}

// binary expressions
void ExpressionVisitor::visit(shptr<ast::be::Eq const> eqExpr)
{
	/* plan:
	 * evaluate rhs, save result node
	 * evaluate lhs, but replace final load by store
	 */
	std::cout << "got assignment" << std::endl;

	eqExpr->getRightChild()->accept(*this);
	store_value = resultNode;
	ir_node* rhs = resultNode;

	do_store = true;
	eqExpr->getLeftChild()->accept(*this);
	do_store = false;
	resultNode = rhs;
	std::cout << "assignment done" << std::endl;
}

void ExpressionVisitor::visit(shptr<ast::be::AndAnd const> andAndExpr)
{
	auto rightTarget = std::make_shared<JumpTarget>();
	ExpressionVisitor vleft(rightTarget, falseTarget);
	andAndExpr->getLeftChild()->accept(vleft);
	set_cur_block(rightTarget->targetNode);
	andAndExpr->getRightChild()->accept(*this);
}

void ExpressionVisitor::visit(shptr<ast::be::OrOr const> orOrExpr)
{
	auto rightTarget = std::make_shared<JumpTarget>();
	ExpressionVisitor vleft(trueTarget, rightTarget);
	orOrExpr->getLeftChild()->accept(vleft);
	set_cur_block(rightTarget->targetNode);
	orOrExpr->getRightChild()->accept(*this);
}

void ExpressionVisitor::visit(shptr<ast::be::EqEq const> eqEqExpr)
{
	std::cout << "visiting EqEq" << std::endl;
	visitRelationalExpression(eqEqExpr, ir_relation::ir_relation_equal);
}

void ExpressionVisitor::visit(shptr<ast::be::NotEq const> notEqExpr)
{
	//see documentation for libFirm: ir_releation_less_greater for integers is "not equal".
	visitRelationalExpression(notEqExpr, ir_relation::ir_relation_unordered_greater_equal);
}

void ExpressionVisitor::visit(shptr<ast::be::GreaterThan const> greaterThanExpr)
{
	visitRelationalExpression(greaterThanExpr, ir_relation::ir_relation_greater);
}

void ExpressionVisitor::visit(shptr<ast::be::GreaterThanEq const> greaterThanEqExpr)
{
	visitRelationalExpression(greaterThanEqExpr, ir_relation::ir_relation_greater_equal);
}

void ExpressionVisitor::visit(shptr<ast::be::LessThan const> lessThanExpr)
{
	visitRelationalExpression(lessThanExpr, ir_relation::ir_relation_less);
}

void ExpressionVisitor::visit(shptr<ast::be::LessThanEq const> lessThanEqExpr)
{
	visitRelationalExpression(lessThanEqExpr, ir_relation::ir_relation_less_equal);
}

void ExpressionVisitor::visit(shptr<ast::be::Plus const> plusExpr)
{
	visitBinaryExpression(plusExpr, [] (ir_node * left, ir_node * right) -> ir_node*
	{
		return new_Add(left, right, FirmInterface::getInstance().getIntegerMode());
	});
}

void ExpressionVisitor::visit(shptr<ast::be::Minus const> minusExpr)
{
	visitBinaryExpression(minusExpr, [] (ir_node * left, ir_node * right) -> ir_node*
	{
		return new_Sub(left, right, FirmInterface::getInstance().getIntegerMode());
	});
}

void ExpressionVisitor::visit(shptr<ast::be::Mod const> modExpr)
{
	visitBinaryExpression(modExpr, [] (ir_node * left, ir_node * right) -> ir_node*
	{
		return FirmInterface::getInstance().createModOperation(left, right);
	});
}

void ExpressionVisitor::visit(shptr<ast::be::Mult const> multExpr)
{
	visitBinaryExpression(multExpr, [] (ir_node * left, ir_node * right) -> ir_node*
	{
		return new_Mul(left, right, FirmInterface::getInstance().getIntegerMode());
	});
}

void ExpressionVisitor::visit(shptr<ast::be::Slash const> slashExpr)
{
	visitBinaryExpression(slashExpr, [] (ir_node * left, ir_node * right) -> ir_node*
	{
		return FirmInterface::getInstance().createDivOperation(left, right);
	});
}

void ExpressionVisitor::visit(shptr<ast::be::Invalid const>)
{
	std::cerr << "ExpressionVisitor visited Invalid: in" << __FILE__
	          << " at " << __LINE__ << std::endl;
}

// postfix expression
void ExpressionVisitor::visit(shptr<ast::PostfixExpression const> postfixExpression)
{
	std::cout << "Visiting PostfixExpression" << std::endl;
	bool old_do_store = do_store;
	//only the last PostfixOp does the store - or the PrimaryExpression if there are no PostfixOps
	do_store &= !postfixExpression->getPostfixOps()->empty();

	std::cout << "ev visit pe" << std::endl;
	postfixExpression->getChild()->accept(*this);
	auto pops = postfixExpression->getPostfixOps();
	std::cout << "ev visit pe done" << std::endl;

	if (!pops->empty())
	{
		PostfixOpsVisitor popsVisitor(*this);

		for (auto it = pops->begin(); it != pops->end() - 1; it++)
		{
			std::cout << "ev/popsv visit pop" << std::endl;
			(*it)->accept(popsVisitor);
			resultNode = popsVisitor.getResultNode();
			resultType = popsVisitor.getResultType();
			std::cout << "ev/popsv visit pop done" << std::endl;
		}

		//last PostfixOp may store
		do_store = old_do_store;

		if (do_store)
			popsVisitor.setStoreValue(store_value);

		std::cout << "ev/popsv visit LAST pop" << std::endl;
		pops->back()->accept(popsVisitor);
		resultNode = popsVisitor.getResultNode();
		resultType = popsVisitor.getResultType();
		std::cout << "ev/popsv visit LAST pop done" << std::endl;
	}
}
