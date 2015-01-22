#include "GraphHandler.hpp"

namespace firm
{

	GraphHandler::GraphHandler(ir_graph* irg): irg(irg)
	{

		newNodes = std::make_shared<std::set<ir_node*>>();
		changed = false;
	}

	void GraphHandler::markOutNodesAsNew(ir_node* node)
	{
		ir_edge_t* edge = NULL;
		foreach_out_edge(node, edge)
		{
			ir_node* srcNode = get_edge_src_irn(edge);
			// ir_printf("adding srcNode= %F at %p to worklist again\n", srcNode, srcNode);
			newNodes->insert(srcNode);
		}
	}

	shptr<std::set<ir_node*>> GraphHandler::getNewNodes() const
	{
		return this->newNodes;
	}

	void GraphHandler::replaceNode(Node oldNode, Node newNode, bool keepTarval)
	{
		changed = true;
		oldNode.replaceWith(newNode, keepTarval);
	}

	bool GraphHandler::graphChanged() const
	{
		return changed;
	}

	void GraphHandler::processChildren(Node node, std::function<void (Node leftChild, Node rightChild)> fun)
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

	bool GraphHandler::tarvalIsZero(Tarval tarval)
	{
		return tarval && tarval.isNumeric() && tarval.getLong() == 0;
	}
}
