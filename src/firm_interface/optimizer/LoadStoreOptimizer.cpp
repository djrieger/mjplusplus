#include "LoadStoreOptimizer.hpp"
#include "../FirmInterface.hpp"
#include <iostream>
#include <algorithm>

namespace firm
{
	LoadStoreOptimizer::LoadStoreOptimizer(ir_graph* irg): GraphHandler(irg)
	{

	}

	void LoadStoreOptimizer::handle(Node node)
	{
		set_irn_link(node, NULL);

		if (is_Load(node))
			handleLoad(node);
	}

	void LoadStoreOptimizer::handleLoad(Node node)
	{
		Node prevMemOp = node.getChild(0).getChild(0);
		Node loadAddress = node.getChild(1);

		if (is_Store(prevMemOp) && loadAddress == prevMemOp.getChild(1))
		{

			for (auto& ne : node.getOuts())
			{
				Node outChild = ne.first;

				if (outChild.getMode() == mode_M)
				{
					// Relink memory chain
					ne.first.setChild(ne.second, prevMemOp);
				}
				else
					outChild.replaceWith(prevMemOp.getChild(2));
			}
		}
	}

	void LoadStoreOptimizer::cleanUp(Node node)
	{
	}
}
