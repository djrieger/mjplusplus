#ifndef GRAPH_OPTIMIZER_HPP
#define GRAPH_OPTIMIZER_HPP

#include <libfirm/firm.h>
#include <set>
#include "../globals.hpp"
#include "Node.hpp"
#include "Tarval.hpp"

namespace firm
{
	class GraphOptimizer
	{
		protected:
			ir_graph* irg;
			shptr<std::set<ir_node*>> newNodes;
			bool changed;

			/**
			 * add all successors to the worklist again
			 */
			void markOutNodesAsNew(ir_node* node);
		public:
			GraphOptimizer(ir_graph* irg);
			virtual void handle(Node node) = 0;
			virtual void cleanUp(Node node) = 0;
			void replaceNode(Node oldNode, Node newNode, bool keepTarval = false);
			bool graphChanged() const;
			void processChildren(Node node, std::function<void (Node leftChild, Node rightChild)> fun);
			bool tarvalIsZero(Tarval tarval);
			shptr<std::set<ir_node*>> getNewNodes() const;
	};
}
#endif
