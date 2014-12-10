#ifndef CLASS_VISITOR_HPP
#define CLASS_VISITOR_HPP

#include "FirmVisitor.hpp"
#include "../../ast/ClassDeclaration.hpp"

namespace firm
{
	namespace visitor
	{
		class ClassVisitor : public FirmVisitor
		{
			public:

				/**
				 * Visits a ClassDeclaration. Sets its owner to the the class type that's retrieved
				 * from the declaration's ident with the help of FirmInterface.
				 * Then uses a MemberVisitor to visit the declaration's members.
				 * @param classDeclaration the given classDeclaration
				 * @see MemberVisitor
				 * @see FirmInterface
				 */
				virtual void visit(shptr<const ast::ClassDeclaration> classDeclaration);
		};
	}
}
#endif
