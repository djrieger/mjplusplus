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
		virtual void visit(shptr<const ast::Node> node);
		virtual void visit(shptr<const ast::MethodDeclaration> node);
		virtual void visit(shptr<const ast::FieldDeclaration> node);
		ir_graph* getGraph() const;
};

#endif