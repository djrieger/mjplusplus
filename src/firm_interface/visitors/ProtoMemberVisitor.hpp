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

				/**
				 * The resultung function graph
				 */
				ir_graph* function_graph;

				/**
				 * The calling ProtoClassVisitor
				 */
				ProtoClassVisitor& classVisitor;

			public:

				/**
				 * Constructs a ProtoMemberVisitor from the given ProtoClassVisitor.
				 * Sets this owner to the one from the ProtoClassVisitor.
				 * @param classVisitor the given ProtoClassVisitor
				 */
				ProtoMemberVisitor(ProtoClassVisitor& classVisitor);

				/**
				 * Creates a method entity from the visited MethodDeclaration's mangled name
				 * and the visitor's owner.
				 * This entity is saved in the FirmInterface.
				 * @param methodDeclaration the visited MethodDeclaration
				 * @see FirmInterface
				 */
				virtual void visit(shptr<const ast::MethodDeclaration> methodDeclaration);

				/**
				 * Does nothing here.
				 * @param mainMethodDecl
				 */
				virtual void visit(shptr<const ast::MainMethodDeclaration> mainMethodDecl);

				/**
				 * Creates a field entity from the visited FieldDeclaration using it's mangled
				 * name and calculating its offset of the class type.
				 * This entity is saved in the FirmInterface.
				 * @param fieldDeclaration [description]
				 * @see FirmInterface
				 */
				virtual void visit(shptr<const ast::FieldDeclaration> fieldDeclaration);

				/**
				 * Returns the function graph.
				 * @return function graph
				 */
				ir_graph* getGraph() const;
		};
	}
}

#endif
