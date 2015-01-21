#include "BitFiddlingOptimizer.hpp"


namespace firm
{
	BitFiddlingOptimizer::BitFiddlingOptimizer(ir_graph* irg) : GraphOptimizer(irg) {}

	void BitFiddlingOptimizer::handle(Node) {}

	void BitFiddlingOptimizer::cleanUp(Node node)
	{
		if (is_Div(node))
		{
			Node dividend = node.getChild(1);
			Node divisor = node.getChild(2);

			if (is_Const(divisor) && divisor.getTarval().getLong() != 0)
			{
				bool is_divisor_positive = divisor.getTarval().getLong() > 0;
				unsigned long l_divisor = is_divisor_positive ?
				                          (unsigned long)  divisor.getTarval().getLong()
				                          : (unsigned long) - divisor.getTarval().getLong();

				if (__builtin_popcount(l_divisor) == 1)
				{

					int i_shr = sizeof(l_divisor) * 8 - __builtin_clzl(l_divisor) - 1;
					ir_node* shr;

					if (get_Div_resmode(node) == mode_Is)
					{
						shr = new_r_Shrs(get_nodes_block(node),
						                 dividend,
						                 new_r_Const_long(irg, mode_Iu, i_shr),
						                 mode_Is);

						if (!is_divisor_positive)
							shr = new_r_Minus(get_nodes_block(node), shr, mode_Is);
					}
					else if (get_Div_resmode(node) == mode_Iu)
						shr = new_r_Shr(get_nodes_block(node),
						                dividend,
						                new_r_Const_long(irg, mode_Iu, i_shr),
						                mode_Iu);
					else
					{
						printf("BitFiddlingOptimizer::cleanUp: Cannot convert division to shift because of unknown mode.\n");
						return;
					}

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
		else if (is_Mul(node))
		{
			Node factor = node.getChild(1);

			if (is_Const(factor))
			{
				bool is_factor_positive = factor.getTarval().getLong() >= 0;
				unsigned long l_factor = is_factor_positive ?
				                         (unsigned long)  factor.getTarval().getLong()
				                         : (unsigned long) - factor.getTarval().getLong();

				if (__builtin_popcount(l_factor) == 1)
				{
					int i_shl = sizeof(l_factor) * 8 - __builtin_clzl(l_factor) - 1;
					ir_node* shl = new_r_Shl(get_nodes_block(node),
					                         node.getChild(0),
					                         new_r_Const_long(irg, mode_Iu, i_shl),
					                         node.getMode());

					if (!is_factor_positive)
						shl = new_r_Minus(get_nodes_block(node), shl, node.getMode());

					replaceNode(node, shl);
				}
			}
		}
	}
}
