#ifndef CONVHANDLER_H
#define CONVHANDLER_H
#include "GraphHandler.hpp"

namespace firm
{
	class ConvHandler : public GraphHandler
	{
		public:
			virtual void handle(Node node);
			virtual void cleanUp(Node node);
			ConvHandler(ir_graph* irg);
	};
}

#endif /* CONVHANDLER_H */