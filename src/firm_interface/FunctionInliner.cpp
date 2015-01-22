#include "FunctionInliner.hpp"

namespace firm
{
	FunctionInliner::FunctionInliner(ir_graph* irg): GraphOptimizer(irg)
	{

	}

	void FunctionInliner::handle(Node node)
	{
		if (is_Call(node)) {
			ir_printf("Call node with %d params, ", get_Call_n_params(node));
			ir_entity* callee = get_Call_callee(node);
			ir_printf("callee: %F %s\n", callee, get_entity_name(callee));
		}
	}

	void FunctionInliner::cleanUp(Node node)
	{
	}

}