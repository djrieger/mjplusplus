#ifndef WORKLIST_HPP
#define WORKLIST_HPP

#include <libfirm/firm.h>
#include <queue>
#include <functional>
#include <set>
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
		public:
			Worklist(ir_graph* functionGraph, ConstantFolder& handler);
			void run();

	};
}

#endif