#ifndef TYPEIDENT_HPP
#define TYPEIDENT_HPP

#include "Node.hpp"
#include "Ident.hpp"

namespace ast
{
	class TypeIdent : public Node
	{

		public:
			enum Primitive_type
			{
				NONE,
				INT,
				BOOLEAN,
				VOID
			};

			TypeIdent(std::unique_ptr<Ident> &identifier, Primitive_type primitive_type, std::string class_name = "", int dimension = 0, int array_size = 0);
			virtual std::string toString();

		private:
			std::unique_ptr<Ident> identifier;
			Primitive_type primitive_type;
			std::string class_name; // maybe a pointer to the string table
			int dimension;
			int array_size;
			// NewArrayExpression einbauen? Expression wg Größe



	};
}

#endif