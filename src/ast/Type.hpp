#ifndef TYPE_HPP
#define TYPE_HPP

#include "../globals.hpp"
#include "Node.hpp"
#include "Ident.hpp"
#include "Expression.hpp"

namespace ast
{
	class Type : public Node
	{
		public:
			enum Primitive_type
			{
				NONE,
				INT,
				BOOLEAN,
				VOID
			};

			Type(Type::Primitive_type primitive_type);
			Type(uptr<Ident> &class_name);			
			Type(Type::Primitive_type primitive_type, int dimension);
			Type(uptr<Ident> &class_name, int dimension);

			virtual void toString(std::ostream& out, unsigned int indent, uptr<Expression> &expression, bool = false) const;
			virtual void toString(std::ostream& out, unsigned int indent, bool = false) const;

			void setDimension(int dimension);

		private:
			Primitive_type primitive_type;
			uptr<Ident> class_name; 
			int dimension;
	};
}

#endif