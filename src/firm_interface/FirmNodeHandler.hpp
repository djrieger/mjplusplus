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
		public:
			FirmNodeHandler(ir_graph* irg);
			shptr<std::set<ir_node*>> handle(ir_node*);
	};
}
#endif