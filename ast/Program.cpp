#include "Program.hpp"

ast::Program::Program(std::vector<ClassDeclaration> classes) : classes(classes) {

}

std::string ast::Program::toString() {
	std::string class_def = "";
	for (c : classes) {
		class_def += c.toString();
	}
	return class_def;
}