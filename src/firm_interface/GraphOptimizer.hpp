#ifndef GRAPH_OPTIMIZER_HPP
#define GRAPH_OPTIMIZER_HPP

#include <libfirm/firm.h>
#include <set>
#include "../globals.hpp"
#include "Node.hpp"

namespace firm
{
	class GraphOptimizer
	{
		protected:
			ir_graph* irg;
			shptr<std::set<ir_node*>> newNodes;

			/**
			 * add all successors to the worklist again
			 */
			void markOutNodesAsNew(ir_node* node);
		public:
			GraphOptimizer(ir_graph* irg);
			virtual void handle(Node) = 0;
			virtual void cleanUp(Node node) = 0;
			virtual bool graphChanged();
			shptr<std::set<ir_node*>> getNewNodes() const;
	};
}
#endif
