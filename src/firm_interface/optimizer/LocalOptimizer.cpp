#include "LocalOptimizer.hpp"
#include "../Tarval.hpp"

namespace firm
{
	LocalOptimizer::LocalOptimizer(ir_graph* irg) : GraphHandler(irg)
	{

	}

	void LocalOptimizer::handle(Node node)
	{

	}

	void LocalOptimizer::replaceAdd(Node node)
	{
		processChildren(node, [&] (Node leftChild, Node rightChild) -> void
		{
			if (leftChild.isConst() && tarvalIsZero(leftChild.getValue()))
				// x + 0 = x
				replaceNode(node, rightChild);
			else if (rightChild.isConst() && tarvalIsZero(rightChild.getValue()))
				// 0 + x = x
				replaceNode(node, leftChild);
			else if (is_Minus(leftChild) || is_Minus(rightChild))
			{
				// if one of the two operands is a minus, we probably can simplify the term
				unsigned int flag = is_Minus(leftChild) + 2 * is_Minus(rightChild);

				switch (flag)
				{
					case 0:
						break; // we will never land here because if the else if condition

					case 1:
						replaceNode(node, new_r_Sub(get_nodes_block(node), rightChild, leftChild.getChild(0), node.getMode()));
						break; // (-a) + b = (b-a)

					case 2:
						replaceNode(node, new_r_Sub(get_nodes_block(node), leftChild, rightChild.getChild(0), node.getMode()));
						break; // a +(-b) = a-b

					case 3:
						replaceNode(node, new_r_Minus(get_nodes_block(node),
						new_r_Add(get_nodes_block(node), leftChild.getChild(0), rightChild.getChild(0), node.getMode()),
						node.getMode()));
						break; // -a +(-b) = -(a+b)
				}
			}

		});
	}

	void LocalOptimizer::replaceSub(Node node)
	{
		processChildren(node, [&] (Node leftChild, Node rightChild) -> void
		{
			if (leftChild.isConst() && tarvalIsZero(leftChild.getValue()))
				// 0 - x = -x
				replaceNode(node, new_r_Minus(get_nodes_block(node), rightChild, get_irn_mode(node)));
			else if (rightChild.isConst() && tarvalIsZero(rightChild.getValue()))
				// x - 0 =  x
				replaceNode(node, leftChild);
			else if (is_Minus(leftChild) || is_Minus(rightChild))
			{
				// if one of the two operands is a minus, we probably can simplify the term
				unsigned int flag = is_Minus(leftChild) + 2 * is_Minus(rightChild);

				switch (flag)
				{
					case 0:
						break; // we will never land here because if the else if condition

					case 1:
						replaceNode(node, new_r_Minus(get_nodes_block(node),
						new_r_Add(get_nodes_block(node), leftChild.getChild(0), rightChild, node.getMode()),
						node.getMode()));
						break; // (-a) - b = -(a + b)  we are not sure if we want to do something here

					case 2:
						replaceNode(node, new_r_Add(get_nodes_block(node), leftChild, rightChild.getChild(0), node.getMode()));
						break; // a - (-b) = a + b

					case 3:
						replaceNode(node, new_r_Sub(get_nodes_block(node), rightChild.getChild(0), leftChild.getChild(0), node.getMode()));
						break; // -a -(-b) = -(a+b)
				}
			}
		});
	}

	void LocalOptimizer::replaceMinus(Node node)
	{
		if (is_Minus(node.getChild(0)))
			replaceNode(node, node.getChild(0).getChild(0));
	}

	void LocalOptimizer::replaceMul(Node node)
	{
		processChildren(node, [&] (Node leftChild, Node rightChild) -> void
		{
			auto handleCases = [&] (Node leftChild, Node rightChild) -> void
			{
				if (leftChild.isConst() && leftChild.getValue().isNumeric())
				{
					switch (leftChild.getValue().getLong())
					{
						case 0:
							replaceNode(node, new_r_Const_long(irg, get_irn_mode(node), 0));
							break;

						case 1:
							replaceNode(node, rightChild);
							break;

						case -1:
							replaceNode(node, new_r_Minus(get_nodes_block(node), rightChild, node.getMode()));
					}
				}
			};
			handleCases(leftChild, rightChild);
			handleCases(rightChild, leftChild);
		});
	}

	void LocalOptimizer::cleanUp(Node node)
	{
		if (is_Add(node)) replaceAdd(node);
		else if (is_Mul(node)) replaceMul(node);
		else if (is_Sub(node)) replaceSub(node);
		else if (is_Minus(node)) replaceMinus(node);
		else if (is_Div(node))
		{
			auto divisor = node.getChild(2);

			if (is_Const(divisor) && divisor.getValue().getLong() == 1)
			{
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
						outChild.replaceWith(node.getChild(1));
				}
			}
			else if (is_Const(divisor) && divisor.getValue().getLong() == -1)
			{
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
						outChild.replaceWith(new_r_Minus(get_nodes_block(node), node.getChild(1), node.getChild(1).getMode()));
				}
			}
		}
	}

} /* firm */