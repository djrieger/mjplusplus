#include <string>

namespace ast
{
	class Node
	{
		public:
			virtual std::string toString() = 0;
	};
}