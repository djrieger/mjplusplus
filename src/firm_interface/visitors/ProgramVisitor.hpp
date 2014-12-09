#ifndef PROGRAMVISITOR_HPP
#define PROGRAMVISITOR_HPP

#include "FirmVisitor.hpp"
#include "../../ast/Program.hpp"

namespace firm
{
	namespace visitor
	{
		class ProgramVisitor : public FirmVisitor
		{
			public:
				virtual void visit(shptr<const ast::Program> program);
		};
	}
}

#endif
