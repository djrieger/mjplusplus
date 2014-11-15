#include "Program.hpp"

ast::Program::Program(std::unique_ptr<std::vector<ClassDeclaration>>& classes) : classes(std::move(classes))
{

}

void ast::Program::toString(std::ostream& out, unsigned int indent) const
{
	//TODO: sort
	for (auto& c : *classes)
		c.toString(out, indent);
}
