#include "FirmNodeHandler.hpp"
#include "FirmInterface.hpp"

namespace firm
{
	FirmNodeHandler::FirmNodeHandler(ir_graph* irg): irg(irg)
	{
		newNodes = std::make_shared<std::set<ir_node*>>();
	}

	void FirmNodeHandler::optimizePhi(ir_node* node)
	{
		int predCount = get_irn_arity(node);

		bool onlyUnknowns = true;

		bool numWasFound = false;
		int numFound = -1;

		std::cout << "phi has " << predCount << " predecessors" << std::endl;

		for (int i = 0; i < predCount; i++)
		{
			ir_node* pred = get_irn_n(node, i);
			ir_tarval* curTarval = (ir_tarval*)get_irn_link(pred);

			if (curTarval == tarval_bad ||
			        // number with different value found
			        (numWasFound &&
			         get_tarval_mode(curTarval) == mode_Is &&
			         numFound != get_tarval_long(curTarval)))
			{
				std::cout << "case 1" << std::endl;
				// set tarval of phi to bad
				set_irn_link(node, (void*)tarval_bad);

				return;
			}
			// handle only unknown values
			else if (curTarval != tarval_unknown)
				onlyUnknowns = false;

			// if current predecessor is a number, store index of predecessor and
			if (get_tarval_mode(curTarval) == mode_Is)
			{
				numWasFound = true;
				numFound = get_tarval_long(curTarval);
				std::cout << "num found: " << numFound << std::endl;
				std::cout << "phi predecessor: ";
				ir_printf("%F\n", pred);
			}
		}

		// only unknown values found
		if (onlyUnknowns)
		{
			std::cout << "case 4" << std::endl;
			set_irn_link(node, (void*)tarval_unknown);
		}
		// n times same number found
		else if (numWasFound)
		{
			std::cout << "case 3" << std::endl;
			//TODO: I'm not sure you may do this here...
			ir_node* newConstNode = new_r_Const_long(get_irn_irg(node), mode_Is, numFound);
			set_irn_link(newConstNode, (void*)new_tarval_from_long(numFound, mode_Is));
			exchange(node, newConstNode);
		}
		else
			// n - 1 unknown values and 1 number found
		{
			std::cout << "case 2" << std::endl;
			set_irn_link(node, (ir_tarval*)new_tarval_from_long(numFound, mode_Is));
		}
	}

	void FirmNodeHandler::updateTarvalForArithmeticNode(ir_node* node)
	{
		ir_node* child1 = get_irn_n(node, 0);
		ir_tarval* tarval1 = (ir_tarval*)get_irn_link(child1);
		ir_node* child2 = NULL;
		ir_tarval* tarval2 = NULL;

		if (get_irn_arity(node) > 1)
		{
			child2 = get_irn_n(node, 1);
			tarval2 = (ir_tarval*)get_irn_link(child2);
		}

		if (get_tarval_mode(tarval1) == mode_Is && get_tarval_mode(tarval2) == mode_Is)
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
			ir_edge_t* edge = NULL;
			foreach_out_edge(node, edge)
			{
				ir_node* srcNode = get_edge_src_irn(edge);
				ir_printf("srcNode= %F\n", srcNode);
				newNodes->insert(srcNode);
			}
		}
	}

	void FirmNodeHandler::updateTarvalAndExchange(ir_node* oldNode, ir_node* newNode)
	{
		//TODO: Fix segfaults
		// updateTarvalForArithmeticNode(oldNode);
		exchange(oldNode, newNode);
	}

	void FirmNodeHandler::updateTarvalAndExchangeMemory(ir_node* oldNode, ir_node* newNode)
	{
		/* TODO: Only tested for Div and Mod currently
		 * may need adjusting for Load, Store, ... (any node requring a Proj)
		 */
		for (auto& ne : FirmInterface::getInstance().getOuts(oldNode))
		{
			ir_node* o = ne.first;

			if (get_irn_mode(o) == mode_M)
			{
				// Relink memory chain
				for (auto& e : FirmInterface::getInstance().getOuts(o))
					set_irn_n(e.first, e.second, get_irn_n(oldNode, 0));
			}
			else
				exchange(o, newNode);
		}
	}

	shptr<std::set<ir_node*>> FirmNodeHandler::getNewNodes() const
	{
		return this->newNodes;
	}

	void FirmNodeHandler::handleConst(ir_node* node)
	{
		// set tarval of this const node as its irn_link value
		set_irn_link(node, (void*)get_Const_tarval(node));
	}

	void FirmNodeHandler::handlePhi(ir_node* node)
	{
		optimizePhi(node);
	}

	void FirmNodeHandler::handleMinus(ir_node* node)
	{
		ir_node* child = get_irn_n(node, 0);

		if (is_Const(child))
			updateTarvalAndExchange(node, new_r_Const_long(irg, mode_Is, -get_tarval_long(computed_value(child))));
		else if (is_Minus(child))
			updateTarvalAndExchange(node, get_irn_n(child, 0));
	}

	void FirmNodeHandler::handleAdd(ir_node* node)
	{
		ir_node* left = get_irn_n(node, 0);
		ir_node* right = get_irn_n(node, 1);

		// Both arguments are constants.
		if (is_Const(left) && is_Const(right))
		{
			updateTarvalAndExchange(node, new_r_Const_long(irg, mode_Is,
			                        get_tarval_long(computed_value(left)) + get_tarval_long(computed_value(right))));
		}
		else
		{
			// Check whether at least one argument is 0, and if so,
			// apply the rule x + 0 = x (or 0 + x = x).

			if (is_Const(left) && get_tarval_long(computed_value(left)) == 0)
				updateTarvalAndExchange(node, right);

			if (is_Const(right) && get_tarval_long(computed_value(right)) == 0)
				updateTarvalAndExchange(node, left);
		}
	}

	void FirmNodeHandler::handleSub(ir_node* node)
	{
		ir_tarval* tarVal = computed_value(node);

		if (get_tarval_mode(tarVal) == mode_Is)
			updateTarvalAndExchange(node, new_r_Const_long(irg, mode_Is, get_tarval_long(tarVal)));
		else
		{
			// Check whether at least one argument is 0, and if so,
			// apply the rule x - 0 = x (or 0 - x = -x).
			ir_node* left = get_irn_n(node, 0);
			ir_node* right = get_irn_n(node, 1);

			if (is_Const(left) && get_tarval_long(computed_value(left)) == 0)
				updateTarvalAndExchange(node, new_r_Minus(get_nodes_block(node), right, mode_Is));

			if (is_Const(right) && get_tarval_long(computed_value(right)) == 0)
				updateTarvalAndExchange(node, left);
		}
	}

	void FirmNodeHandler::handleMul(ir_node* node)
	{
		ir_tarval* tarVal = computed_value(node);

		if (get_tarval_mode(tarVal) == mode_Is)
			updateTarvalAndExchange(node, new_r_Const_long(irg, mode_Is, get_tarval_long(tarVal)));
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
					updateTarvalAndExchange(node, new_r_Minus(get_nodes_block(node), right, mode_Is));
				else if (value == 0)
					updateTarvalAndExchange(node, new_r_Const_long(irg, mode_Is, 0));
				else if (value == 1)
					updateTarvalAndExchange(node, right);
			}

			// See above...
			if (is_Const(right))
			{
				long value = get_tarval_long(computed_value(right));

				if (value == -1)
					updateTarvalAndExchange(node, new_r_Minus(get_nodes_block(node), left, mode_Is));
				else if (value == 0)
					updateTarvalAndExchange(node, new_r_Const_long(irg, mode_Is, 0));
				else if (value == 1)
					updateTarvalAndExchange(node, left);
			}
		}
	}

	void FirmNodeHandler::handleDivAndMod(ir_node* node)
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

	void FirmNodeHandler::handleProj(ir_node* node)
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
				exchange(node, new_r_Jmp(get_nodes_block(child_node)));
			}

			if ((proj_num == pn_Cond_true  && relation == ir_relation::ir_relation_false)
			        || (proj_num == pn_Cond_false && relation == ir_relation::ir_relation_true))
			{
				// Exchange Proj nodes leading to dead blocks with bad blocks.
				// TODO: Remove bad nodes.
				exchange(node, new_r_Bad(irg, get_modeX()));
			}
		}
	}

	void FirmNodeHandler::handleCmp(ir_node* node)
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

	void FirmNodeHandler::handleConv(ir_node* node)
	{
		// This removes *some* unnecessary conversions, notably those
		// occuring during calls to System.out.println, but this should
		// probably be extended for more general useless conversions.
		ir_node* child = get_irn_n(node, 0);

		if (is_Conv(child))
		{
			ir_node* grand_child = get_irn_n(child, 0);

			if (get_irn_mode(node) == get_irn_mode(grand_child))
				exchange(node, grand_child);
		}
	}

	void FirmNodeHandler::handle(ir_node* node)
	{
		newNodes->clear();

		if (is_Const(node)) handleConst(node);
		// TODO: Fix segfaults
		// else if (is_Phi(node)) handlePhi(node);
		else if (is_Minus(node)) handleMinus(node);
		else if (is_Add(node)) handleAdd(node);
		else if (is_Sub(node)) handleSub(node);
		else if (is_Mul(node)) handleMul(node);
		else if (is_Div(node) || is_Mod(node)) handleDivAndMod(node);
		else if (is_Proj(node)) handleProj(node);
		else if (is_Cmp(node)) handleCmp(node);
		else if (is_Conv(node)) handleConv(node);
	}
}
