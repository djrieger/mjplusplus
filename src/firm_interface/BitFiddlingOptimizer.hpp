#ifndef BITFIDDLINGOPTIMIZER_HPP
#define BITFIDDLINGOPTIMIZER_HPP


#include "../globals.hpp"
#include "GraphOptimizer.hpp"
#include "Node.hpp"
#include <libfirm/firm.h>


namespace firm
{
	class BitFiddlingOptimizer : public GraphOptimizer
	{
		public:
			virtual void handle(Node node);
			virtual void cleanUp(Node node);
			BitFiddlingOptimizer(ir_graph* irg);
	};
}


#endif
