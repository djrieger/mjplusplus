#ifndef PROGRAM_HPP
#define PROGRAM_HPP

#include "Node.hpp"
#include "ClassDeclaration.hpp"
#include <vector>

namespace ast
{
	class Program : public Node
	{
		private:
			std::vector<ClassDeclaration> classes;

		public:
			Program(std::vector<ClassDeclaration> classes);
			std::string toString();

	};

}

#endif // PROGRAM_HPP