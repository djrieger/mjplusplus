#ifndef FUNCTION_INLINER_HPP
#define FUNCTION_INLINER_HPP

#include <libfirm/firm.h>
#include <iostream>
#include "../Node.hpp"
#include "../../globals.hpp"

namespace firm
{
	class FunctionInliner
	{
		private:
			void recurse(Node node);
			void handle(Node node);
			void inlineFunction(Node callNode, ir_graph* calleeIrg);
		public:
			void run();
			// FunctionInliner();
	};
}

#endif