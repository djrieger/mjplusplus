#ifndef FIRM_NODE_HANDLER_HPP
#define FIRM_NODE_HANDLER_HPP

#include <libfirm/firm.h>
#include <set>
#include <queue>
#include <iostream>
#include "../globals.hpp"

namespace firm
{
	class FirmNodeHandler
	{
		private:
			ir_graph* irg;
			shptr<std::set<ir_node*>> newNodes;

			void optimizePhi(ir_node* node);
			void updateTarvalForArithmeticNode(ir_node* node);
			void updateTarvalAndExchange(ir_node* oldNode, ir_node* newNode);
			void updateTarvalAndExchangeMemory(ir_node* oldNode, ir_node* newNode);

			void handleConst(ir_node* node);
			void handlePhi(ir_node* node);
			void handleMinus(ir_node* node);
			void handleAdd(ir_node* node);
			void handleSub(ir_node* node);
			void handleMul(ir_node* node);
			void handleDivAndMod(ir_node* node);
			void handleProj(ir_node* node);
			void handleCmp(ir_node* node);
			void handleConv(ir_node* node);
		public:
			FirmNodeHandler(ir_graph* irg);
			void handle(ir_node*);
			shptr<std::set<ir_node*>> getNewNodes() const;
	};
}
#endif
