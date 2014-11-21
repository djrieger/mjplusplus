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
			Type(shptr<Ident> class_name);
			Type(Type::Primitive_type primitive_type, int dimension);
			Type(shptr<Ident> class_name, int dimension);

			virtual void toString(std::ostream& out, unsigned int indent, shptr<Expression>& expression, bool = false) const;
			virtual void toString(std::ostream& out, unsigned int indent, bool = false) const;
			bool operator==(Type const& other);
			bool operator!=(Type const& other);
			bool isRefType();
			shptr<Type> de_array();

			void setDimension(int dimension);
			Primitive_type getPrimitiveType() const;
			std::string const& getClassName() const;
			std::string getName() const;

		private:
			Primitive_type primitive_type;
			shptr<Ident> class_name;
			int dimension;
	};
}

#endif
