#ifndef WORKLIST_HPP
#define WORKLIST_HPP

#include <libfirm/firm.h>
#include <queue>
#include <functional>
#include <set>
#include <unordered_map>
#include "../globals.hpp"
#include "GraphHandler.hpp"
#include "Node.hpp"

namespace firm
{
	class Worklist
	{
		private:
			ir_graph* functionGraph;
			std::queue<ir_node*> worklist;
			GraphHandler& handler;
			std::unordered_map<ir_node*, bool> isQueued;

			void cleanUp();
		public:
			Worklist(ir_graph* functionGraph, GraphHandler& handler);
			//returns whether optimization happend
			bool run();

			static void walk_topological(ir_graph* irg, std::function<void (ir_node*, void*)> walker, void* env);
	};
}

#endif
