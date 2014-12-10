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
				/**
				 * Iterates twices over all ClassDeclarations of the visited Program.
				 * The first run is done with a ProtoClassVisitor, that captures
				 * all types, fields and method declarations.
				 * The second run is done with a ClassVisitor to inspect the method bodies.
				 * @param program the visted Program
				 * @see ProtoClassVisitor
				 * @see ClassVisitor
				 */
				virtual void visit(shptr<const ast::Program> program);
		};
	}
}

#endif
