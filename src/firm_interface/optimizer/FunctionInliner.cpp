#include "FunctionInliner.hpp"

namespace firm
{
	void FunctionInliner::run()
	{
		for (size_t i = 0; i < get_irp_n_irgs(); i++)
		{
			ir_graph* irg = get_irp_irg(i);
			ir_printf("Proc %F:\n", irg);
			edges_activate(irg);
			recurse(Node(get_irg_end_block(irg)));
			edges_deactivate(irg);
		}
	}

	void FunctionInliner::recurse(Node node)
	{
		// see https://github.com/MatzeB/libfirm/blob/master/ir/opt/opt_inline.c#L326

		// ir_printf("%F %d\n", node, get_irn_arity(node));
		if (is_Call(node)) {
			ir_entity* callee = get_Call_callee(node);
			std::string calleeName(get_entity_name(callee));
			if (calleeName != "calloc" && calleeName != "_COut_Mprintln") {
				ir_printf("  Inlining callee %s (has %d argument(s))...\n", calleeName.c_str(), get_Call_n_params(node));

				ir_graph* calleeIrg = get_entity_irg(callee);
				if (!calleeIrg)
					ir_printf("Callee uninitialized\n");
				else {
					// int numCallers = get_irg_n_callers(calleeIrg);
					// ir_printf("called %d times\n", numCallers);
					// ir_node* endBlock = get_irg_end_block(calleeIrg);
					// ir_printf("|||| %F (%d)\n", endBlock, get_irn_node_nr(endBlock));
					// ir_node* retNode = get_irn_n(endBlock, 0);
					// ir_printf("%F\n", retNode);
					inlineFunction(node, calleeIrg);
				}
			}
		}

		for (firm::Node child: node.getChildren())
		{
			recurse(child);
		}
	}

	void FunctionInliner::inlineFunction(Node callNode, ir_graph* calleeIrg)
	{

	}
}