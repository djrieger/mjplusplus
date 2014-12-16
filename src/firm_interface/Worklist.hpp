#ifndef WORKLIST_HPP
#define WORKLIST_HPP

#include <libfirm/firm.h>
#include <queue>
#include <functional>
#include <set>
#include <unordered_map>
#include "../globals.hpp"
#include "ConstantFolder.hpp"

namespace firm
{
	class Worklist
	{
		private:
			ir_graph* functionGraph;
			std::queue<ir_node*> worklist;
			ConstantFolder& handler;
			std::unordered_map<ir_node*,bool> isQueued;

			void replacePhi(ir_node * node);
		public:
			Worklist(ir_graph* functionGraph, ConstantFolder& handler);
			void run();

	};
}

#endif