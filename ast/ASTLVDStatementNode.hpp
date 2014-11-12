#include "Node.hpp"

class LVDStatement : public Statement
{
	protected:
		TypeIdent& type_ident;

	public:
		LVDStatement(TypeIdent& type_ident);
}