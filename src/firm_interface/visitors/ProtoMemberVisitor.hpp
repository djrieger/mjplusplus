#ifndef PROTO_MEMBER_VISITOR_HPP
#define PROTO_MEMBER_VISITOR_HPP

#include "FirmVisitor.hpp"
#include "ProtoClassVisitor.hpp"
#include "../../ast/MainMethodDeclaration.hpp"
#include "../../ast/FieldDeclaration.hpp"

namespace firm
{
	namespace visitor
	{
		class ProtoMemberVisitor : public FirmVisitor
		{
			private:
				ssize_t byte_pos;
				ssize_t int_pos;

			protected:
				ir_graph* function_graph;
				ProtoClassVisitor& classVisitor;

			public:
				ProtoMemberVisitor(ProtoClassVisitor& classVisitor);
				virtual void visit(shptr<const ast::MethodDeclaration> node);
				virtual void visit(shptr<const ast::MainMethodDeclaration> mainMethodDecl);
				virtual void visit(shptr<const ast::FieldDeclaration> fieldDeclaration);
				ir_graph* getGraph() const;
		};
	}
}

#endif
