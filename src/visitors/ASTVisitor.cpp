#include <iostream>

#include "ASTVisitor.hpp"

#include "../ast/Node.hpp"
#include "../ast/Arguments.hpp"
#include "../ast/ArrayAccess.hpp"
#include "../ast/BinaryExpression.hpp"
#include "../ast/Block.hpp"
#include "../ast/ClassDeclaration.hpp"
#include "../ast/ClassMember.hpp"
#include "../ast/Expression.hpp"
#include "../ast/ExpressionStatement.hpp"
#include "../ast/FieldAccess.hpp"
#include "../ast/FieldDeclaration.hpp"
#include "../ast/Ident.hpp"
#include "../ast/IfStatement.hpp"
#include "../ast/LVDStatement.hpp"
#include "../ast/MainMethodDeclaration.hpp"
#include "../ast/MethodDeclaration.hpp"
#include "../ast/MethodInvocation.hpp"
#include "../ast/PostfixExpression.hpp"
#include "../ast/PostfixOp.hpp"
#include "../ast/PrimaryExpression.hpp"
#include "../ast/Program.hpp"
#include "../ast/ReturnStatement.hpp"
#include "../ast/Statement.hpp"
#include "../ast/Type.hpp"
#include "../ast/TypeIdent.hpp"
#include "../ast/UnaryExpression.hpp"
#include "../ast/WhileStatement.hpp"


void ASTVisitor::visit(shptr<ast::Node const>)
{
	std::cerr << "This Visitor does not accept ast::Node" << std::endl;
	std::cerr << "generating segfault for easy backtrace" << std::endl;
	int* i = nullptr;
	(*i)++;
	throw "This Visitor does not accept ast::Node";
}

void ASTVisitor::visit(shptr<ast::Arguments const> node)
{
	visit(std::static_pointer_cast<ast::Node const>(node));
}

void ASTVisitor::visit(shptr<ast::ArrayAccess const> node)
{
	visit(std::static_pointer_cast<ast::PostfixOp const>(node));
}

void ASTVisitor::visit(shptr<ast::be::BinaryExpression const> node)
{
	visit(std::static_pointer_cast<ast::Expression const>(node));
}

void ASTVisitor::visit(shptr<ast::be::AndAnd const> node)
{
	visit(std::static_pointer_cast<ast::be::BinaryExpression const>(node));
}

void ASTVisitor::visit(shptr<ast::be::Eq const> node)
{
	visit(std::static_pointer_cast<ast::be::BinaryExpression const>(node));
}

void ASTVisitor::visit(shptr<ast::be::EqEq const> node)
{
	visit(std::static_pointer_cast<ast::be::BinaryExpression const>(node));
}

void ASTVisitor::visit(shptr<ast::be::GreaterThan const> node)
{
	visit(std::static_pointer_cast<ast::be::BinaryExpression const>(node));
}

void ASTVisitor::visit(shptr<ast::be::GreaterThanEq const> node)
{
	visit(std::static_pointer_cast<ast::be::BinaryExpression const>(node));
}

void ASTVisitor::visit(shptr<ast::be::Invalid const> node)
{
	visit(std::static_pointer_cast<ast::be::BinaryExpression const>(node));
}

void ASTVisitor::visit(shptr<ast::be::LessThan const> node)
{
	visit(std::static_pointer_cast<ast::be::BinaryExpression const>(node));
}

void ASTVisitor::visit(shptr<ast::be::LessThanEq const> node)
{
	visit(std::static_pointer_cast<ast::be::BinaryExpression const>(node));
}

void ASTVisitor::visit(shptr<ast::be::Minus const> node)
{
	visit(std::static_pointer_cast<ast::be::BinaryExpression const>(node));
}

void ASTVisitor::visit(shptr<ast::be::Mod const> node)
{
	visit(std::static_pointer_cast<ast::be::BinaryExpression const>(node));
}

void ASTVisitor::visit(shptr<ast::be::Mult const> node)
{
	visit(std::static_pointer_cast<ast::be::BinaryExpression const>(node));
}

void ASTVisitor::visit(shptr<ast::be::NotEq const> node)
{
	visit(std::static_pointer_cast<ast::be::BinaryExpression const>(node));
}

void ASTVisitor::visit(shptr<ast::be::OrOr const> node)
{
	visit(std::static_pointer_cast<ast::be::BinaryExpression const>(node));
}

void ASTVisitor::visit(shptr<ast::be::Plus const> node)
{
	visit(std::static_pointer_cast<ast::be::BinaryExpression const>(node));
}

void ASTVisitor::visit(shptr<ast::be::Slash const> node)
{
	visit(std::static_pointer_cast<ast::be::BinaryExpression const>(node));
}

void ASTVisitor::visit(shptr<ast::Block const> node)
{
	visit(std::static_pointer_cast<ast::Statement const>(node));
}

void ASTVisitor::visit(shptr<ast::ClassDeclaration const> node)
{
	visit(std::static_pointer_cast<ast::Node const>(node));
}
void ASTVisitor::visit(shptr<ast::ClassMember const> node)
{
	visit(std::static_pointer_cast<ast::Node const>(node));
}

void ASTVisitor::visit(shptr<ast::Expression const> node)
{
	visit(std::static_pointer_cast<ast::Node const>(node));
}

void ASTVisitor::visit(shptr<ast::ExpressionStatement const> node)
{
	visit(std::static_pointer_cast<ast::Statement const>(node));
}

void ASTVisitor::visit(shptr<ast::FieldAccess const> node)
{
	visit(std::static_pointer_cast<ast::PostfixOp const>(node));
}

void ASTVisitor::visit(shptr<ast::FieldDeclaration const> node)
{
	visit(std::static_pointer_cast<ast::ClassMember const>(node));
}

void ASTVisitor::visit(shptr<ast::Ident const> node)
{
	visit(std::static_pointer_cast<ast::Node const>(node));
}

void ASTVisitor::visit(shptr<ast::IfStatement const> node)
{
	visit(std::static_pointer_cast<ast::Statement const>(node));
}

void ASTVisitor::visit(shptr<ast::LVDStatement const> node)
{
	visit(std::static_pointer_cast<ast::Statement const>(node));
}

void ASTVisitor::visit(shptr<ast::MainMethodDeclaration const> node)
{
	visit(std::static_pointer_cast<ast::MethodDeclaration const>(node));
}

void ASTVisitor::visit(shptr<ast::MethodDeclaration const> node)
{
	visit(std::static_pointer_cast<ast::ClassMember const>(node));
}

void ASTVisitor::visit(shptr<ast::MethodInvocation const> node)
{
	visit(std::static_pointer_cast<ast::PostfixOp const>(node));
}

void ASTVisitor::visit(shptr<ast::PostfixExpression const> node)
{
	visit(std::static_pointer_cast<ast::Expression const>(node));
}

void ASTVisitor::visit(shptr<ast::pe::PrimaryExpression const> node)
{
	visit(std::static_pointer_cast<ast::Expression const>(node));
}

void ASTVisitor::visit(shptr<ast::pe::Bool const> node)
{
	visit(std::static_pointer_cast<ast::pe::PrimaryExpression const>(node));
}

void ASTVisitor::visit(shptr<ast::pe::Ident const> node)
{
	visit(std::static_pointer_cast<ast::pe::PrimaryExpression const>(node));
}

void ASTVisitor::visit(shptr<ast::pe::Integer const> node)
{
	visit(std::static_pointer_cast<ast::pe::PrimaryExpression const>(node));
}

void ASTVisitor::visit(shptr<ast::pe::MethodInvocation const> node)
{
	visit(std::static_pointer_cast<ast::pe::PrimaryExpression const>(node));
}

void ASTVisitor::visit(shptr<ast::pe::NewArrayExpression const> node)
{
	visit(std::static_pointer_cast<ast::pe::PrimaryExpression const>(node));
}

void ASTVisitor::visit(shptr<ast::pe::NewObjectExpression const> node)
{
	visit(std::static_pointer_cast<ast::pe::PrimaryExpression const>(node));
}

void ASTVisitor::visit(shptr<ast::pe::Object const> node)
{
	visit(std::static_pointer_cast<ast::pe::PrimaryExpression const>(node));
}

void ASTVisitor::visit(shptr<ast::PostfixOp const> node)
{
	visit(std::static_pointer_cast<ast::Node const>(node));
}

void ASTVisitor::visit(shptr<ast::Program const> node)
{
	visit(std::static_pointer_cast<ast::Node const>(node));
}

void ASTVisitor::visit(shptr<ast::ReturnStatement const> node)
{
	visit(std::static_pointer_cast<ast::Statement const>(node));
}

void ASTVisitor::visit(shptr<ast::Statement const> node)
{
	visit(std::static_pointer_cast<ast::Node const>(node));
}

void ASTVisitor::visit(shptr<ast::Type const> node)
{
	visit(std::static_pointer_cast<ast::Node const>(node));
}

void ASTVisitor::visit(shptr<ast::TypeIdent const> node)
{
	visit(std::static_pointer_cast<ast::Node const>(node));
}

void ASTVisitor::visit(shptr<ast::ue::UnaryExpression const> node)
{
	visit(std::static_pointer_cast<ast::Expression const>(node));
}

void ASTVisitor::visit(shptr<ast::ue::Neg const> node)
{
	visit(std::static_pointer_cast<ast::ue::UnaryExpression const>(node));
}

void ASTVisitor::visit(shptr<ast::ue::Not const> node)
{
	visit(std::static_pointer_cast<ast::ue::UnaryExpression const>(node));
}

void ASTVisitor::visit(shptr<ast::WhileStatement const> node)
{
	visit(std::static_pointer_cast<ast::Statement const>(node));
}
