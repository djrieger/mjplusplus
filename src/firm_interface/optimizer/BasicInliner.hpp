#ifndef BASIC_INLINER_HPP
#define BASIC_INLINER_HPP

#include <libfirm/firm.h>
#include <iostream>
#include "../GraphHandler.hpp"
#include "../Node.hpp"
#include "../../globals.hpp"

namespace firm
{
	class BasicInliner : public GraphHandler
	{
		private:
			void tryInline(Node callNode, ir_graph* calleeIrg);
			void inlinePureFunction(Node callNode, ir_graph* calleeIrg, Tarval returnValue);
		public:
			BasicInliner(ir_graph* irg);
			void run();
			virtual void cleanUp(Node node);
			virtual void handle(Node node);

			struct inlineInfo
			{
				ir_tarval* tar = tarval_bad;
				bool noMem = false;
			};
	};
}

#endif
