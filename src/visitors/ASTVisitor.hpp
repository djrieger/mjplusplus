#ifndef AST_VISITOR_HPP
#define AST_VISITOR_HPP

#include "../globals.hpp"

// visitors and inheritance don't mix that well in C++
namespace ast
{
	class Node;
	class Node;
	class Arguments;
	class ArrayAccess;
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
	class Block;
	class ClassDeclaration;
	class ClassMember;
	class Expression;
	class ExpressionStatement;
	class FieldAccess;
	class FieldDeclaration;
	class Ident;
	class IfStatement;
	class LVDStatement;
	class MainMethodDeclaration;
	class MethodDeclaration;
	class MethodInvocation;
	class PostfixExpression;
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
	class PostfixOp;
	class Program;
	class ReturnStatement;
	class Statement;
	class Type;
	class TypeIdent;
	namespace ue
	{
		class UnaryExpression;
		class Neg;
		class Not;
	}
	class WhileStatement;

}

class ASTVisitor
{
	public:
		virtual void visit(shptr<ast::Node const> node);
		virtual void visit(shptr<ast::Arguments const> node);
		virtual void visit(shptr<ast::ArrayAccess const> node);
		virtual void visit(shptr<ast::be::BinaryExpression const> node);
		virtual void visit(shptr<ast::be::AndAnd const> node);
		virtual void visit(shptr<ast::be::Eq const> node);
		virtual void visit(shptr<ast::be::EqEq const> node);
		virtual void visit(shptr<ast::be::GreaterThan const> node);
		virtual void visit(shptr<ast::be::GreaterThanEq const> node);
		virtual void visit(shptr<ast::be::Invalid const> node);
		virtual void visit(shptr<ast::be::LessThan const> node);
		virtual void visit(shptr<ast::be::LessThanEq const> node);
		virtual void visit(shptr<ast::be::Minus const> node);
		virtual void visit(shptr<ast::be::Mod const> node);
		virtual void visit(shptr<ast::be::Mult const> node);
		virtual void visit(shptr<ast::be::NotEq const> node);
		virtual void visit(shptr<ast::be::OrOr const> node);
		virtual void visit(shptr<ast::be::Plus const> node);
		virtual void visit(shptr<ast::be::Slash const> node);
		virtual void visit(shptr<ast::Block const> node);
		virtual void visit(shptr<ast::ClassDeclaration const> node);
		virtual void visit(shptr<ast::ClassMember const> node);
		virtual void visit(shptr<ast::Expression const> node);
		virtual void visit(shptr<ast::ExpressionStatement const> node);
		virtual void visit(shptr<ast::FieldAccess const> node);
		virtual void visit(shptr<ast::FieldDeclaration const> node);
		virtual void visit(shptr<ast::Ident const> node);
		virtual void visit(shptr<ast::IfStatement const> node);
		virtual void visit(shptr<ast::LVDStatement const> node);
		virtual void visit(shptr<ast::MainMethodDeclaration const> node);
		virtual void visit(shptr<ast::MethodDeclaration const> node);
		virtual void visit(shptr<ast::MethodInvocation const> node);
		virtual void visit(shptr<ast::PostfixExpression const> node);
		virtual void visit(shptr<ast::pe::PrimaryExpression const> node);
		virtual void visit(shptr<ast::pe::Bool const> node);
		virtual void visit(shptr<ast::pe::Ident const> node);
		virtual void visit(shptr<ast::pe::Integer const> node);
		virtual void visit(shptr<ast::pe::MethodInvocation const> node);
		virtual void visit(shptr<ast::pe::NewArrayExpression const> node);
		virtual void visit(shptr<ast::pe::NewObjectExpression const> node);
		virtual void visit(shptr<ast::pe::Object const> node);
		virtual void visit(shptr<ast::PostfixOp const> node);
		virtual void visit(shptr<ast::Program const> node);
		virtual void visit(shptr<ast::ReturnStatement const> node);
		virtual void visit(shptr<ast::Statement const> node);
		virtual void visit(shptr<ast::Type const> node);
		virtual void visit(shptr<ast::TypeIdent const> node);
		virtual void visit(shptr<ast::ue::UnaryExpression const> node);
		virtual void visit(shptr<ast::ue::Neg const> node);
		virtual void visit(shptr<ast::ue::Not const> node);
		virtual void visit(shptr<ast::WhileStatement const> node);
};

#endif
