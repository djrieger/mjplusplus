#ifndef MEMBER_VISITOR_HPP
#define MEMBER_VISITOR_HPP

#include "FirmVisitor.hpp"
#include "ClassVisitor.hpp"
#include "../../ast/MainMethodDeclaration.hpp"
#include "../../ast/FieldDeclaration.hpp"

namespace firm
{
	namespace visitor
	{
		class MemberVisitor : public FirmVisitor
		{
			protected:
				ir_graph* function_graph;
				ClassVisitor& classVisitor;

				/**
				 * Create method body firm nodes, create local variables for parameters and add
				 * a trailing return statement if necessary
				 */
				void visitMethodBodyAndFinalize(shptr<const ast::MethodDeclaration> methodDeclaration, ir_graph* irg);
			public:
				MemberVisitor(ClassVisitor& classVisitor);
				virtual void visit(shptr<const ast::MethodDeclaration> node);
				virtual void visit(shptr<const ast::MainMethodDeclaration> mainMethodDecl);
				virtual void visit(shptr<const ast::FieldDeclaration> fieldDeclaration);
				ir_graph* getGraph() const;
		};
	}
}
#endif
