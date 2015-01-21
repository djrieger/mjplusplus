#include "BitFiddlingOptimizer.hpp"


namespace firm
{
	BitFiddlingOptimizer::BitFiddlingOptimizer(ir_graph* irg) : GraphOptimizer(irg) {}

	void BitFiddlingOptimizer::handle(Node) {}

	void BitFiddlingOptimizer::cleanUp(Node node)
	{
		/*if (is_Div(node))
		{
			Node dividend = node.getChild(1);
			Node divisor = node.getChild(2);

			if (is_Const(divisor))
			{
				bool is_divisor_positive = divisor.getTarval().getLong() > 0;
				unsigned long l_divisor = (unsigned long) abs(divisor.getTarval().getLong());

				if (__builtin_popcount(l_divisor) == 1)
				{
					ir_node* shr;

					if (get_Div_resmode(node) == mode_Is)
					{
						shr = new_r_Shrs(get_nodes_block(node),
						                 dividend,
						                 new_r_Const_long(irg, mode_Iu, __builtin_ctzl(l_divisor)),
						                 mode_Is);

						if (!is_divisor_positive)
							shr = new_r_Minus(get_nodes_block(node), shr, mode_Is);
					}
					else if (get_Div_resmode(node) == mode_Iu)
						shr = new_r_Shr(get_nodes_block(node),
						                dividend,
						                new_r_Const_long(irg, mode_Iu, __builtin_ctzl(l_divisor)),
						                mode_Iu);
					else
						return;

					for (auto& ne : node.getOuts())
					{
						Node outChild = ne.first;

						if (outChild.getMode() == mode_M)
						{
							for (auto& e : outChild.getOuts())
								e.first.setChild(e.second, node.getChild(0));
						}
						else
							replaceNode(outChild, shr);
					}
				}
			}
		}
		else if (is_Mul(node))*/
		if (is_Mul(node))
		{
			Node factor = node.getChild(is_Const(node.getChild(0)) ? 0 : 1);

			if (is_Const(factor))
			{
				bool is_factor_positive = factor.getTarval().getLong() >= 0;
				unsigned long l_factor = (unsigned long) abs(factor.getTarval().getLong());

				if (__builtin_popcount(l_factor) == 1)
				{
					ir_node* shl = new_r_Shl(get_nodes_block(node),
					                         node.getChild(is_Const(node.getChild(0)) ? 1 : 0),
					                         new_r_Const_long(irg, mode_Iu, __builtin_ctzl(l_factor)),
					                         node.getMode());

					if (!is_factor_positive)
						shl = new_r_Minus(get_nodes_block(node), shl, node.getMode());

					replaceNode(node, shl);
				}
			}
		}
	}
}
