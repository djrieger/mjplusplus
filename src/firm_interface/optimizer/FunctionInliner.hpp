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
			void relaxGraphStates(ir_graph* callerIrg, ir_graph* calleeIrg);
			void copy_parameter_entities(ir_node *call, ir_graph *called_graph);
			void copy_frame_entities(ir_graph *from, ir_graph *to);
			void copy_node_inline(ir_node *node, void *env);
			void set_preds_inline(ir_node *node, void *env);
			ir_node *irn_copy_into_irg(const ir_node *node, ir_graph *irg);
			void irn_rewire_inputs(ir_node *node);
			bool inlineFunction(Node callNode, ir_graph* calleeIrg);
		public:
			void run();
			// FunctionInliner();
	};
}

#endif