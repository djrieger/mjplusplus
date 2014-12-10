#ifndef TYPE_HPP
#define TYPE_HPP

#include <functional>

#include "../globals.hpp"
#include "Node.hpp"
#include "Ident.hpp"

namespace ast
{

	class Expression;

	class Type : public Node
	{
			friend struct std::hash<shptr<Type>>;
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
			bool operator==(Type const& other) const;
			bool operator!=(Type const& other) const;
			bool operator <(Type const& other) const;
			bool isRefType(bool checkNullType = false);
			bool isClassType();
			bool isInteger();
			bool isAnyInteger();
			bool isBool();
			bool isVoid();
			bool isArray();
			shptr<Type> de_array();

			void setDimension(int dimension);
			Primitive_type getPrimitiveType() const;
			std::string const& getClassName() const;
			shptr<ast::Ident> const& getClassNameIdent() const;
			std::string getName() const;
			int getDimension() const;
			virtual void accept(ASTVisitor& visitor) const;
			/**
			 *  returns whether this type is $System or $out
			 */
			bool isSpecialType() const;

		private:
			Primitive_type primitive_type;
			shptr<Ident> class_name;
			int dimension;
	};
}

namespace std
{
	template<>
	struct hash<shptr<ast::Type>>
	{
		size_t operator ()(shptr<ast::Type> const& t) const
		{
			hash<int> hi;
			hash<string> hs;
			return t ? hi(t->dimension) + (t->class_name ? hs(t->class_name->getName()) : 0) + t->primitive_type : 0;
		}
	};

	template<>
	struct equal_to<shptr<ast::Type>>
	{
		bool operator() (shptr<ast::Type> const& a, shptr<ast::Type> const& b) const
		{
			return a == b || *a == *b;
		}
	};
}

#endif
