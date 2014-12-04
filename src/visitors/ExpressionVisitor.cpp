#include "ExpressionVisitor.hpp"

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
}

// primary expressions
void ExpressionVisitor::visit(shptr<ast::pe::Bool const> boolExpr)
{
	bool value = boolExpr->getValue();
	this->resultNode = FirmInterface::getInstance().createNodeForBooleanConstant(value);
}
void ExpressionVisitor::visit(shptr<ast::pe::Ident const> identExpr)
{
	;
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
	;
}
void ExpressionVisitor::visit(shptr<ast::pe::NewObjectExpression const> newObjectExpr)
{
	;
}
void ExpressionVisitor::visit(shptr<ast::pe::Object const> objectExpr)
{
	;
}

// unary expressions
void ExpressionVisitor::visit(shptr<ast::ue::Neg const> negExpr)
{
	shptr<ast::Expression> child = negExpr->getChild();
	child->accept(*this);

	if (negExpr->getSize() % 2 == 1)
	{
		ir_node* left = this->resultNode;
		ir_node* right = FirmInterface::getInstance().createNodeForIntegerConstant(-1);
		this->resultNode = new_Mul(left, right, FirmInterface::getInstance().getIntegerMode());
	}
}

void ExpressionVisitor::visit(shptr<ast::ue::Not const> notExpr)
{
	shptr<ast::Expression> child = notExpr->getChild();
	child->accept(*this);

	if (notExpr->getSize() == 1)
	{
		ir_node* left = FirmInterface::getInstance().createNodeForBooleanConstant(true);
		ir_node* right = this->resultNode;
		this->resultNode = new_Sub(left, right, FirmInterface::getInstance().getBooleanMode());
	}
}

// binary expressions
void ExpressionVisitor::visit(shptr<ast::be::Eq const> eqExpr)
{

	std::string lhs_ident = NULL;
	ir_node* rhs = NULL; //TODO: get rhs variable of eqExpr
	ast::MethodDeclaration* currentMethodDeclaration = NULL;

	int lhs_pos = (*currentMethodDeclaration->setVariablePositions())[lhs_ident];

	set_value(lhs_pos, rhs);

	this->resultNode = rhs; //return rhs or lhs?
}

void ExpressionVisitor::visit(shptr<ast::be::AndAnd const> andAndExpr)
{
	;
}

void ExpressionVisitor::visit(shptr<ast::be::OrOr const> orOrExpr)
{
	;
}

void ExpressionVisitor::visit(shptr<ast::be::EqEq const> eqEqExpr)
{
	visitRelationalExpression(eqEqExpr, ir_relation::ir_relation_equal);
}

void ExpressionVisitor::visit(shptr<ast::be::NotEq const> notEqExpr)
{
	//see documentation for libFirm: ir_releation_less_greater for integers is "not equal".
	visitRelationalExpression(notEqExpr, ir_relation::ir_relation_less_greater);
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
	;
}
