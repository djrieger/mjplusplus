#ifndef PROGRAMVISITOR_HPP
#define PROGRAMVISITOR_HPP

#include "FirmVisitor.hpp"
#include "ClassVisitor.hpp"
#include "ProtoClassVisitor.hpp"
#include "ClassTypeVisitor.hpp"
#include "../ast/Program.hpp"

class ProgramVisitor : public FirmVisitor
{
	public:
		virtual void visit(shptr<const ast::Program> program);
};

#endif
