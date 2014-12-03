#include "ProgramVisitor.hpp"

void ProgramVisitor::visit(shptr<const ast::Program> program)
{
	auto classes = program->getClasses();
	ClassVisitor cv;

	for (auto& class_ : *classes)
		class_->accept(cv);
}
