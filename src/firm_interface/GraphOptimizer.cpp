#include "GraphOptimizer.hpp"

namespace firm
{

	GraphOptimizer::GraphOptimizer(ir_graph* irg): irg(irg)
	{

		newNodes = std::make_shared<std::set<ir_node*>>();
	}

	void GraphOptimizer::markOutNodesAsNew(ir_node* node)
	{
		ir_edge_t* edge = NULL;
		foreach_out_edge(node, edge)
		{
			ir_node* srcNode = get_edge_src_irn(edge);
			ir_printf("adding srcNode= %F at %p to worklist again\n", srcNode, srcNode);
			newNodes->insert(srcNode);
		}
	}

	shptr<std::set<ir_node*>> GraphOptimizer::getNewNodes() const
	{
		return this->newNodes;
	}
}