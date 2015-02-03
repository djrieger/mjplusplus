#ifndef BASIC_INLINER_HPP
#define BASIC_INLINER_HPP

#include <libfirm/firm.h>
#include <iostream>
#include "../Node.hpp"
#include "../../globals.hpp"

namespace firm
{
	class BasicInliner
	{
		private:
			void recurse(Node node);
			void handle(Node node);
			void inlineFunction(Node callNode, ir_graph* calleeIrg);
			// bool hasControlFlow(Node node);
		public:
			void run();
	};
}

#endif