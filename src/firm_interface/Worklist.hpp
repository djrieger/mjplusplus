#ifndef WORKLIST_HPP
#define WORKLIST_HPP

#include <libfirm/firm.h>
#include <queue>
#include <functional>
#include <set>
#include <unordered_map>
#include "../globals.hpp"
#include "GraphOptimizer.hpp"
#include "Node.hpp"

namespace firm
{
	class Worklist
	{
		private:
			ir_graph* functionGraph;
			std::queue<ir_node*> worklist;
			GraphOptimizer& handler;
			std::unordered_map<ir_node*, bool> isQueued;

			void cleanUp();
		public:
			Worklist(ir_graph* functionGraph, GraphOptimizer& handler);
			//returns whether optimization happend
			bool run();
	};
}

#endif
