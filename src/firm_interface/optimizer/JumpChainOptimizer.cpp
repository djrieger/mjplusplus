#include <cassert>
#include "JumpChainOptimizer.hpp"
#include "../FirmInterface.hpp"


namespace firm
{
	JumpChainOptimizer::JumpChainOptimizer(ir_graph* irg): GraphHandler(irg) {}

	void JumpChainOptimizer::handle(Node node)
	{
		// if (is_Block(node))
		// {
		// 	// ir_node* block = get_nodes_block(node);
		// 	auto res = FirmInterface::getInstance().getOuts(node);
		// 	if (res.size() > 0)
		// 	{
		// 		ir_printf("%F (%d) has %zu successors\n", node, get_irn_node_nr(node), res.size());
		// 		for (auto &x: res)
		// 			ir_printf("\t%F (%d)\n", x.first, get_irn_node_nr(x.first));
		// 	}
		// }

		if (is_Jmp(node))
		{
			ir_printf("Found jump %F (%d)\n", node, get_irn_node_nr(node));
			if (isOnlyNodeInBlock(node))
			{
				ir_printf("Optimizing %F (%d)\n", node, get_irn_node_nr(node));
				Node jumpTarget = getJumpTarget(node);
				int childIndex = 0;

				for (auto &pred: jumpTarget.getChildren()) {
					if (pred == node)
						jumpTarget.setChild(childIndex, get_irn_n(get_nodes_block(node), 0));
					childIndex++;
				}
				ir_node* block = get_nodes_block(node);
				ir_node* badNode = new_r_Bad(irg, mode_X);
				set_irn_n(block,0,badNode);
			}

			// ir_node* jumpPred = get_irn_n(node, 0);
			// ir_printf("%F (%d), pred %F (%d) \n", node, get_irn_node_nr(node), jumpPred, get_irn_node_nr(jumpPred));
		}
	}

	Node JumpChainOptimizer::getJumpTarget(Node jumpNode)
	{
		assert(is_Jmp(jumpNode));

		auto res = FirmInterface::getInstance().getOuts(jumpNode);
		assert(res.size() == 1);

		return Node(res[0].first);
	}

	bool JumpChainOptimizer::isOnlyNodeInBlock(ir_node* node)
	{
		ir_node* block = get_nodes_block(node);
		//ir_printf("Block of jump is %F (%d)\n", block, get_irn_node_nr(block));
		auto res = FirmInterface::getInstance().getOuts(block);
		//ir_printf("Block of jump is %F (%d) and has child %F (%d), %d children, 1st child == jmp? %d\n", 
		//	block, get_irn_node_nr(block), res[0].first, get_irn_node_nr(res[0].first), res.size(), res[0].first == node);
		ir_printf("%F (%d) in %F (%d) with successors\n",node,get_irn_node_nr(node),block,get_irn_node_nr(block));
		for (auto &x:res) {
			ir_printf("%F (%d)\n",x.first,get_irn_node_nr(x.first));
		}
		if(res.size() == 1 && res[0].first == node)
		{
			Node jumpTarget = getJumpTarget(node);
			ir_printf("jumpTarget = %F (%d)\n", jumpTarget, get_irn_node_nr(jumpTarget));
			ir_node* proj;
			ir_node* blockPred = get_irn_n(block, 0);

			if (jumpTarget.getChildren().size() > 1)
				for (auto &pred: jumpTarget.getChildren()) {
					if (pred != node) {
						ir_printf("pred = %F (%d)\n", pred, get_irn_node_nr(pred));
						ir_node* predBlock = get_nodes_block(pred);
						ir_printf("trying to get child 0 of block %F (%d)\n", predBlock, get_irn_node_nr(predBlock));
						proj = get_irn_n(predBlock, 0);
						if (is_Proj(proj) && is_Proj(blockPred) && get_irn_n(proj, 0) == get_irn_n(blockPred, 0))
							return false;
					}
				}
			else
				return true;
		}
		return false;
	}

	void JumpChainOptimizer::cleanUp(Node node)
	{

	}

}