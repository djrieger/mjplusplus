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
			std::unique_ptr<std::vector<ClassDeclaration>> classes;

		public:
			Program(std::unique_ptr<std::vector<ClassDeclaration>>& classes);
			virtual std::string toString();

	};

}

#endif // PROGRAM_HPP
