#include "../globals.hpp"
#include "MethodInvocation.hpp"

ast::MethodInvocation::MethodInvocation(shptr<Ident> method_name, shptr<Arguments> arguments)
	: method_name(method_name), MethodInvocationBase(arguments)
{

}

void ast::MethodInvocation::toString(std::ostream& out, unsigned int indent, bool) const
{
	out << '.';
	method_name->toString(out, indent);
	arguments->toString(out, indent);
}

shptr<ast::Type> ast::MethodInvocation::get_type(SemanticAnalysis& sa, shptr<SymbolTable> symbolTable, shptr<ast::Type> callingType)
{
	if (callingType->isClassType())
	{
		auto class_table = sa.getClassTable();
		auto class_it = class_table.find(callingType->getClassName());

		if (class_it != class_table.end())
		{
			auto& class_item = class_it->second;
			auto method_table = class_item.methodTable->getMethodTable();
			auto method_it = method_table.find(method_name->getName());

			if (method_it != method_table.end())
				return performTypeChecks(method_name, method_it->second, sa, symbolTable);
			else
				sa.reportError("$type{" + callingType->getName() + "} has no method named $ident{" + method_name->getName() + "}",
				               method_name);
		}
		else
			sa.reportError("No such class: $type{" + callingType->getClassName() + "}", method_name);
	}
	else
		sa.reportError("Cannot invoke a method on a primitive or array type.", method_name);

	return shptr<ast::Type>();
}

shptr<const ast::MethodDeclaration> ast::MethodInvocation::getTarget() const
{
	return this->callTarget;
}

bool ast::MethodInvocation::lValueHelp() const
{
	return false;
}

void ast::MethodInvocation::accept(ASTVisitor& visitor) const
{
	visitor.visit(shared_from_this());
}