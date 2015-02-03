#include "BasicInliner.hpp"
#include "../Worklist.hpp"
#include "../FirmInterface.hpp"

namespace firm
{

	void BasicInliner::run()
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

	void BasicInliner::recurse(Node node)
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

	// bool BasicInliner::hasControlFlow(Node node)
	// {
	// 	ir_printf("Visiting %F (%d)\n", node, get_irn_node_nr(node));

	// 	if (is_Jmp(node) || is_Cond(node))
	// 		return true;

	// 	for (firm::Node child : node.getChildren())
	// 		if (hasControlFlow(child))
	// 			return true;

	// 	return false;
	// }

	void BasicInliner::inlineFunction(Node callNode, ir_graph* calleeIrg)
	{
		ir_graph* callerIrg = get_irn_irg(callNode); // irg

		ir_printf("Checking if %F (%d) %s can be inlined...\n", callNode, get_irn_node_nr(callNode), get_entity_name(get_Call_callee(callNode)));		
		// checks to perform:
		Tarval returnValue((ir_tarval*)get_irg_link(calleeIrg));
		//  has constant return value?
		if (tarval_is_constant(returnValue) && tarval_is_long(returnValue)) {
			std::cout << "-> Constant return value " << returnValue.toString() << std::endl;

			// no control flow? (jmp, cond nodes)
			// ir_printf("Control flow found: %d\n", hasControlFlow(get_irg_end_block(calleeIrg)));

			typedef void (*ir_func)(ir_node*, void*);
			ir_func walkNodes = [](ir_node * node, void* env)
			{
				// ir_printf("Visiting %F (%d)\n", node, get_irn_node_nr(node));
				if (is_Jmp(node) || is_Cond(node)) {
					auto ctrlFlowNodes = (unsigned int*)env;
					*ctrlFlowNodes += 1;
				}
			};

			unsigned int ctrlFlowNodes = 0;
			Worklist::walk_topological(calleeIrg, walkNodes, &ctrlFlowNodes);
			ir_printf("-> %d control flow nodes found\n", ctrlFlowNodes);
			if (ctrlFlowNodes == 0)
			{
				auto callOutEdges = callNode.getOuts();
				for (auto edge: callOutEdges) {
					Node parentProj = edge.first;
					ir_printf("%d: %F (%d)\n", edge.second, edge.first, get_irn_node_nr(edge.first));
					if (parentProj.getMode() == mode_M) {
						// memory projection found
						// rewire to child of child of old memory projection of Call node
						Node oldMemoryProj = get_Call_mem(callNode);
						ir_printf("old mem proj of call node: %F (%d)\n", oldMemoryProj, get_irn_node_nr(oldMemoryProj));
						Node oldMemProjChild = get_Proj_pred(oldMemoryProj);
						ir_printf("old mem proj child: %F (%d)\n", oldMemProjChild, get_irn_node_nr(oldMemProjChild));

						set_Proj_pred(parentProj, oldMemProjChild);
					} else if (parentProj.getMode() == mode_T) {
						// return value projection found
						// replace with new Const node
						Node grandparentProj = parentProj.getOuts()[0].first;
						ir_printf("grandparent %F (%d)\n", grandparentProj, get_irn_node_nr(grandparentProj));
						parentProj.replaceWith(new_r_Const_long(callerIrg, returnValue.getMode(), returnValue.getLong()));
					}
				}
			}

			FirmInterface::getInstance().outputFirmGraph(callerIrg, "INLINING");
		}
		//   only one return node?
		//   no memory nodes?
		//   what else?
	}
}