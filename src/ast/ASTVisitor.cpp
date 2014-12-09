#include <iostream>

#include "ASTVisitor.hpp"

#include "Node.hpp"
#include "Arguments.hpp"
#include "ArrayAccess.hpp"
#include "BinaryExpression.hpp"
#include "Block.hpp"
#include "ClassDeclaration.hpp"
#include "ClassMember.hpp"
#include "Expression.hpp"
#include "ExpressionStatement.hpp"
#include "FieldAccess.hpp"
#include "FieldDeclaration.hpp"
#include "Ident.hpp"
#include "IfStatement.hpp"
#include "LVDStatement.hpp"
#include "MainMethodDeclaration.hpp"
#include "MethodDeclaration.hpp"
#include "MethodInvocation.hpp"
#include "PostfixExpression.hpp"
#include "PostfixOp.hpp"
#include "PrimaryExpression.hpp"
#include "Program.hpp"
#include "ReturnStatement.hpp"
#include "Statement.hpp"
#include "Type.hpp"
#include "TypeIdent.hpp"
#include "UnaryExpression.hpp"
#include "WhileStatement.hpp"

namespace ast
{
	void ASTVisitor::visit(shptr<Node const>)
	{
		std::cerr << "This Visitor does not accept ast::Node" << std::endl;
		std::cerr << "generating segfault for easy backtrace" << std::endl;
		int* i = nullptr;
		(*i)++;
		throw "This Visitor does not accept Node";
	}

	void ASTVisitor::visit(shptr<Arguments const> node)
	{
		visit(std::static_pointer_cast<Node const>(node));
	}

	void ASTVisitor::visit(shptr<be::BinaryExpression const> node)
	{
		visit(std::static_pointer_cast<Expression const>(node));
	}

	void ASTVisitor::visit(shptr<be::AndAnd const> node)
	{
		visit(std::static_pointer_cast<be::BinaryExpression const>(node));
	}

	void ASTVisitor::visit(shptr<be::Eq const> node)
	{
		visit(std::static_pointer_cast<be::BinaryExpression const>(node));
	}

	void ASTVisitor::visit(shptr<be::EqEq const> node)
	{
		visit(std::static_pointer_cast<be::BinaryExpression const>(node));
	}

	void ASTVisitor::visit(shptr<be::GreaterThan const> node)
	{
		visit(std::static_pointer_cast<be::BinaryExpression const>(node));
	}

	void ASTVisitor::visit(shptr<be::GreaterThanEq const> node)
	{
		visit(std::static_pointer_cast<be::BinaryExpression const>(node));
	}

	void ASTVisitor::visit(shptr<be::Invalid const> node)
	{
		visit(std::static_pointer_cast<be::BinaryExpression const>(node));
	}

	void ASTVisitor::visit(shptr<be::LessThan const> node)
	{
		visit(std::static_pointer_cast<be::BinaryExpression const>(node));
	}

	void ASTVisitor::visit(shptr<be::LessThanEq const> node)
	{
		visit(std::static_pointer_cast<be::BinaryExpression const>(node));
	}

	void ASTVisitor::visit(shptr<be::Minus const> node)
	{
		visit(std::static_pointer_cast<be::BinaryExpression const>(node));
	}

	void ASTVisitor::visit(shptr<be::Mod const> node)
	{
		visit(std::static_pointer_cast<be::BinaryExpression const>(node));
	}

	void ASTVisitor::visit(shptr<be::Mult const> node)
	{
		visit(std::static_pointer_cast<be::BinaryExpression const>(node));
	}

	void ASTVisitor::visit(shptr<be::NotEq const> node)
	{
		visit(std::static_pointer_cast<be::BinaryExpression const>(node));
	}

	void ASTVisitor::visit(shptr<be::OrOr const> node)
	{
		visit(std::static_pointer_cast<be::BinaryExpression const>(node));
	}

	void ASTVisitor::visit(shptr<be::Plus const> node)
	{
		visit(std::static_pointer_cast<be::BinaryExpression const>(node));
	}

	void ASTVisitor::visit(shptr<be::Slash const> node)
	{
		visit(std::static_pointer_cast<be::BinaryExpression const>(node));
	}

	void ASTVisitor::visit(shptr<Block const> node)
	{
		visit(std::static_pointer_cast<Statement const>(node));
	}

	void ASTVisitor::visit(shptr<ClassDeclaration const> node)
	{
		visit(std::static_pointer_cast<Node const>(node));
	}

	void ASTVisitor::visit(shptr<ClassMember const> node)
	{
		visit(std::static_pointer_cast<Node const>(node));
	}

	void ASTVisitor::visit(shptr<Expression const> node)
	{
		visit(std::static_pointer_cast<Node const>(node));
	}

	void ASTVisitor::visit(shptr<ExpressionStatement const> node)
	{
		visit(std::static_pointer_cast<Statement const>(node));
	}

	void ASTVisitor::visit(shptr<FieldDeclaration const> node)
	{
		visit(std::static_pointer_cast<ClassMember const>(node));
	}

	void ASTVisitor::visit(shptr<Ident const> node)
	{
		visit(std::static_pointer_cast<Node const>(node));
	}

	void ASTVisitor::visit(shptr<IfStatement const> node)
	{
		visit(std::static_pointer_cast<Statement const>(node));
	}

	void ASTVisitor::visit(shptr<LVDStatement const> node)
	{
		visit(std::static_pointer_cast<Statement const>(node));
	}

	void ASTVisitor::visit(shptr<MainMethodDeclaration const> node)
	{
		visit(std::static_pointer_cast<MethodDeclaration const>(node));
	}

	void ASTVisitor::visit(shptr<MethodDeclaration const> node)
	{
		visit(std::static_pointer_cast<ClassMember const>(node));
	}

	void ASTVisitor::visit(shptr<PostfixExpression const> node)
	{
		visit(std::static_pointer_cast<Expression const>(node));
	}

	void ASTVisitor::visit(shptr<pe::PrimaryExpression const> node)
	{
		visit(std::static_pointer_cast<Expression const>(node));
	}

	void ASTVisitor::visit(shptr<pe::Bool const> node)
	{
		visit(std::static_pointer_cast<pe::PrimaryExpression const>(node));
	}

	void ASTVisitor::visit(shptr<pe::Ident const> node)
	{
		visit(std::static_pointer_cast<pe::PrimaryExpression const>(node));
	}

	void ASTVisitor::visit(shptr<pe::Integer const> node)
	{
		visit(std::static_pointer_cast<pe::PrimaryExpression const>(node));
	}

	void ASTVisitor::visit(shptr<pe::MethodInvocation const> node)
	{
		visit(std::static_pointer_cast<pe::PrimaryExpression const>(node));
	}

	void ASTVisitor::visit(shptr<pe::NewArrayExpression const> node)
	{
		visit(std::static_pointer_cast<pe::PrimaryExpression const>(node));
	}

	void ASTVisitor::visit(shptr<pe::NewObjectExpression const> node)
	{
		visit(std::static_pointer_cast<pe::PrimaryExpression const>(node));
	}

	void ASTVisitor::visit(shptr<pe::Object const> node)
	{
		visit(std::static_pointer_cast<pe::PrimaryExpression const>(node));
	}

	void ASTVisitor::visit(shptr<po::PostfixOp const> node)
	{
		visit(std::static_pointer_cast<Node const>(node));
	}

	void ASTVisitor::visit(shptr<po::ArrayAccess const> node)
	{
		visit(std::static_pointer_cast<po::PostfixOp const>(node));
	}

	void ASTVisitor::visit(shptr<po::FieldAccess const> node)
	{
		visit(std::static_pointer_cast<po::PostfixOp const>(node));
	}

	void ASTVisitor::visit(shptr<po::MethodInvocation const> node)
	{
		visit(std::static_pointer_cast<po::PostfixOp const>(node));
	}

	void ASTVisitor::visit(shptr<Program const> node)
	{
		visit(std::static_pointer_cast<Node const>(node));
	}

	void ASTVisitor::visit(shptr<ReturnStatement const> node)
	{
		visit(std::static_pointer_cast<Statement const>(node));
	}

	void ASTVisitor::visit(shptr<Statement const> node)
	{
		visit(std::static_pointer_cast<Node const>(node));
	}

	void ASTVisitor::visit(shptr<Type const> node)
	{
		visit(std::static_pointer_cast<Node const>(node));
	}

	void ASTVisitor::visit(shptr<TypeIdent const> node)
	{
		visit(std::static_pointer_cast<Node const>(node));
	}

	void ASTVisitor::visit(shptr<ue::UnaryExpression const> node)
	{
		visit(std::static_pointer_cast<Expression const>(node));
	}

	void ASTVisitor::visit(shptr<ue::Neg const> node)
	{
		visit(std::static_pointer_cast<ue::UnaryExpression const>(node));
	}

	void ASTVisitor::visit(shptr<ue::Not const> node)
	{
		visit(std::static_pointer_cast<ue::UnaryExpression const>(node));
	}

	void ASTVisitor::visit(shptr<WhileStatement const> node)
	{
		visit(std::static_pointer_cast<Statement const>(node));
	}
}
