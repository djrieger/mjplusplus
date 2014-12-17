#include "ConstantFolder.hpp"
#include "FirmInterface.hpp"

namespace firm
{
	ConstantFolder::ConstantFolder(ir_graph* irg): GraphOptimizer(irg)
	{

	}

	void ConstantFolder::optimizePhi(Node node)
	{
		std::cout << "---------------------------" << std::endl;
		Tarval prevTarval = node.getTarval();
		unsigned int predCount = node.getChildCount();
		bool isConst = true;
		bool isBad = false;
		bool valSet = false;
		long val = 0;
		unsigned int i = 0;
		int numUnkowns = 0;
		ir_printf("processing %F (%d) at %p with %d predecessors and tarval %F\n", (ir_node*)node, get_irn_node_nr(node), (ir_node*)node, predCount, prevTarval);

		do
		{
			Node pred = node.getChild(i);

			ir_printf("tarval of pred %F (%d)", pred, get_irn_node_nr(pred));

			// we now know, that the node is not const and
			// therefore, operate on tarvals from the map
			Tarval curTarval = pred.getTarval();
			// to compare on previous tarval later on, set
			isConst = false;

			if (curTarval == tarval_unknown)
			{
				ir_printf(" is unknown\n");
				++numUnkowns;
				// just count the number of unknown values
			}
			else if (curTarval == tarval_bad)
			{
				ir_printf(" is bad\n");
				// if any input is bad, were bad aswell
				// no need to set isConst = false;
				isBad = true;
			}
			else if (curTarval.isModeIs())
			{
				// were not bad, were not unknown and were integer
				// compare the tarval with the current const val candidate
				auto cur = curTarval.getLong();

				if (valSet)
				{
					if (cur != val)
					{
						ir_printf("'s value does not match the current value\n");
						// now we know, that we can not replace anything.
						isBad = true;
					}
					else
						ir_printf("'s value does match the current value\n");
				}
				else
				{
					ir_printf(": initial value set to %d\n", cur);
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
			node.setTarval(BadTarval());
		}
		else if (valSet)
		{
			// get_tarval_mode(prevTarval) == mode_Is && get_tarval_long(prevTarval) != val
			// we do have a change in the tarval of the current node, so update this accordingly
			Tarval newTarval(val);
			node.setTarval(newTarval);
			std::cout << "tarval has been updated" << std::endl;
		}
		else
			std::cout << "remains totally unchanged" << std::endl;
	}

	void ConstantFolder::updateTarvalForArithmeticNode(Node node)
	{
		Node child1(node.getChild(0));
		Tarval tarval1 = child1.getTarval();

		if (!tarval1)
			tarval1 = Tarval(tarval_unknown);

		Node child2;
		Tarval tarval2;

		if (node.getChildCount() > 1)
		{
			child2 = node.getChild(1);
			tarval2 = child2.getTarval();

			if (!tarval2)
				tarval2 = Tarval(tarval_unknown);
		}

		if (tarval1.isNumeric() && (is_Minus(node) || tarval2.isNumeric()))
		{
			ir_tarval* resultVal;

			if (is_Add(node)) resultVal = tarval_add(tarval1, tarval2);
			else if (is_Sub(node)) resultVal = tarval_sub(tarval1, tarval2, NULL);
			else if (is_Mul(node)) resultVal = tarval_mul(tarval1, tarval2);
			else if (is_Minus(node)) resultVal = tarval_neg(tarval1);
			//else if (is_Div(node)) resultVal = tarval_div(tarval1, tarval2);
			else
				throw "updateTarvalForArithmeticNode called on illegal node";


			if (resultVal)
				set_irn_link(node, (void*) resultVal);

			if (is_Mul(node))
				ir_printf("Mul children 1: %F, 2: %F yields resultVal %F\n", tarval1, tarval2, resultVal);

			markOutNodesAsNew(node);
		}

		if (tarval1 == tarval_bad || (tarval2 && tarval2 == tarval_bad))
			set_irn_link(node, (void*) tarval_bad);
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

	void ConstantFolder::handleDivAndMod(ir_node* node)
	{
		// Get children of div node (operands)
		/*
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
			*/
	}

	void ConstantFolder::handleProj(ir_node* node)
	{
		// Get first child of proj node
		// TODO: Does Proj need to copy the tarval of a preceding Proj?
		/*ir_node* child_node = get_irn_n(node, 0);
		if (!is_Start(child_node)) {
		ir_printf("Trying to get tarval of Proj node %F (%d) child %F, tarval %F\n", node, get_irn_node_nr(node), child_node, get_irn_link(child_node));
		set_irn_link(node, get_irn_link(child_node));
		ir_printf("Proj node tarval after setting: %F\n", get_irn_link(node));*/

		/*
				ir_printf("===== %F with tar %F\n", node, Node(node).getTarval());

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
				}*/
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
		//ir_printf("///////////////////////// Conv node %d\n", get_irn_node_nr(node));
		set_irn_link(node, (void*)new_tarval_from_long(get_tarval_long((ir_tarval*)get_irn_link(get_irn_n(node, 0))), get_irn_mode(node)));
		//ir_printf("Conv tarval after setting: %F\n", get_irn_link(node));
	}

	void ConstantFolder::handle(ir_node* node)
	{
		newNodes->clear();

		if (Node(node).getChildCount() == 0)
			return;

		Tarval oldTarval = Node(node).getTarval();

		bool isBad = false;
		unsigned int numUnkowns = 0;


		for (Node child : Node(node).getChildren())
		{
			if (child.getTarval() == tarval_bad)
			{
				isBad = true;
				break;
			}
			else if (child.getTarval() == tarval_unknown)
				numUnkowns++;
		}

		if (isBad)
			set_irn_link(node, (void*)tarval_bad);
		else if (numUnkowns == Node(node).getChildCount())
			set_irn_link(node, (void*)tarval_unknown);
		else
		{
			if (is_Phi(node) && get_irn_mode(node) == mode_Is)
				optimizePhi(firm::Node(node));
			else if (is_Minus(node) || is_Add(node) || is_Sub(node) || is_Mul(node))
				updateTarvalForArithmeticNode(node);
			else if (is_Div(node) || is_Mod(node))
				handleDivAndMod(node);
			else if (is_Proj(node))
				handleProj(node);
			else if (is_Conv(node))
				handleConv(node);
		}

		if (Node(node).getTarval() != oldTarval)
			markOutNodesAsNew(node);

		//else if (is_Cmp(node))
		//	handleCmp(node);
	}
}
