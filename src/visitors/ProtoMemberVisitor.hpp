#ifndef PROTO_MEMBER_VISITOR_HPP
#define PROTO_MEMBER_VISITOR_HPP

#include "FirmVisitor.hpp"
#include "ClassTypeVisitor.hpp"
#include "../ast/MainMethodDeclaration.hpp"
#include "../ast/FieldDeclaration.hpp"

class ProtoMemberVisitor : public FirmVisitor
{
	protected:
		ir_graph* function_graph;
		ClassTypeVisitor& classVisitor;

	public:
		ProtoMemberVisitor(ClassTypeVisitor& classVisitor);
		virtual void visit(shptr<const ast::MethodDeclaration> node);
		virtual void visit(shptr<const ast::MainMethodDeclaration> mainMethodDecl);
		virtual void visit(shptr<const ast::FieldDeclaration> fieldDeclaration);
		ir_graph* getGraph() const;
};

#endif
