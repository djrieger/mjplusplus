#ifndef CONTROLFLOWOPTIMIZER_H
#define CONTROLFLOWOPTIMIZER_H

#include <libfirm/firm.h>
#include <set>
#include <queue>
#include <iostream>
#include "../globals.hpp"
#include "GraphOptimizer.hpp"
#include "Node.hpp"

namespace firm
{
	class ControlFlowOptimizer : public GraphOptimizer
	{
		public:
			virtual void handle(Node node);
			virtual void cleanUp(Node node);
			ControlFlowOptimizer(ir_graph* irg);
	};
}

#endif /* CONTROLFLOWOPTIMIZER_H */