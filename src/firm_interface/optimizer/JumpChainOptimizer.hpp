#ifndef JUMP_CHAIN_OPTIMIZER_HPP
#define JUMP_CHAIN_OPTIMIZER_HPP

#include "../GraphHandler.hpp"

namespace firm
{
	class JumpChainOptimizer: public GraphHandler
	{
		private:
			Node getJumpTarget(Node jumpNode);
			bool isOnlyNodeInBlock(ir_node* node);
		public:
			JumpChainOptimizer(ir_graph* irg);
			virtual void handle(Node node);
			virtual void cleanUp(Node node);
	};
}
#endif