#include "ControlFlowOptimizer.hpp"

namespace firm
{
	ControlFlowOptimizer::ControlFlowOptimizer(ir_graph* irg) : GraphOptimizer(irg)
	{

	}

	void ControlFlowOptimizer::handle(Node node)
	{
		if (is_Cmp(node))
		{

			ir_node* left = get_irn_n(node, 0);
			ir_node* right = get_irn_n(node, 1);

			if (is_Const(left) && is_Const(right))
			{

				long left_value = get_tarval_long(get_Const_tarval(left));
				long right_value = get_tarval_long(get_Const_tarval(right));

#define SET_RELATION(A, B) \
					case ir_relation::ir_relation_ ## A : \
						set_Cmp_relation(node, left_value B right_value ? ir_relation::ir_relation_true : ir_relation::ir_relation_false); \
					break;

				switch (get_Cmp_relation(node))
				{
						SET_RELATION(equal, == )
						SET_RELATION(greater, > )
						SET_RELATION(greater_equal, >= )
						SET_RELATION(less, < )
						SET_RELATION(less_equal, <= )
						SET_RELATION(unordered_less_greater, != )

					default:
						break;
				} // switch

#undef SET_RELATION
			} // if (is_Const ...
			else if (left == right)
				set_Cmp_relation(node, get_Cmp_relation(node) & ir_relation_equal ? ir_relation_true : ir_relation_false);
		}
	}

	void ControlFlowOptimizer::cleanUp(Node node)
	{
		if (is_Proj(node))
		{
			Node child_node = node.getChild(0);

			if (is_Cond(child_node))
			{
				unsigned proj_num = get_Proj_num(node);
				Node cmp_node = child_node.getChild(0);
				ir_relation relation = get_Cmp_relation(cmp_node);

				if ((proj_num == pn_Cond_true  && relation == ir_relation::ir_relation_true)
				        || (proj_num == pn_Cond_false && relation == ir_relation::ir_relation_false))
				{
					// Exchange the Proj with an unconditional jump.
					replaceNode(node, new_r_Jmp(get_nodes_block(child_node)));
				}

				if ((proj_num == pn_Cond_true  && relation == ir_relation::ir_relation_false)
				        || (proj_num == pn_Cond_false && relation == ir_relation::ir_relation_true))
				{
					// Exchange Proj nodes leading to dead blocks with bad blocks.
					replaceNode(node, new_r_Bad(irg, get_modeX()));
				}
			}
		}
	}
}