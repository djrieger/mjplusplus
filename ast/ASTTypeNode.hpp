#include "Node.hpp"

class TypeIdent : public Node
{
	private:
		Ident& identifier;
		int dimension;
		int array_size;
		// NewArrayExpression einbauen? Expression wg Größe
		Primitive_Type primitive_type;
		std::string class_name; // maybe a pointer to the string table


	public:
		TypeIdent(Ident& identifier, Primitive_type primitive_type, std::string class_name = "", int dimension = 0, int array_size = 0);

		enum Primitive_Type
		{
			NONE,
			INT,
			BOOLEAN,
			VOID
		};
}