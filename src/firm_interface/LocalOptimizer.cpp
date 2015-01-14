#include "LocalOptimizer.hpp"
#include "Tarval.hpp"

namespace firm
{
	LocalOptimizer::LocalOptimizer(ir_graph* irg) : GraphOptimizer(irg)
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
				node.replaceWith(rightChild);
			else if (rightChild.isConst() && tarvalIsZero(rightChild.getValue()))
				node.replaceWith(leftChild);

		});
	}

	void LocalOptimizer::replaceSub(Node node)
	{
		processChildren(node, [&] (Node leftChild, Node rightChild) -> void
		{
			if (leftChild.isConst() && tarvalIsZero(leftChild.getValue()))
				node.replaceWith(new_r_Minus(get_nodes_block(node), rightChild, get_irn_mode(node)));
			else if (rightChild.isConst() && tarvalIsZero(rightChild.getValue()))
				node.replaceWith(leftChild);
		});
	}

	void LocalOptimizer::replaceMinus(Node node)
	{
		if (is_Minus(node.getChild(0)))
			node.replaceWith(node.getChild(0).getChild(0));
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

	void LocalOptimizer::cleanUp(Node node)
	{
		if (is_Add(node)) replaceAdd(node);
		else if (is_Mul(node)) replaceMul(node);
		else if (is_Sub(node)) replaceSub(node);
		else if (is_Minus(node)) replaceMinus(node);
	}

} /* firm */