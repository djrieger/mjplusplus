#ifndef CLASS_TYPE_VISITOR_HPP
#define CLASS_TYPE_VISITOR_HPP

#include "FirmVisitor.hpp"
#include "../ast/ClassDeclaration.hpp"

class ClassTypeVisitor : public FirmVisitor
{
	public:
		virtual void visit(shptr<const ast::ClassDeclaration> classDeclaration);
};

#endif