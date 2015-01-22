#ifndef BITFIDDLINGOPTIMIZER_HPP
#define BITFIDDLINGOPTIMIZER_HPP


#include "../../globals.hpp"
#include "../GraphHandler.hpp"
#include "../Node.hpp"
#include <libfirm/firm.h>


namespace firm
{
	class BitFiddlingOptimizer : public GraphHandler
	{
		public:
			virtual void handle(Node node);
			virtual void cleanUp(Node node);
			BitFiddlingOptimizer(ir_graph* irg);
	};
}


#endif
