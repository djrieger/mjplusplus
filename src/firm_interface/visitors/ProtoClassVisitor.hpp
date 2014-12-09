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
				virtual void visit(shptr<const ast::ClassDeclaration> classDeclaration);
		};
	}
}

#endif
