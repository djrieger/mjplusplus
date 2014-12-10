#ifndef PROTO_CLASS_VISITOR_HPP
#define PROTO_CLASS_VISITOR_HPP

#include "FirmVisitor.hpp"
#include "../../ast/ClassDeclaration.hpp"

namespace firm
{
	namespace visitor
	{
		class ProtoClassVisitor : public FirmVisitor
		{
			public:

				/**
				 * Creates a new class type for the visited ClassDeclaration
				 * and then uses a ProtoMemberVisitor to evaluate its members.
				 * @param classDeclaration the visited ClassDeclaration
				 * @see ProtoMemberVisitor
				 */
				virtual void visit(shptr<const ast::ClassDeclaration> classDeclaration);
		};
	}
}

#endif
