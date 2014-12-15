#ifndef GRAPH_OPTIMIZER_HPP
#define GRAPH_OPTIMIZER_HPP

#include <libfirm/firm.h>
//#include "../globals.hpp"

namespace firm
{
	class GraphOptimizer
	{
		protected:
			ir_graph* irg;
		public:
			GraphOptimizer(ir_graph* irg);
			virtual void handle(ir_node*) = 0;
	};
}
#endif