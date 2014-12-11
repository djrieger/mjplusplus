#ifndef WORKLIST_HPP
#define WORKLIST_HPP

#include <libfirm/firm.h>
#include <queue>
#include <functional>
#include <set>
#include "../globals.hpp"
#include "FirmNodeHandler.hpp"

namespace firm
{
	class Worklist
	{
		private:
			ir_graph* functionGraph;
			std::queue<ir_node*> worklist;
			FirmNodeHandler& handler;
		public:
			Worklist(ir_graph* functionGraph, FirmNodeHandler& handler);
			void run();

	};
}

#endif