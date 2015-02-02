#ifndef JUMP_CHAIN_OPTIMIZER_HPP
#define JUMP_CHAIN_OPTIMIZER_HPP

#include "../GraphHandler.hpp"

namespace firm
{
	class JumpChainOptimizer: public GraphHandler
	{
		private:
			std::vector<Node> getJumpPreds(Node jumpNode);
			Node getJumpTarget(Node jumpNode);
			bool isOnlyNodeInBlock(Node node);
			bool haveSameCond(std::vector<Node> jumpPredecessors, Node otherJumpTargetPredecessor);
			Node traceJumpChain(Node node);
			bool breaksPhi(Node jump, Node jumpTarget);
		public:
			JumpChainOptimizer(ir_graph* irg);
			virtual void handle(Node node);
			virtual void cleanUp(Node node);
	};
}
#endif