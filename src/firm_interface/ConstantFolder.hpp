#ifndef CONSTANT_FOLDER
#define CONSTANT_FOLDER

#include <libfirm/firm.h>
#include <set>
#include <queue>
#include <iostream>
#include "../globals.hpp"
#include "GraphOptimizer.hpp"
#include "Node.hpp"

namespace firm
{
	class ConstantFolder : public GraphOptimizer
	{
		protected:
			ir_graph* irg;

			void optimizePhi(Node node);
			void updateTarvalForArithmeticNode(Node node);
			void updateTarvalAndExchangeMemory(ir_node* oldNode, ir_node* newNode);

			void handlePhi(ir_node* node);
			void handleMinus(ir_node* node);
			void handleMul(ir_node* node);
			void handleDivAndMod(ir_node* node);
			void handleProj(ir_node* node);
			void handleCmp(ir_node* node);
			void handleConv(ir_node* node);
		public:
			virtual void handle(ir_node*);
			ConstantFolder(ir_graph* irg);
	};
}
#endif
