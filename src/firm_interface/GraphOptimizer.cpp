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
			// ir_printf("adding srcNode= %F at %p to worklist again\n", srcNode, srcNode);
			newNodes->insert(srcNode);
		}
	}

	shptr<std::set<ir_node*>> GraphOptimizer::getNewNodes() const
	{
		return this->newNodes;
	}

	bool GraphOptimizer::graphChanged()
	{
		return false;
	}

	void GraphOptimizer::processChildren(Node node, std::function<void (Node leftChild, Node rightChild)> fun)
	{
		Node child1 = node.getChild(0);

		if (node.getChildCount() == 1 && node.getTarval().isModeIs())
			fun(child1, NULL);
		else
		{
			Node child2 = node.getChild(1);
			fun(child1, child2);
		}
	}

	bool GraphOptimizer::tarvalIsZero(Tarval tarval)
	{
		return tarval && tarval.isNumeric() && tarval.getLong() == 0;
	}
}
