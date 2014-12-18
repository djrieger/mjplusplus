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
		unsigned int predCount = node.getChildCount();
		bool isBad = false;
		bool valSet = false;
		long val = 0;
		unsigned int i = 0;
		auto nodeMode = node.getMode();
		ir_printf("processing %F (%d) at %p with %d predecessors and tarval %F\n", (ir_node*)node, get_irn_node_nr(node), (ir_node*)node, predCount, node.getTarval());

		while (i < predCount && !isBad)
		{
			Node pred = node.getChild(i);

			ir_printf("tarval of pred %F (%d)", pred, get_irn_node_nr(pred));

			Tarval curTarval = pred.getTarval();

			if (nodeMode == curTarval.getMode())
			{
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
				// mode of current predecessor does not match the mode of the node
			}

			++i;
		}

		ir_printf("%F:\t", node);

		if (isBad)
		{
			std::cout << "is bad" << std::endl;
			node.setTarval(BadTarval());
		}
		else if (valSet)
		{
			// we do have a change in the tarval of the current node, so update this accordingly
			Tarval newTarval(val, nodeMode);
			node.setTarval(newTarval);
			std::cout << "tarval has been updated" << std::endl;
		}
		else
			std::cout << "remains totally unchanged" << std::endl;
	}

	void ConstantFolder::updateTarvalForArithmeticNode(Node node)
	{
		Tarval tvLeft = node.getChild(0).getTarval();
		Tarval tvRight = node.getChild(1).getTarval();
		Tarval resultVal;

		switch (node.getOpcode())
		{
			case iro_Add:
				if (tvLeft.isNumeric() && tvRight.isNumeric()) resultVal = tarval_add(tvLeft, tvRight);

				break;

			case iro_Mul:
				// 0 * x = x * 0 = 0
				if (tvLeft.isNumWithVal(0) || tvRight.isNumWithVal(0)) resultVal = Tarval(0, node.getMode());
				else if (tvLeft.isNumeric() && tvRight.isNumeric()) resultVal = tarval_mul(tvLeft, tvRight);

				break;

			case iro_Minus:
				if (tvLeft.isNumeric()) resultVal = Tarval(- tvLeft.getLong(), node.getMode());

				break;

			case iro_Sub:
				if (tvLeft.isNumeric() && tvRight.isNumeric()) resultVal = tarval_sub(tvLeft, tvRight, NULL);

				break;

			case iro_Div:
			case iro_Mod:
				if (tvRight.isNumWithVal(0) // x / 0, x % 0 => undefined behavior, set to zero
				        || tvLeft.isNumWithVal(0) // 0 / x, 0 % x = 0
				        || (is_Mod(node) && (tvRight.isNumWithVal(1) || tvRight.isNumWithVal(-1)))) // x % 1 or -1 = 0
					resultVal = Tarval(0, node.getMode());
				// x / y, x % y
				else if (tvLeft.isNumeric() && tvRight.isNumeric())
				{
					if (is_Div(node))
						resultVal = tarval_div(tvLeft, tvRight);
					else
					{
						ir_tarval* remainder;
						tarval_divmod(tvLeft, tvRight, &remainder);
						resultVal = Tarval(remainder);
					}
				}

				break;

			default:
				throw "updateTarvalForArithmeticNode called on illegal node";
		}

		if (resultVal)
			node.setTarval(resultVal);
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

	void ConstantFolder::handleCmp(ir_node* node)
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

	void ConstantFolder::handle(Node node)
	{
		// we might have to add nodes to the queue again, therefore clear the container for them.
		newNodes->clear();

		// nodes without predecessors such as const or start nodes shouldn't need any treatment at all
		if (node.getChildCount() == 0)
			return;

		// preserve old Tarval for comparison later on
		Tarval oldTarval = node.getTarval();

		bool isBad = false;
		unsigned int numUnkowns = 0;

		// iterate over the predecessors of the node and prolong bad or unknown tarvals
		for (Node child : node.getChildren())
		{
			if (child.getTarval().isBad())
			{
				isBad = true;
				break;
			}
			else if (child.getTarval().isUnknown())
				numUnkowns++;
		}

		if (isBad)
			node.setTarval(BadTarval());
		else if (numUnkowns == Node(node).getChildCount())
			node.setTarval(UnknownTarval());
		else
		{
			// we now know that, depending on the node, we might be able to optimize something
			if (is_Phi(node) && node.isNumericOrBool())
				optimizePhi(node);
			else if (is_Add(node) || is_Sub(node) || is_Mul(node) || is_Div(node) || is_Mod(node))
				updateTarvalForArithmeticNode(node);
			else if (is_Minus(node))
			{
				Tarval childTarval = node.getChild(0).getTarval();

				if (childTarval.isNumeric())
					node.setTarval(Tarval(- childTarval.getLong(), node.getMode()));
			}
			else if (is_Proj(node))
			{
				auto child_node = node.getChild(0);

				if (!is_Start(child_node))
				{
					//ir_printf("Trying to get tarval of Proj node %F (%d) child %F, tarval %F\n", node, get_irn_node_nr(node), child_node, get_irn_link(child_node));
					node.setTarval(child_node.getTarval());
					//ir_printf("Proj node tarval after setting: %F\n", get_irn_link(node));*/
				}
			}
			else if (is_Conv(node))
			{
				Tarval newTarval(node.getChild(0).getTarval().getLong(), node.getMode());
				node.setTarval(newTarval);
			}
		}

		// TODO: fix this ->
		//else if (is_Cmp(node) && node.isNumericOrBool())
		//	handleCmp(node);

		// compare the preserved tarval from before the iteration
		// and the newly set tarval; if they differ: add successors to the queue again.
		if (node.getTarval() != oldTarval)
			markOutNodesAsNew(node);
	}

	void ConstantFolder::processChildren(Node node, std::function<void (Node leftChild, Node rightChild)> fun)
	{
		Node child1 = node.getChild(0);

		if (node.getChildCount() == 1 && node.getTarval().isModeIs())
			fun(child1, NULL);
		else
		{
			Node child2 = node.getChild(1);
			fun(child1, child2);
		}
	}

	bool ConstantFolder::tarvalIsZero(Tarval tarval)
	{
		return tarval && tarval.isNumeric() && tarval.getLong() == 0;
	}

	void ConstantFolder::replaceAdd(Node node)
	{
		processChildren(node, [&] (Node leftChild, Node rightChild) -> void
		{
			if (tarvalIsZero(leftChild.getTarval()))
				node.replaceWith(rightChild);
			else if (tarvalIsZero(rightChild.getTarval()))
				node.replaceWith(leftChild);

		});
	}

	void ConstantFolder::replaceSub(Node node)
	{
		processChildren(node, [&] (Node leftChild, Node rightChild) -> void
		{
			if (tarvalIsZero(leftChild.getTarval()))
				node.replaceWith(new_r_Minus(get_nodes_block(node), rightChild, get_irn_mode(node)));
			else if (tarvalIsZero(rightChild.getTarval()))
				node.replaceWith(leftChild);
		});
	}

	void ConstantFolder::replaceMinus(Node node)
	{
		if (is_Minus(node.getChild(0)))
			node.replaceWith(node.getChild(0).getChild(0));
	}

	void ConstantFolder::replaceMul(Node node)
	{
		processChildren(node, [&] (Node leftChild, Node rightChild) -> void
		{
			auto handleCases = [&] (Node leftChild, Node rightChild) -> void
			{
				if (leftChild.getTarval() && leftChild.getTarval().isNumeric())
				{
					switch (leftChild.getTarval().getLong())
					{
						case 0:
							node.replaceWith(new_r_Const_long(irg, get_irn_mode(node), 0));
							break;

						case 1:
							node.replaceWith(rightChild);
							break;

						case -1:
							node.replaceWith(new_r_Minus(get_nodes_block(node), rightChild, node.getMode()));
					}
				}
			};
			handleCases(leftChild, rightChild);
			handleCases(rightChild, leftChild);
		});
	}

	void ConstantFolder::replaceConv(Node node)
	{
		Node child = node.getChild(0);

		if (is_Conv(child) && node.getMode() == child.getMode())
			node.replaceWith(child);
		else if (is_Const(child))
			node.replaceWith(new_r_Const_long(irg, node.getMode(), child.getTarval().getLong()));
	}

	void ConstantFolder::replaceProj(Node node)
	{
		//TODOish: still necessary or already covered by replaceGeneric?
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

	bool ConstantFolder::replaceGeneric(Node node)
	{
		if (node.getTarval().isNumericOrBool())
		{
			ir_node* constNode = new_r_Const_long(irg, node.getMode(), node.getTarval().getLong());
			node.replaceWith(constNode, true);
			return true;
		}

		return false;
	}

	void ConstantFolder::cleanUp(Node node)
	{
		if (!replaceGeneric(node))
		{
			if (is_Add(node)) replaceAdd(node);
			else if (is_Mul(node)) replaceMul(node);
			else if (is_Sub(node)) replaceSub(node);
			else if (is_Minus(node)) replaceMinus(node);
			else if (is_Conv(node)) replaceConv(node);
			else if (is_Proj(node)) replaceProj(node);
		}

		// Todo: optimize booleans
		// Todo: optimize stuff like a + 0 not only for a's tarval (already implemented)
		// but also for a's value.
	}
}
