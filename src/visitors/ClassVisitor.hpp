#ifndef CLASS_VISITOR_HPP
#define CLASS_VISITOR_HPP

#include "FirmVisitor.hpp"
#include "../ast/ClassDeclaration.hpp"

class ClassVisitor : public FirmVisitor
{
	public:
		virtual void visit(shptr<const ast::ClassDeclaration> classDeclaration);
};

#endif
