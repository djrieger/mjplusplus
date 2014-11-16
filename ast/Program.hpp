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
			std::unique_ptr<std::vector<std::unique_ptr<ClassDeclaration>>> classes;

		public:
			Program(std::unique_ptr<std::vector<std::unique_ptr<ClassDeclaration>>>& classes);
			virtual void toString(std::ostream& out, unsigned int indent, bool = false) const;
	};

}

#endif // PROGRAM_HPP
