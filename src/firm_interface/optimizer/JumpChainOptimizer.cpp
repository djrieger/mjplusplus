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

		/*if (is_Jmp(node))
		{
			ir_printf("Found jump %F (%d)\n", node, get_irn_node_nr(node));
			if (isOnlyNodeInBlock(node))
			{
				set_irn_link(node, node);
				// ir_printf("Optimizing %F (%d)\n", node, get_irn_node_nr(node));
				// Node jumpTarget = getJumpTarget(node);
				// int childIndex = 0;

				// for (auto &pred: jumpTarget.getChildren()) {
				// 	if (pred == node)
				// 		jumpTarget.setChild(childIndex, get_irn_n(get_nodes_block(node), 0));
				// 	childIndex++;
				// }
				// ir_node* block = get_nodes_block(node);
				// ir_node* badNode = new_r_Bad(irg, mode_X);
				// set_irn_n(block,0,badNode);
			}

		}*/

		if (is_Jmp(node))
		{
			ir_printf("observing %F (%d)\n", node, get_irn_node_nr(node));

			// if a jump node is alone in its block, it might be possible to remove it.
			if (isOnlyNodeInBlock(node))
			{
				Node jumpTarget = getJumpTarget(node);
				int num_pred_jmp_target = get_irn_arity(jumpTarget);

				// this is the easiest case: Jmp/Proj <- Jmp <- Block
				if (num_pred_jmp_target == 1)
				{
					set_irn_link(node, node);
					// it is possible to break something phi related, care for Simon_097.mj
				}
				else if (num_pred_jmp_target > 1 && !breaksPhi(node, jumpTarget))
				{
					// we must not optimize away jmp nodes that are necessary for phis
					// therefore, iterate and trace back.
					auto jmpPreds = getJumpPreds(node);
					bool isCand = true;

					for (int i = 0; i < num_pred_jmp_target; ++i)
					{
						Node jumpTargetPred = jumpTarget.getChild(i);

						if (jumpTargetPred != node && is_Jmp(jumpTargetPred))
						{
							if (haveSameCond(jmpPreds, jumpTargetPred))
							{
								isCand = false;
								break;
							}
						}
					}

					if (isCand) set_irn_link(node, node);

				}
			}


		}
	}

	bool JumpChainOptimizer::breaksPhi(Node jump, Node jumpTarget)
	{
		// loop heads usually have a keep alive edge.
		// collect loop heads,
		// also add the directly preceding block of the node 'jump' (see Simon_123.mj)
		ir_node* endBlock = get_irg_end(irg);
		std::vector<Node> loop_heads;

		for (int i = 0; i < get_irn_arity(endBlock); ++i)
		{
			ir_node* pred = get_irn_n(endBlock, i);

			if (is_Block(pred)) loop_heads.push_back(Node(pred));
		}

		for (auto& x : getJumpPreds(jump))
			loop_heads.push_back(get_nodes_block(x));

		// collect phi predecessors of phis in block 'jumpTarget'
		std::vector<ir_node*> phi_preds;
		ir_node* phi = get_Block_phis(jumpTarget);

		while (phi)
		{
			for (int i = 0; i < get_irn_arity(phi); ++i)
			{
				ir_node* pred = get_irn_n(phi, i);

				if (get_nodes_block(pred) != jumpTarget)
					phi_preds.push_back(get_nodes_block(pred));
			}

			phi = get_Phi_next(phi);
		}

		// now intersect loop heads and phi predecessors
		for (auto& x : loop_heads)
		{
			for (auto& y : phi_preds)
			{
				if (x == y)
					return true;
			}
		}

		return false;

		// see the graph of Simon_097.mj for what you really have to do
		// but the following code doesn't work
		/*bool ret = false;
		ir_node* phi = get_Block_phis(jumpTarget);
		Node trace = get_nodes_block(jump);
		if (phi) {
			do {
				while (!ret && trace != get_irg_start(irg)) {
					for (int i = 0; i < get_irn_arity(phi);++i) {
						ir_node* phiPredBlock = get_nodes_block(get_irn_n(phi,i));
						if (trace == phiPredBlock) {
							ret = true;
							break;
						}
						trace = get_nodes_block(get_irn_n(trace,0));
					}
				}
				trace = jump;
				phi = get_Phi_next(phi);
			} while (!ret && phi);
		}
		return ret;*/
	}

	std::vector<Node> JumpChainOptimizer::getJumpPreds(Node jumpNode)
	{
		return Node(get_nodes_block(jumpNode)).getChildren();
	}

	Node JumpChainOptimizer::getJumpTarget(Node jumpNode)
	{
		assert(is_Jmp(jumpNode));

		auto res = FirmInterface::getInstance().getOuts(jumpNode);
		assert(res.size() == 1);

		/*		ir_node* block = res[0].first;
				ir_node* phi = get_Block_phis(block);
				if (phi) {
					ir_printf("%F (%d) has at least one phi node: %F (%d)\n",block,get_irn_node_nr(block),phi,get_irn_node_nr(phi));
				}*/

		return Node(res[0].first);
	}

	bool JumpChainOptimizer::isOnlyNodeInBlock(Node node)
	{
		Node block = Node(get_nodes_block(node));
		return FirmInterface::getInstance().getOuts(block).size() == 1;
		/*
		ir_node* block = get_nodes_block(node);

		// check if we have any bad predecessors
		for (auto &child: Node(block).getChildren())
			if (is_Bad(child))
				return false;

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

			if (jumpTarget.getChildren().size() > 1) {
				for (auto &pred: jumpTarget.getChildren()) {
					if (pred != node && !is_Bad(pred)) {
						ir_printf("pred = %F (%d)\n", pred, get_irn_node_nr(pred));
						ir_node* predBlock = get_nodes_block(pred);
						ir_printf("trying to get child 0 of block %F (%d)\n", predBlock, get_irn_node_nr(predBlock));
						if (get_irn_arity(predBlock) > 0) {
							proj = get_irn_n(predBlock, 0);
							if (is_Proj(proj) && is_Proj(blockPred) && get_irn_n(proj, 0) == get_irn_n(blockPred, 0))
								return false;
						}
					}
				}
				return true;
			}
			else
				return true;
		}
		return false;
		*/
	}

	Node JumpChainOptimizer::traceJumpChain(Node node)
	{
		if (!is_Jmp(node))
			return node;
		else if (isOnlyNodeInBlock(node))
		{
			Node block = Node(get_nodes_block(node));
			// assume a Jump chain, therefore the block has only one child
			Node child = block.getChild(0);

			if (is_Jmp(child))
				return traceJumpChain(child);
			else
				return child;
		}

		return node;
	}

	bool JumpChainOptimizer::haveSameCond(std::vector<Node> jumpPredecessors, Node otherJumpTargetPredecessor)
	{
		bool ret = false;

		if (isOnlyNodeInBlock(otherJumpTargetPredecessor))
		{
			Node block = Node(get_nodes_block(otherJumpTargetPredecessor));
			auto otherJumpPredecessors = block.getChildren();

			for (auto& jp : jumpPredecessors)
			{
				Node a = traceJumpChain(jp);
				ir_printf("comparing %F (%d) against: ", a, get_irn_node_nr(a));

				if (is_Proj(a))
				{
					for (auto& ojp : otherJumpPredecessors)
					{
						Node b = traceJumpChain(ojp);
						ir_printf("%F (%d),  ", b, get_irn_node_nr(b));

						if (is_Proj(b) && a.getChild(0) == b.getChild(0))
						{
							//ir_printf("%F (%d) and %F (%d) belong to the same cond\n",a,get_irn_node_nr(a),b,get_irn_node_nr(b));
							ret = true;
							break;
						}
					}
				}

				printf("\n");

				if (ret) break;
			}
		}

		return ret;
	}

	void JumpChainOptimizer::cleanUp(Node node)
	{
		if (is_Jmp(node) && (ir_node*)get_irn_link(node) == node)
		{
			ir_printf("Jmp opt candidate %F (%d):\t", node, get_irn_node_nr(node));
			auto jmpPreds = getJumpPreds(node);

			if (jmpPreds.size() == 1)
			{
				Node jumpTarget = getJumpTarget(node);
				int childIndex = 0;

				for (auto& pred : jumpTarget.getChildren())
				{
					if (pred == node)
						jumpTarget.setChild(childIndex, jmpPreds[0]);

					childIndex++;
				}

				ir_node* block = get_nodes_block(node);
				ir_node* badNode = new_r_Bad(irg, mode_X);
				set_irn_n(block, 0, badNode);
				changed = true;
			}
			else
				printf("has %zu preds", jmpPreds.size());

			printf("\n");
		}
	}

}