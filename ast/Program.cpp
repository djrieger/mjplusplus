#include <algorithm>

#include "Program.hpp"

ast::Program::Program(std::unique_ptr<std::vector<std::unique_ptr<ClassDeclaration>>>& classes) : classes(std::move(classes))
{

}

void ast::Program::toString(std::ostream& out, unsigned int indent, bool) const
{
	//TODO: sort
	auto sortClasses = [](const std::unique_ptr<ast::ClassDeclaration>& a,
	                      const std::unique_ptr<ast::ClassDeclaration>& b) -> bool
	{
		return a->getName() < b->getName();
	};
	std::sort(classes->begin(), classes->end(), sortClasses);

	for (auto& c : *classes)
		c->toString(out, indent);
}
