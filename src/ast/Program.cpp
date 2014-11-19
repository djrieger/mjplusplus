#include "../globals.hpp"
#include <algorithm>

#include "Program.hpp"

ast::Program::Program(uptr<vec<uptr<ClassDeclaration>>> classes) : classes(std::move(classes))
{

}

void ast::Program::toString(std::ostream& out, unsigned int indent, bool) const
{
	//TODO: sort
	auto sortClasses = [](const uptr<ast::ClassDeclaration>& a,
	                      const uptr<ast::ClassDeclaration>& b) -> bool
	{
		return a->getName() < b->getName();
	};
	std::sort(classes->begin(), classes->end(), sortClasses);

	for (auto& c : *classes)
		c->toString(out, indent);
}
