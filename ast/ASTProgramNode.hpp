#include "Node.hpp"

class Program : public Node
{
	private:
		std::vector<ClassDeclaration> classes;

	public:
		Program(std::vector<ClassDeclaration> classes);

}