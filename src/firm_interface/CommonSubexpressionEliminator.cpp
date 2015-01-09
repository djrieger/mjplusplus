#include "CommonSubexpressionEliminator.hpp"
#include "FirmInterface.hpp"
#include <iostream>
#include <algorithm>

namespace firm
{
	CommonSubexpressionEliminator::CommonSubexpressionEliminator(ir_graph* irg): GraphOptimizer(irg)
	{

	}

	void CommonSubexpressionEliminator::handle(Node node)
	{
		set_irn_link(node, NULL);

		if (is_Const(node))
			handleConst(node);
		else
		{
			ir_mode* mode = node.getMode();

			if (!is_Proj(node) && (mode == mode_Is || mode == mode_Lu))
				handleArithmetic(node);
		}
	}

	void CommonSubexpressionEliminator::handleConst(Node node)
	{
		//std::cout << ">>>>>>>>>>>>>>>>>>>>>>>>> " << node.getNodeNumber() << std::endl;

		if (node.getMode() == mode_Is)
		{
			long int n = get_tarval_long(get_Const_tarval(node));
			auto constIt = const_Is_nodes.find(n);
			bool change = constIt != const_Is_nodes.end();
			//std::cout << "constant " << n << " bool " << change << std::endl;

			if (change)
				set_irn_link(node, (void*) &constIt->second);
			else
				const_Is_nodes.emplace(n, node);
		}
		else if (node.getMode() == mode_Lu)
		{
			uint64_t n = get_tarval_long(get_Const_tarval(node));
			auto constIt = const_Lu_nodes.find(n);
			bool change = constIt != const_Lu_nodes.end();
			//std::cout << "constant " << n << " bool " << change << std::endl;

			if (change)
				set_irn_link(node, (void*) &constIt->second);
			else
				const_Lu_nodes.emplace(n, node);
		}

	}

	void CommonSubexpressionEliminator::handleArithmetic(Node node)
	{
		//std::cout << ">>>>>>>>>>>>>>>>>>>>>>>>> " << node.getNodeNumber() << std::endl;

		unsigned op = get_irn_opcode(node);
		vec<Node> childrenNodes = node.getChildren();
		vec<unsigned> children;

		for (auto c : childrenNodes)
			children.push_back(get_irn_node_nr(c));

		//std::cout << "<<<<<<<<<<<<<<<<<<<<<<<<< " << get_irn_node_nr(get_irn_n(node, -1)) << std::endl;
		nodeInfo elem {op, get_irn_node_nr(get_irn_n(node, -1)), (uint64_t) node.getMode(), children};
		//std::cout << op << " and first child " << children[0] << std::endl;
		auto nodeIt = comp_nodes.find(elem);

		if (nodeIt != comp_nodes.end())
			set_irn_link(node, (void*) &nodeIt->second);
		else
			comp_nodes.emplace(elem, node);
	}

	bool CommonSubexpressionEliminator::graphChanged()
	{
		return changed;
	}

	void CommonSubexpressionEliminator::cleanUp(Node node)
	{
		Node* replacement = (Node*) get_irn_link(node);

		if (replacement != NULL)
		{
			std::cout << "Replacing " << node.getNodeNumber() << " with " << replacement->getNodeNumber() << std::endl;
			node.replaceWith(*replacement);
			changed = true;
		}
	}
}
