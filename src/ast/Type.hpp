#ifndef TYPE_HPP
#define TYPE_HPP

#include "../globals.hpp"
#include "Node.hpp"
#include "Ident.hpp"

namespace ast
{

	class Expression;

	class Type : public Node
	{
		public:
			enum Primitive_type
			{
				NONE,
				INT,
				BOOLEAN,
				VOID,
				INT_NEG_ONLY,
				NULL_TYPE
			};

			Type(Type::Primitive_type primitive_type);
			Type(shptr<Ident> class_name);
			Type(Type::Primitive_type primitive_type, int dimension);
			Type(shptr<Ident> class_name, int dimension);

			virtual void toString(std::ostream& out, unsigned int indent, shptr<Expression> const& expression, bool = false) const;
			virtual void toString(std::ostream& out, unsigned int indent, bool = false) const;
			bool operator==(Type const& other);
			bool operator!=(Type const& other);
			bool isRefType(bool checkNullType = false);
			bool isClassType();
			bool isInteger();
			bool isBool();
			bool isVoid();
			shptr<Type> de_array();

			void setDimension(int dimension);
			Primitive_type getPrimitiveType() const;
			std::string const& getClassName() const;
			shptr<ast::Ident> const& getClassNameIdent() const;
			std::string getName() const;
			int getDimension() const;
			virtual void accept(ASTVisitor& visitor) const;

		private:
			Primitive_type primitive_type;
			shptr<Ident> class_name;
			int dimension;
	};
}

#endif
