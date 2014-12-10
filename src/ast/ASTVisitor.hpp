#ifndef AST_VISITOR_HPP
#define AST_VISITOR_HPP

#include "../globals.hpp"

namespace ast
{
	class Node;
	class Arguments;
	namespace be
	{
		class BinaryExpression;
		class AndAnd;
		class Eq;
		class EqEq;
		class GreaterThan;
		class GreaterThanEq;
		class Invalid;
		class LessThan;
		class LessThanEq;
		class Minus;
		class Mod;
		class Mult;
		class NotEq;
		class OrOr;
		class Plus;
		class Slash;
	}
	class ClassDeclaration;
	class ClassMember;
	class Expression;
	class FieldDeclaration;
	class Ident;
	class MainMethodDeclaration;
	class MethodDeclaration;
	namespace pe
	{
		class PrimaryExpression;
		class Bool;
		class Ident;
		class Integer;
		class MethodInvocation;
		class NewArrayExpression;
		class NewObjectExpression;
		class Object;
	}
	namespace po
	{
		class PostfixExpression;
		class PostfixOp;
		class ArrayAccess;
		class FieldAccess;
		class MethodInvocation;
	}
	class Program;
	class Type;
	class TypeIdent;
	namespace stmt
	{
		class Statement;
		class Block;
		class ExpressionStatement;
		class IfStatement;
		class LVDStatement;
		class ReturnStatement;
		class WhileStatement;
	}
	namespace ue
	{
		class UnaryExpression;
		class Neg;
		class Not;
	}

	class ASTVisitor
	{
		public:
			virtual void visit(shptr<Node const> node);
			virtual void visit(shptr<Arguments const> node);
			virtual void visit(shptr<be::BinaryExpression const> node);
			virtual void visit(shptr<be::AndAnd const> node);
			virtual void visit(shptr<be::Eq const> node);
			virtual void visit(shptr<be::EqEq const> node);
			virtual void visit(shptr<be::GreaterThan const> node);
			virtual void visit(shptr<be::GreaterThanEq const> node);
			virtual void visit(shptr<be::Invalid const> node);
			virtual void visit(shptr<be::LessThan const> node);
			virtual void visit(shptr<be::LessThanEq const> node);
			virtual void visit(shptr<be::Minus const> node);
			virtual void visit(shptr<be::Mod const> node);
			virtual void visit(shptr<be::Mult const> node);
			virtual void visit(shptr<be::NotEq const> node);
			virtual void visit(shptr<be::OrOr const> node);
			virtual void visit(shptr<be::Plus const> node);
			virtual void visit(shptr<be::Slash const> node);
			virtual void visit(shptr<ClassDeclaration const> node);
			virtual void visit(shptr<ClassMember const> node);
			virtual void visit(shptr<Expression const> node);
			virtual void visit(shptr<FieldDeclaration const> node);
			virtual void visit(shptr<Ident const> node);
			virtual void visit(shptr<MainMethodDeclaration const> node);
			virtual void visit(shptr<MethodDeclaration const> node);
			virtual void visit(shptr<pe::PrimaryExpression const> node);
			virtual void visit(shptr<pe::Bool const> node);
			virtual void visit(shptr<pe::Ident const> node);
			virtual void visit(shptr<pe::Integer const> node);
			virtual void visit(shptr<pe::MethodInvocation const> node);
			virtual void visit(shptr<pe::NewArrayExpression const> node);
			virtual void visit(shptr<pe::NewObjectExpression const> node);
			virtual void visit(shptr<pe::Object const> node);
			virtual void visit(shptr<po::PostfixExpression const> node);
			virtual void visit(shptr<po::PostfixOp const> node);
			virtual void visit(shptr<po::ArrayAccess const> node);
			virtual void visit(shptr<po::FieldAccess const> node);
			virtual void visit(shptr<po::MethodInvocation const> node);
			virtual void visit(shptr<Program const> node);
			virtual void visit(shptr<stmt::Statement const> node);
			virtual void visit(shptr<stmt::Block const> node);
			virtual void visit(shptr<stmt::ExpressionStatement const> node);
			virtual void visit(shptr<stmt::IfStatement const> node);
			virtual void visit(shptr<stmt::LVDStatement const> node);
			virtual void visit(shptr<stmt::ReturnStatement const> node);
			virtual void visit(shptr<stmt::WhileStatement const> node);
			virtual void visit(shptr<Type const> node);
			virtual void visit(shptr<TypeIdent const> node);
			virtual void visit(shptr<ue::UnaryExpression const> node);
			virtual void visit(shptr<ue::Neg const> node);
			virtual void visit(shptr<ue::Not const> node);
	};
}

#endif
