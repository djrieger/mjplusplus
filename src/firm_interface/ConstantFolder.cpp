#include "ConstantFolder.hpp"
#include "FirmInterface.hpp"

namespace firm
{
	ConstantFolder::ConstantFolder(ir_graph* irg): GraphOptimizer(irg)
	{
		
	}

	void ConstantFolder::optimizePhi(ir_node* node)
	{
		std::cout << "---------------------------" << std::endl;
		auto prevTarval = (ir_tarval*)get_irn_link(node);
		int predCount = get_irn_arity(node);
		bool isConst = true;
		bool isBad = false;
		bool valSet = false;
		long val = 0;
		int i = 0;
		int numUnkowns = 0;
		ir_printf("processing %F at %p with %d predecessors and tarval %F\n", node, node, predCount, prevTarval);

		do
		{
			ir_node* pred = get_irn_n(node, i);

			// we now know, that the node is not const and
			// therefore, operate on tarvals from the map
			auto curTarval = (ir_tarval*)get_irn_link(pred);
			// to compare on previous tarval later on, set
			isConst = false;

			if (curTarval == tarval_unknown)
			{
				ir_printf("tarval of predecessor %F is unknown\n", pred);
				++numUnkowns;
				// just count the number of unknown values
			}
			else if (curTarval == tarval_bad)
			{
				ir_printf("tarval of predecessor %F is bad\n", pred);
				// if any input is bad, were bad aswell
				// no need to set isConst = false;
				isBad = true;
			}
			else if (get_tarval_mode(curTarval) == mode_Is)
			{
				// were not bad, were not unknown and were integer
				// compare the tarval with the current const val candidate
				auto cur = get_tarval_long(curTarval);

				if (valSet)
				{
					if (cur != val)
					{
						ir_printf("tarval of predecessor %F's value does not match the current value\n", pred);
						// now we know, that we can not replace anything.
						isBad = true;
					}
				}
				else
				{
					ir_printf("initial value has been set to predecessor %F's tarval: %d\n", pred, cur);
					// in first iteration: set const candidate.
					val = cur;
					valSet = true;
				}
			}
			else
			{
				// not unknown, not bad, not Is, so nothing we currently handle
			}

			++i;
		}
		while (i < predCount && !isBad);

		ir_printf("%F:\t", node);

		if (isBad)
		{
			std::cout << "is bad" << std::endl;
			set_irn_link(node, (void*)tarval_bad);
		}
		else if (valSet)
		{
			// get_tarval_mode(prevTarval) == mode_Is && get_tarval_long(prevTarval) != val
			// we do have a change in the tarval of the current node, so update this accordingly
			ir_tarval* newTarval = new_tarval_from_long(val, mode_Is);
			set_irn_link(node, (void*) newTarval);
			
			if (newTarval != prevTarval)
				markOutNodesAsNew(node);

			std::cout << "tarval has been updated" << std::endl;
		}
		else
			std::cout << "remains totally unchanged" << std::endl;
	}

	void ConstantFolder::updateTarvalForArithmeticNode(ir_node* node)
	{
		ir_node* child1 = get_irn_n(node, 0);
		ir_tarval* tarval1 = (ir_tarval*)get_irn_link(child1);

		if (!tarval1)
			tarval1 = tarval_unknown;

		ir_node* child2 = NULL;
		ir_tarval* tarval2 = NULL;

		if (get_irn_arity(node) > 1)
		{
			child2 = get_irn_n(node, 1);
			tarval2 = (ir_tarval*)get_irn_link(child2);

			if (!tarval2)
				tarval2 = tarval_unknown;
		}

		if (get_tarval_mode(tarval1) == mode_Is && (is_Minus(node) || get_tarval_mode(tarval2) == mode_Is))
		{
			ir_tarval* resultVal;

			if (is_Add(node)) resultVal = tarval_add(tarval1, tarval2);
			else if (is_Sub(node)) resultVal = tarval_sub(tarval1, tarval2, NULL);
			else if (is_Mul(node)) resultVal = tarval_mul(tarval1, tarval2);
			else if (is_Minus(node)) resultVal = tarval_neg(tarval1);
			//else if (is_Div(node)) resultVal = tarval_div(tarval1, tarval2);
			else
				throw "updateTarvalForArithmeticNode called on illegal node";

			set_irn_link(node, (void*) resultVal);
			markOutNodesAsNew(node);
		}
	}

	void ConstantFolder::updateTarvalAndExchange(ir_node* oldNode, ir_node* newNode)
	{
		//TODO: Fix segfaults
		updateTarvalForArithmeticNode(oldNode);
		// exchange(oldNode, newNode);
	}

	void ConstantFolder::updateTarvalAndExchangeMemory(ir_node* oldNode, ir_node* newNode)
	{
		/* TODO: Only tested for Div and Mod currently
		 * may need adjusting for Load, Store, ... (any node requring a Proj)
		 */
		/*
		for (auto& ne : FirmInterface::getInstance().getOuts(oldNode))
		{
			ir_node* o = ne.first;

			if (get_irn_mode(o) == mode_M)
			{
				// Relink memory chain
				for (auto& e : FirmInterface::getInstance().getOuts(o))
					set_irn_n(e.first, e.second, get_irn_n(oldNode, 0));
			}
			//else
				//exchange(o, newNode);
		}
		*/
	}

	void ConstantFolder::handlePhi(ir_node* node)
	{
		optimizePhi(node);
	}

	void ConstantFolder::handleMinus(ir_node* node)
	{
		ir_node* child = get_irn_n(node, 0);

		if (is_Const(child))
			updateTarvalAndExchange(node, new_r_Const_long(irg, get_irn_mode(node), -get_tarval_long(computed_value(child))));
		else if (is_Minus(child))
			updateTarvalAndExchange(node, get_irn_n(child, 0));
	}

	void ConstantFolder::handleAdd(ir_node* node)
	{
		ir_node* left = get_irn_n(node, 0);
		ir_node* right = get_irn_n(node, 1);

		// Both arguments are constants.
		// if (is_Const(left) && is_Const(right))
		// {
			updateTarvalForArithmeticNode(node);
		// }
		// else
		// {
			// Check whether at least one argument is 0, and if so,
			// apply the rule x + 0 = x (or 0 + x = x).
			/*
				TODO: Reenable
			if (is_Const(left) && get_tarval_long(computed_value(left)) == 0)
				updateTarvalAndExchange(node, right);

			if (is_Const(right) && get_tarval_long(computed_value(right)) == 0)
				updateTarvalAndExchange(node, left);
				*/
		// }
	}

	void ConstantFolder::handleSub(ir_node* node)
	{
		ir_tarval* tarVal = computed_value(node);

		if (/* tarVal != tarval_unknown && */ tarVal != tarval_bad)
			updateTarvalAndExchange(node, NULL /* new_r_Const_long(irg, get_irn_mode(node), get_tarval_long(tarVal))*/);
		else
		{
			// Check whether at least one argument is 0, and if so,
			// apply the rule x - 0 = x (or 0 - x = -x).
			ir_node* left = get_irn_n(node, 0);
			ir_node* right = get_irn_n(node, 1);

			if (is_Const(left) && get_tarval_long(computed_value(left)) == 0)
				updateTarvalAndExchange(node, new_r_Minus(get_nodes_block(node), right, get_irn_mode(node)));

			if (is_Const(right) && get_tarval_long(computed_value(right)) == 0)
				updateTarvalAndExchange(node, new_r_Minus(get_nodes_block(node), left, get_irn_mode(node)));
		}
	}

	void ConstantFolder::handleMul(ir_node* node)
	{
		//ir_tarval* tarVal = computed_value(node);
		updateTarvalForArithmeticNode(node);

		return;

		
		ir_tarval* tarVal = (ir_tarval*)get_irn_link(node);
		if (tarVal != tarval_unknown && tarVal != tarval_bad)
			//updateTarvalAndExchange(node, new_r_Const_long(irg, get_irn_mode(node), get_tarval_long(tarVal)));
			updateTarvalForArithmeticNode(node);
		else
		{
			ir_node* left = get_irn_n(node, 0);
			ir_node* right = get_irn_n(node, 1);

			// If possible, apply the rules:
			//     x * (-1) = -x
			//     x * 1 = x
			//     x * 0 = 0
			if (is_Const(left))
			{
				long value = get_tarval_long(computed_value(left));

				if (value == -1)
					updateTarvalAndExchange(node, new_r_Minus(get_nodes_block(node), right, get_irn_mode(node)));
				else if (value == 0)
					updateTarvalAndExchange(node, new_r_Const_long(irg, get_irn_mode(node), 0));
				else if (value == 1)
					updateTarvalAndExchange(node, right);
			}

			// See above...
			if (is_Const(right))
			{
				long value = get_tarval_long(computed_value(right));

				if (value == -1)
					updateTarvalAndExchange(node, new_r_Minus(get_nodes_block(node), left, get_irn_mode(node)));
				else if (value == 0)
					updateTarvalAndExchange(node, new_r_Const_long(irg, get_irn_mode(node), 0));
				else if (value == 1)
					updateTarvalAndExchange(node, left);
			}
		}
	}

	void ConstantFolder::handleDivAndMod(ir_node* node)
	{
		// Get children of div node (operands)
		ir_node* dividend = get_irn_n(node, 1);
		ir_node* divisor = get_irn_n(node, 2);

		if (is_Const(dividend) && is_Const(divisor))
		{
			long divisorValue = get_tarval_long(computed_value(divisor));

			if (divisorValue != 0)
			{
				long dividendValue = get_tarval_long(computed_value(dividend));
				ir_node* replacement = new_r_Const_long(irg, mode_Is, is_Div(node) ? dividendValue / divisorValue : dividendValue % divisorValue);
				updateTarvalAndExchangeMemory(node, replacement);
			}
			else
				updateTarvalAndExchangeMemory(node, new_r_Unknown(irg, mode_Is));
		}
		else if (is_Const(dividend))
		{
			long value = get_tarval_long(computed_value(dividend));

			if (value == 0)
				updateTarvalAndExchangeMemory(node, new_r_Const_long(irg, mode_Is, 0));
			else if (value == 1 && is_Div(node))
				updateTarvalAndExchangeMemory(node, new_r_Const_long(irg, mode_Is, 1));
		}
		else if (is_Const(divisor))
		{
			long value = get_tarval_long(computed_value(divisor));

			if (value == -1)
				updateTarvalAndExchangeMemory(node, is_Div(node) ? new_r_Minus(get_nodes_block(node), dividend, mode_Is) : new_r_Const_long(irg, mode_Is, 0));
			else if (value == 0)
				updateTarvalAndExchangeMemory(node, new_r_Unknown(irg, mode_Is));
			else if (value == 1)
				updateTarvalAndExchangeMemory(node, is_Div(node) ? dividend : new_r_Const_long(irg, mode_Is, 0));
		}
		else if (dividend == divisor)
			updateTarvalAndExchangeMemory(node, new_r_Const_long(irg, mode_Is, is_Div(node) ? 1 : 0));
	}

	void ConstantFolder::handleProj(ir_node* node)
	{
		// Get first child of proj node
		ir_node* child_node = get_irn_n(node, 0);

		if (is_Cond(child_node))
		{
			unsigned proj_num = get_Proj_num(node);
			ir_node* cmp_node = get_irn_n(child_node, 0);
			ir_relation relation = get_Cmp_relation(cmp_node);

			if ((proj_num == pn_Cond_true  && relation == ir_relation::ir_relation_true)
			        || (proj_num == pn_Cond_false && relation == ir_relation::ir_relation_false))
			{
				// Exchange the Proj with an unconditional jump.
				//exchange(node, new_r_Jmp(get_nodes_block(child_node)));
			}

			if ((proj_num == pn_Cond_true  && relation == ir_relation::ir_relation_false)
			        || (proj_num == pn_Cond_false && relation == ir_relation::ir_relation_true))
			{
				// Exchange Proj nodes leading to dead blocks with bad blocks.
				// TODO: Remove bad nodes.
				//exchange(node, new_r_Bad(irg, get_modeX()));
			}
		}
	}

	void ConstantFolder::handleCmp(ir_node* node)
	{
		ir_node* left = get_irn_n(node, 0);
		ir_node* right = get_irn_n(node, 1);

		if (is_Const(left) && is_Const(right))
		{

			long left_value = get_tarval_long(computed_value(left));
			long right_value = get_tarval_long(computed_value(right));

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

	void ConstantFolder::handleConv(ir_node* node)
	{
		// This removes *some* unnecessary conversions, notably those
		// occuring during calls to System.out.println, but this should
		// probably be extended for more general useless conversions.
		ir_node* child = get_irn_n(node, 0);
/*
		if (is_Conv(child))
		{
			ir_node* grand_child = get_irn_n(child, 0);

			if (get_irn_mode(node) == get_irn_mode(grand_child))
				exchange(node, grand_child);
		}
		else if (is_Const(child))
			exchange(node, new_r_Const_long(irg, get_irn_mode(node), get_tarval_long(computed_value(child))));
		*/
	}

	void ConstantFolder::handle(ir_node* node)
	{
		newNodes->clear();

		// TODO: Fix segfaults
		if (is_Phi(node) && get_irn_mode(node) == mode_Is) handlePhi(node);
		else if (is_Minus(node)) handleMinus(node);
		else if (is_Add(node)) handleAdd(node);
		else if (is_Sub(node)) handleSub(node);
		else if (is_Mul(node)) handleMul(node);
		else if (is_Div(node) || is_Mod(node)) handleDivAndMod(node);
		else if (is_Proj(node)) handleProj(node);
		//else if (is_Cmp(node)) handleCmp(node);
		else if (is_Conv(node)) handleConv(node);
	}
}
