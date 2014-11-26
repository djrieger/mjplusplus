#ifndef MEMBER_VISITOR_HPP
#define MEMBER_VISITOR_HPP

#include "FirmVisitor.hpp"
#include "../ast/MethodDeclaration.hpp"
#include "../ast/FieldDeclaration.hpp"

class MemberVisitor : public FirmVisitor
{
	protected:
		ir_graph* function_graph;
	public:
		MemberVisitor(FirmInterface& firmInterface);
		virtual void visit(shptr<const ast::Node> node);
		virtual void visit(shptr<const ast::MethodDeclaration> node);
		virtual void visit(shptr<const ast::FieldDeclaration> fieldDeclaration);
		ir_graph* getGraph() const;
};

#endif