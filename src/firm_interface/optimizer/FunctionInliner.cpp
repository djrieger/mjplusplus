#include "FunctionInliner.hpp"

namespace firm
{

	void FunctionInliner::run()
	{
		for (size_t i = 0; i < get_irp_n_irgs(); i++)
		{
			ir_graph* irg = get_irp_irg(i);
			ir_printf("[Proc %F]\n", irg);
			edges_activate(irg);
			recurse(Node(get_irg_end_block(irg)));
			edges_deactivate(irg);
		}
	}

	void FunctionInliner::recurse(Node node)
	{
		if (is_Call(node))
		{
			ir_entity* callee = get_Call_callee(node);
			std::string calleeName(get_entity_name(callee));

			if (calleeName != "calloc" && calleeName != "_COut_Mprintln")
			{
				ir_graph* calleeIrg = get_entity_irg(callee);

				if (!calleeIrg)
					ir_printf("Callee uninitialized\n");
				else
					inlineFunction(node, calleeIrg);
			}
		}

		for (firm::Node child : node.getChildren())
			recurse(child);
	}

	void FunctionInliner::inlineFunction(Node callNode, ir_graph* calleeIrg)
	{
		ir_graph* callerIrg = get_irn_irg(callNode); // irg

		caller = NodeCloner(parentGraph, childGraph)
	}
}