#include "Node.hpp"

namespace ast
{
	class ObjectLiteral : public Node
	{
		private:
			Object_Type object_type;

		public:
			ObjectLiteral(Object_Type object_type);

			enum Object_Type
			{
				NULL_OBJECT,
				THIS_OBJECT
			};
	}
}