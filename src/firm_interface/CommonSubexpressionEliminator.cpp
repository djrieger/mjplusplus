#include "CommonSubexpressionEliminator.hpp"
#include "FirmInterface.hpp"
#include <iostream>

namespace firm
{
	CommonSubexpressionEliminator::CommonSubexpressionEliminator(ir_graph* irg): GraphOptimizer(irg)
	{

	}

	void CommonSubexpressionEliminator::handle(Node node)
	{
		newNodes->clear();

		bool changed = false;

		if (is_Const(node))
			changed = handleConst(node);

		if (changed)
			markOutNodesAsNew(node);
	}

	bool CommonSubexpressionEliminator::handleConst(Node node)
	{
		//std::cout << ">>>>>>>>>>>>>>>>>>>>>>>>> " << node.getNodeNumber() << std::endl;

		if (node.getMode() != mode_Is)
			return false;

		long int n = node.getTarval().getLong();
		auto constIt = const_nodes.find(n);
		bool change = constIt != const_nodes.end();

		if (change)
			node.replaceWith(constIt->second);
		else
			const_nodes.emplace(n, node);

		return change;
	}

	void CommonSubexpressionEliminator::cleanUp(Node node)
	{
		/*if (!replaceGeneric(node))
		{
			if (is_Add(node)) replaceAdd(node);
			else if (is_Mul(node)) replaceMul(node);
			else if (is_Sub(node)) replaceSub(node);
			else if (is_Minus(node)) replaceMinus(node);
			else if (is_Conv(node)) replaceConv(node);
		}

		// Todo: optimize booleans
		// Todo: optimize stuff like a + 0 not only for a's tarval (already implemented)
		// but also for a's value.
		*/
	}
}
