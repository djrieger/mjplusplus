#ifndef COMMON_SUBEXPRESSION_ELIMINATOR_HPP
#define COMMON_SUBEXPRESSION_ELIMINATOR_HPP

#include <libfirm/firm.h>
#include <unordered_map>
#include "../globals.hpp"
#include "GraphOptimizer.hpp"
#include "Node.hpp"

namespace firm
{
	class CommonSubexpressionEliminator : public GraphOptimizer
	{
		protected:
			std::unordered_map<long int, Node> const_nodes;
			bool handleConst(Node node);
		public:
			CommonSubexpressionEliminator(ir_graph* irg);
			virtual void handle(Node);
			virtual void cleanUp(Node node);
	};
}
#endif
