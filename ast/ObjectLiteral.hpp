#ifndef OBJECTLITERAL_HPP
#define OBJECTLITERAL_HPP
#include "Node.hpp"

namespace ast
{
	class ObjectLiteral : public Node
	{
		public:
			enum Object_Type
			{
				NULL_OBJECT,
				THIS_OBJECT
			};

			ObjectLiteral(Object_Type object_type);
			virtual std::string toString();

		private:
			Object_Type object_type;


	};
}

#endif // OBJECTLITERAL_HPP