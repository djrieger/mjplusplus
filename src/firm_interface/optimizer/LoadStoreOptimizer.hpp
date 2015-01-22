#ifndef LOAD_STORE_OPTIMIZER_HPP
#define LOAD_STORE_OPTIMIZER_HPP

#include <libfirm/firm.h>
#include "../../globals.hpp"
#include "../GraphHandler.hpp"
#include "../Node.hpp"

namespace firm
{
	class LoadStoreOptimizer : public GraphHandler
	{

		protected:
			void handleLoad(Node node);
		public:
			LoadStoreOptimizer(ir_graph* irg);
			virtual void handle(Node);
			virtual void cleanUp(Node node);
	};
}
#endif
