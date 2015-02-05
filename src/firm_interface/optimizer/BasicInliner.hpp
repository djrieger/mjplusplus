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
			bool canInline(ir_graph* calleeIrg, long* constReturnValue);
			void inlineFunction(Node callNode, ir_graph* calleeIrg);
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
