#include "Program.hpp"

ast::Program::Program(std::unique_ptr<std::vector<ClassDeclaration>>& classes) : classes(std::move(classes))
{

}

std::string ast::Program::toString(unsigned int indent) const
{
	std::string class_def = "";

	for (auto& c : *classes)
		class_def += c.toString(indent);

	return class_def;
}
