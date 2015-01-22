#ifndef FUNCTION_INLINER_HPP
#define FUNCTION_INLINER_HPP

#include <libfirm/firm.h>
#include <iostream>
#include "../globals.hpp"
#include "GraphOptimizer.hpp"

namespace firm
{
	class FunctionInliner : public GraphOptimizer
	{
		public:
			virtual void handle(Node node);
			virtual void cleanUp(Node node);
			FunctionInliner(ir_graph* irg);
	};
}

#endif