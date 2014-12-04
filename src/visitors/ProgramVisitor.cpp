#include "ProgramVisitor.hpp"

void ProgramVisitor::visit(shptr<const ast::Program> program)
{
	auto classes = program->getClasses();

	ProtoClassVisitor pcv;

	for (auto& class_ : *classes)
		class_->accept(pcv);

	ClassTypeVisitor ctv;

	for (auto& class_ : *classes)
		class_->accept(ctv);

	ClassVisitor cv;

	for (auto& class_ : *classes)
		class_->accept(cv);
}
