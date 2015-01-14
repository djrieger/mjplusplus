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

				/**
				 * The resulting function graph
				 */
				ir_graph* function_graph;

				/**
				 * The calling ClassVisitor
				 */
				ClassVisitor& classVisitor;

				/**
				 * Create method body firm nodes, create local variables for parameters and add
				 * a trailing return statement if necessary.
				 * After that the graph is optimized and then finalized.
				 * @param methodDeclaration the method's declaration
				 * @param irg the graph for this method call
				 * @see StatementVisitor
				 */
				void visitMethodBodyAndFinalize(shptr<const ast::MethodDeclaration> methodDeclaration, ir_graph* irg);

				/**
				 * Optimize the function graph by folding the constants.
				 * @param irg the function graph
				 */
				void foldConstants(ir_graph* irg);

				void optimizeControlFlow(ir_graph* irg);

				void optimizeLocal(ir_graph* irg);

				/**
				 * Optimize the function graph by eliminating common subexpressions.
				 * @param irg the function graph
				 */
				void eliminateCommonSubexpressions(ir_graph* irg);

			public:

				/**
				 * Constructs a MemberVisitor from the given ClassVisitor.
				 * Sets this owner to the one from the ClassVisitor.
				 * @param classVisitor the given ClassVisitor
				 */
				MemberVisitor(ClassVisitor& classVisitor);

				/**
				 * Gets the entity of the corresponding method from the FirmInterface using the owner
				 * and mangled name and then creates a new function graph from it
				 * and the number of local variables. Then it evaluates the method body with the help
				 * of a StatementVisitor, optimizes and then finalizes the graph.
				 * @param methodDeclaration the method's declaration
				 * @see FirmInterface
				 * @see MemberVisitor::visitMethodBodyAndFinalize
				 */
				virtual void visit(shptr<const ast::MethodDeclaration> methodDeclaration);

				/**
				 * Creates a new Method-entity for the main method and uses it for a new function graph.
				 * This is then evaluated, optimized and finalized.
				 * @param mainMethodDecl the methods' declaration
				 * @see MemberVisitor::visitMethodBodyAndFinalize
				 */
				virtual void visit(shptr<const ast::MainMethodDeclaration> mainMethodDecl);

				/**
				 * Nothing to be done for fields.
				 */
				virtual void visit(shptr<const ast::FieldDeclaration>);

				/**
				 * Get the current graph.
				 * @return function graph
				 */
				ir_graph* getGraph() const;
		};
	}
}
#endif
