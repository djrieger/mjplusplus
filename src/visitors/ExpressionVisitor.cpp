#include "ExpressionVisitor.hpp"
#include "PostfixOpsVisitor.hpp"
#include "VariableDeclVisitor.hpp"
#include <sstream>

ExpressionVisitor::ExpressionVisitor() {}

ExpressionVisitor::ExpressionVisitor(shptr<JumpTarget> trueTarget, shptr<JumpTarget> falseTarget): trueTarget(trueTarget), falseTarget(falseTarget)
{}

ir_node* ExpressionVisitor::getResultNode() const
{
	return resultNode;
}

void ExpressionVisitor::visitBinaryExpression(
    shptr<ast::be::BinaryExpression const> binExpr,
    std::function<ir_node* (ir_node*, ir_node*)> createResultNode)
{
	binExpr->getLeftChild()->accept(*this);
	ir_node* left = this->resultNode;

	binExpr->getRightChild()->accept(*this);
	ir_node* right = this->resultNode;

	this->resultNode = createResultNode(left, right);
}

void ExpressionVisitor::visitRelationalExpression(shptr<ast::be::BinaryExpression const> binaryExpression, ir_relation relation)
{
	visitBinaryExpression(binaryExpression, [relation] (ir_node * left, ir_node * right) -> ir_node *
	{
		return new_Cmp(left, right, relation);
	});
	ir_node* cond = new_Cond(resultNode);
	ir_node* pt = new_Proj(cond, get_modeX(), COND_JMP_PRED_TRUE);
	trueTarget->jumpFromBlock(pt);
	ir_node* pf = new_Proj(cond, get_modeX(), COND_JMP_PRED_FALSE);
	falseTarget->jumpFromBlock(pf);
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
	VariableDeclVisitor vdVisitor(identExpr);
	auto decl = identExpr->getDeclaration();

	if (decl)
	{
		std::cout << "got declaration " << std::endl;
		decl->accept(vdVisitor);
		this->resultNode = vdVisitor.getResultNode();
	}
	else
	{
		std::cout << "got System " << std::endl;
		// TODO: Create System
		// FirmInterface::getInstance().createSystemNode()
	}
}
void ExpressionVisitor::visit(shptr<ast::pe::Integer const> integerExpr)
{
	int64_t x = atoll(integerExpr->getStringValue().c_str());
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
}

void ExpressionVisitor::visit(shptr<ast::pe::NewObjectExpression const> newObjectExpr)
{
	ir_node* one = FirmInterface::getInstance().createNodeForIntegerConstant(1);
	auto at = std::make_shared<ast::Type>(newObjectExpr->getIdent());
	ir_type* t = FirmInterface::getInstance().getType(at);
	resultNode = FirmInterface::getInstance().createNodeForCallocCall(one, get_type_size_bytes(get_pointer_points_to_type(t)));
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

	std::stringstream output;
	std::cout << "eqExpr.toString = " << eqExpr << " pointing to "; // << (*eqExpr);
	//eqExpr->toString(output, false);
	std::cout << std::endl;

	std::string lhs_ident; 
	eqExpr->getLeftChild()->accept(*this);
	std::cout << "1" << std::endl;

	eqExpr->getRightChild()->accept(*this); //TODO: get rhs variable of eqExpr
	ir_node* rhs = this->resultNode;
	std::cout << "2" << std::endl;

	auto lhsIdent = std::dynamic_pointer_cast<ast::pe::Ident>(eqExpr->getLeftChild());
	std::cout << "3" << std::endl;
	if (lhsIdent)
	{
		std::cout << "3a" << std::endl;
		// simple case such as i = 5, lhs is an ident aka local variable
		int lhs_pos = (*FirmInterface::getInstance().getVarMap())[lhsIdent->getIdentifier()];
		std::cout << "lhs_pos =" << lhs_pos << std::endl;
		set_value(lhs_pos, rhs);
	} else {
		std::cout << "3b" << std::endl;
		std::cout << "Complex left-hand side expressions not implemented yet" << std::endl;
	}
	//this->resultNode = rhs; see above
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

void ExpressionVisitor::visit(shptr<ast::be::Invalid const> invalidExpr)
{
	std::cerr << "ExpressionVisitor visited Invalid: in" << __FILE__
	          << " at " << __LINE__ << std::endl;
}

// postfix expression
void ExpressionVisitor::visit(shptr<ast::PostfixExpression const> postfixExpression)
{
	std::cout << "Visiting PostfixExpression" << std::endl;
	postfixExpression->getChild()->accept(*this);
	PostfixOpsVisitor popsVisitor(*this);

	for (auto& it : *postfixExpression->getPostfixOps())
	{
		it->accept(popsVisitor);
		this->resultNode = popsVisitor.getResultNode();
	}
}
