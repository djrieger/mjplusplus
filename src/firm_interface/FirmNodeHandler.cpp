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
		int i = 0;
		bool onlyUnknowns = true;
		bool isSingleValue = true;

		int numFoundAt = -1;
		int numFound = -1;

		bool valueDetermined = false;

		std::cout << "phi has " << predCount << " predecessors" << std::endl;

		while (i < predCount)
		{
			ir_node* pred = get_irn_n(node, i);
			ir_tarval* curTarval = (ir_tarval*)get_irn_link(pred);

			if (curTarval == tarval_bad ||
			        // number with different value found
			        (numFoundAt >= 0 &&
			         get_tarval_mode(curTarval) == mode_Is &&
			         numFound != get_tarval_long(curTarval)))
			{
				std::cout << "case 1" << std::endl;
				// set tarval of phi to bad
				set_irn_link(node, (void*)tarval_bad);
				valueDetermined = true;

				if (numFound != get_tarval_long(curTarval))
					isSingleValue = false;

				break;
			}
			// handle only unknown values
			else if (curTarval != tarval_unknown)
				onlyUnknowns = false;

			// if current predecessor is a number, store index of predecessor and
			if (get_tarval_mode(curTarval) == mode_Is)
			{
				numFoundAt = i;
				numFound = get_tarval_long(curTarval);
				std::cout << "num found: " << numFound << std::endl;
				std::cout << "phi predecessor: ";
				ir_printf("%F\n", pred);
			}

			i++;
		}

		if (!valueDetermined)
		{
			// only unknown values found
			if (onlyUnknowns)
			{
				std::cout << "case 4" << std::endl;
				set_irn_link(node, (void*)tarval_unknown);
			}
			// n times same number found
			else if (isSingleValue && numFoundAt >= 0)
			{
				std::cout << "case 3" << std::endl;
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
	}

	void FirmNodeHandler::updateTarvalForArithmeticNode(ir_node* node)
	{
		ir_node* child1 = get_irn_n(node, 0);
		ir_node* child2 = get_irn_n(node, 1);
		ir_tarval* tarval1 = (ir_tarval*)get_irn_link(child1);
		ir_tarval* tarval2 = (ir_tarval*)get_irn_link(child2);

		if (get_tarval_mode(tarval1) == mode_Is && get_tarval_mode(tarval2) == mode_Is)
		{
			ir_tarval* resultVal;

			if (is_Add(node)) resultVal = tarval_add(tarval1, tarval2);
			else if (is_Sub(node)) resultVal = tarval_sub(tarval1, tarval2, NULL);
			else if (is_Mul(node)) resultVal = tarval_mul(tarval1, tarval2);
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
		updateTarvalForArithmeticNode(oldNode);
		exchange(oldNode, newNode);
	}


	shptr<std::set<ir_node*>> FirmNodeHandler::handle(ir_node* node)
	{
		newNodes->clear();

		if (is_Const(node))
		{
			// set tarval of this const node as its irn_link value
			set_irn_link(node, (void*)get_Const_tarval(node));
		}
		else if (is_Phi(node))
			optimizePhi(node);
		else if (is_Minus(node))
		{
			ir_node* child = get_irn_n(node, 0);

			if (is_Const(child))
				exchange(node, new_r_Const_long(irg, mode_Is, -get_tarval_long(computed_value(child))));
		}
		else if (is_Add(node))
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
		else if (is_Sub(node))
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
					updateTarvalAndExchange(node, new_Minus(right, mode_Is));

				if (is_Const(right) && get_tarval_long(computed_value(right)) == 0)
					updateTarvalAndExchange(node, left);
			}
		}
		else if (is_Mul(node))
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
						updateTarvalAndExchange(node, new_Minus(right, mode_Is));
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
						updateTarvalAndExchange(node, new_Minus(left, mode_Is));
					else if (value == 0)
						updateTarvalAndExchange(node, new_r_Const_long(irg, mode_Is, 0));
					else if (value == 1)
						updateTarvalAndExchange(node, left);
				}
			}

		}
		else if (is_Div(node) || is_Mod(node))
		{
			// Get children of div node (operands)
			ir_node* dividend = get_irn_n(node, 1);
			ir_node* divisor = get_irn_n(node, 2);

			if (is_Const(dividend) && is_Const(divisor))
			{
				long divisorValue = get_tarval_long(computed_value(divisor));

				// Optimize if not dividing by zero, otherwise simply leave the original Div node alone
				if (divisorValue != 0)
				{
					for (auto& ne : FirmInterface::getInstance().getOuts(node))
					{
						ir_node* o = ne.first;

						if (get_irn_mode(o) == mode_M)
						{
							for (auto& e : FirmInterface::getInstance().getOuts(o))
								set_irn_n(e.first, e.second, get_irn_n(node, 0));
						}
						else
						{
							long dividendValue = get_tarval_long(computed_value(dividend));
							int32_t tarVal = is_Div(node) ? dividendValue / divisorValue : dividendValue % divisorValue;
							// TODO @Max Make div/mod and updateTarvalAndExchange work together
							exchange(o, new_r_Const_long(irg, mode_Is, tarVal));
						}
					}
				}
			}
		}
		else if (is_Proj(node))
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
		else if (is_Cmp(node))
		{

			ir_node* left = get_irn_n(node, 0);
			ir_node* right = get_irn_n(node, 1);

			if (is_Const(left) && is_Const(right))
			{

				long left_value = get_tarval_long(computed_value(left));
				long right_value = get_tarval_long(computed_value(right));

#define SET_RELATION(A, B) \
		        case ir_relation::ir_relation_ ## A : \
		 		do { \
		 			set_Cmp_relation(node, left_value B right_value ? ir_relation::ir_relation_true : ir_relation::ir_relation_false); \
		 		} while (0); \
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
		} // else if (is_Cmp ...


		return newNodes;
	}
}