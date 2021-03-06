#include "ConstantFolder.hpp"
#include "../FirmInterface.hpp"

namespace firm
{
	ConstantFolder::ConstantFolder(ir_graph* irg): GraphHandler(irg)
	{

	}

	void ConstantFolder::optimizePhi(Node node)
	{
		//std::cout << "---------------------------" << std::endl;
		unsigned int predCount = node.getChildCount();
		bool isBad = false;
		bool valSet = false;
		long val = 0;
		unsigned int i = 0;
		auto nodeMode = node.getMode();
		// ir_printf("processing %F (%d) at %p with %d predecessors and tarval %F\n", (ir_node*)node, get_irn_node_nr(node), (ir_node*)node, predCount, node.getTarval());

		while (i < predCount && !isBad)
		{
			Node pred = node.getChild(i);

			// ir_printf("tarval of pred %F (%d)", pred, get_irn_node_nr(pred));

			Tarval curTarval = pred.getTarval();

			if (pred.getTarval().isBad())
			{
				isBad = true;
				continue;
			}

			if (nodeMode == curTarval.getMode())
			{
				auto cur = curTarval.getLong();

				if (valSet)
				{
					if (cur != val)
					{
						// ir_printf("'s value does not match the current value\n");
						// now we know, that we can not replace anything.
						// std::cout << "is bad" << std::endl;
						isBad = true;
					}

					// else
					// ir_printf("'s value does match the current value\n");
				}
				else
				{
					// ir_printf(": initial value set to %d\n", cur);
					// in first iteration: set const candidate.
					val = cur;
					valSet = true;
				}
			}

			++i;
		}

		//ir_printf("%F:\t", node);

		if (isBad)
			node.setTarval(BadTarval());
		else if (valSet)
		{
			// we do have a change in the tarval of the current node, so update this accordingly
			Tarval newTarval(val, nodeMode);
			node.setTarval(newTarval);
			// std::cout << "tarval has been updated to ";
			// ir_printf("%F\n", newTarval);
		}

		// else
		// std::cout << "remains totally unchanged" << std::endl;
	}

	bool ConstantFolder::updateTarvalForArithmeticNode(Node node)
	{
		Tarval tvLeft = node.getChild(0).getTarval();
		Tarval tvRight = node.getChild(1).getTarval();
		Tarval resultVal;

		switch (node.getOpcode())
		{
			case iro_Add:
				if (tvLeft.isNumeric() && tvRight.isNumeric())
					resultVal = tarval_add(tvLeft, tvRight);

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
				else if (node.getChild(0) == node.getChild(1)) resultVal = Tarval(0, node.getMode());

				break;

			case iro_Div:
			case iro_Mod:
			{
				// div and mod have divisor/dividend as child 1 and 2, child 0 is memory projection
				Tarval dividend = node.getChild(1).getTarval();
				Tarval divisor = node.getChild(2).getTarval();

				if (divisor.isNumWithVal(0) // x / 0, x % 0 => undefined behavior, set to zero
				        || dividend.isNumWithVal(0) // 0 / x, 0 % x = 0
				        || (is_Mod(node) && (divisor.isNumWithVal(1) || divisor.isNumWithVal(-1)))) // x % 1 or -1 = 0
					resultVal = Tarval(0, dividend.isNumeric() ? dividend.getMode() : divisor.getMode());
				// x / y, x % y
				else if (dividend.isNumeric() && divisor.isNumeric())
				{
					if (is_Div(node))
						resultVal = tarval_div(dividend, divisor);
					else
					{
						ir_tarval* remainder;
						tarval_divmod(dividend, divisor, &remainder);
						resultVal = Tarval(remainder);
					}
				}
				else if (node.getChild(1) == node.getChild(2))
				{
					// case for unknown tarvals: x / x = 1
					resultVal = Tarval(1, node.getChild(1).getMode());
				}

				break;
			}

			default:
				throw "updateTarvalForArithmeticNode called on illegal node";
		}

		if (resultVal)
		{
			node.setTarval(resultVal);
			return true;
		}
		else
			return false;
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
		bool changed = false;

		if (is_Phi(node) && node.isNumericOrBool())
		{
			optimizePhi(node);
			changed = true;
		}
		else if (is_Minus(node))
		{
			Tarval childTarval = node.getChild(0).getTarval();

			if (childTarval.isNumeric())
			{
				node.setTarval(Tarval(- childTarval.getLong(), node.getMode()));
				changed = true;
			}
		}
		else if (is_Proj(node))
		{
			auto child_node = node.getChild(0);
			node.setTarval(child_node.getTarval());
			changed = true;
		}
		else if (is_Conv(node))
		{
			Tarval childTarval = node.getChild(0).getTarval();

			if (childTarval.isNumeric())
			{
				Tarval newTarval(childTarval.getLong(), node.getMode());
				node.setTarval(newTarval);
				changed = true;
			}
		}
		else if (is_Add(node) || is_Sub(node) || is_Mul(node) || is_Div(node) || is_Mod(node))
			changed = updateTarvalForArithmeticNode(node);

		bool hasBadChildren = false;

		if (!changed)
		{
			for (Node child : node.getChildren())
			{
				if (child.getTarval().isBad())
				{
					hasBadChildren = true;
					break;
				}
			}

			if (hasBadChildren)
				node.setTarval(BadTarval());
		}

		if (node.getTarval() != oldTarval)
			markOutNodesAsNew(node);
	}

	void ConstantFolder::replaceDivMod(Node node)
	{
		if (node.getTarval().isNumeric())
			for (auto& ne : node.getOuts())
			{
				Node outChild = ne.first;

				if (outChild.getMode() == mode_M)
				{
					// Relink memory chain
					for (auto& e : outChild.getOuts())
						e.first.setChild(e.second, node.getChild(0));
				}
				else
					replaceNode(outChild, new_r_Const_long(irg, node.getTarval().getMode(), node.getTarval().getLong()));
			}
	}

	bool ConstantFolder::replaceGeneric(Node node)
	{
		if (!is_Return(node) && !is_Const(node) && !is_Div(node) && !is_Mod(node) && node.getTarval().isNumericOrBool() && node.getMode() != mode_M)
		{
			ir_node* constNode = new_r_Const_long(irg, node.getMode(), node.getTarval().getLong());
			replaceNode(node, constNode, true);
			return true;
		}

		return false;
	}

	void ConstantFolder::cleanUp(Node node)
	{
		// if (is_Return(node))
		// // {
		// 	ir_printf("-- %F (%d): ", node, get_irn_node_nr(node));
		// 	std::cout << node.getTarval().toString() << std::endl;
		// }

		if (!replaceGeneric(node))
		{
			if (is_Div(node) || is_Mod(node)) replaceDivMod(node);
		}

		// Todo: optimize booleans
		// Todo: optimize stuff like a + 0 not only for a's tarval (already implemented)
		// but also for a's value.
	}
}
