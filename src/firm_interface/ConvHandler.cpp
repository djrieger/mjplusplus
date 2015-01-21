#include "ConvHandler.hpp"

namespace firm
{
	ConvHandler::ConvHandler(ir_graph* irg) : GraphOptimizer(irg)
	{

	}

	void ConvHandler::handle(Node node)
	{

	}

	void ConvHandler::cleanUp(Node node)
	{
		if (is_Conv(node))
		{
			Node child = node.getChild(0);

			if (is_Conv(child) && node.getMode() == child.getMode())
				replaceNode(node, child);
			else if (is_Const(child))
				replaceNode(node, new_r_Const_long(irg, node.getMode(), child.getTarval().getLong()));
			else if (is_Conv(child))
				set_irn_n(node, 0, child.getChild(0));
		}
	}

} // namespace firm