#ifndef PROGRAM_HPP
#define PROGRAM_HPP

#include "../globals.hpp"
#include "Node.hpp"
#include "ClassDeclaration.hpp"
#include <vector>

namespace ast
{
	class Program : public Node
	{
		private:
			uptr<vec<uptr<ClassDeclaration>>> classes;

		public:
			Program(uptr<vec<uptr<ClassDeclaration>>> classes);
			virtual void toString(std::ostream& out, unsigned int indent, bool = false) const;
	};

}

#endif // PROGRAM_HPP
