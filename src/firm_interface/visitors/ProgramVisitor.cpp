#include "ProgramVisitor.hpp"
#include "ClassVisitor.hpp"
#include "ProtoClassVisitor.hpp"

void firm::visitor::ProgramVisitor::visit(shptr<const ast::Program> program)
{
	auto classes = program->getClasses();

	//A first run over classes to capture all types, fields and method declarations.
	ProtoClassVisitor pcv;

	for (auto& class_ : *classes)
		class_->accept(pcv);

	//A second run over classes to inspect method bodies.
	ClassVisitor cv;

	for (auto& class_ : *classes)
		class_->accept(cv);
}
