#ifndef LOCALOPTIMIZER_H
#define LOCALOPTIMIZER_H

#include "GraphOptimizer.hpp"

namespace firm
{
	class LocalOptimizer : public GraphOptimizer
	{
		private:
			void replaceAdd(Node node);
			void replaceSub(Node node);
			void replaceMul(Node node);
			void replaceMinus(Node node);
		public:
			void handle(Node node);
			void cleanUp(Node node);
			LocalOptimizer(ir_graph* irg);
	};
} /* firm */



#endif /* LOCALOPTIMIZER_H */