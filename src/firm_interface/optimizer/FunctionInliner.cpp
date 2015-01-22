#include "FunctionInliner.hpp"

namespace firm
{
	FunctionInliner::FunctionInliner(ir_graph* irg): GraphHandler(irg)
	{

	}

	void FunctionInliner::handle(Node node)
	{
		if (is_Call(node)) {
			ir_entity* callee = get_Call_callee(node);
			std::string calleeName(get_entity_name(callee));
			if (calleeName != "calloc" && calleeName != "_COut_Mprintln") {
				ir_printf("Call node with %d params, ", get_Call_n_params(node));
				ir_printf("callee: %F %s\n", callee, get_entity_name(callee));
			}
		}
	}

	void FunctionInliner::cleanUp(Node node)
	{
	}

}