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

			irg_verify(irg);
			FirmInterface::getInstance().outputFirmGraph(irg, "INLINING");
		}
	}

	void BasicInliner::recurse(Node node)
	{
		// typedef void (*ir_func)(ir_node*, void*);
		auto walkNodes = [this](ir_node * node, void*)
		{
			// ir_printf("Visiting %F (%d)\n", node, get_irn_node_nr(node));
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
		};

		Worklist::walk_topological(get_irn_irg(node), walkNodes, NULL);
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
	//
	bool BasicInliner::canInline(Node callNode, ir_graph* calleeIrg, long* constReturnValue)
	{
		ir_printf("Checking if %F (%d) %s can be inlined...\n", callNode, get_irn_node_nr(callNode), get_entity_name(get_Call_callee(callNode)));
		// checks to perform:
		Tarval returnValue((ir_tarval*)get_irg_link(calleeIrg));

		//  has constant return value?
		if (tarval_is_constant(returnValue) && tarval_is_long(returnValue))
		{
			std::cout << "-> Constant return value " << returnValue.toString() << std::endl;

			typedef void (*ir_func)(ir_node*, void*);
			ir_func walkNodes = [](ir_node * node, void* env)
			{
				// ir_printf("Visiting %F (%d)\n", node, get_irn_node_nr(node));
				if (is_Return(node))
				{
					auto returnNodesCount = (unsigned int*)env;
					*returnNodesCount += 1;
				}
			};

			ir_func validateMemoryChain = [](ir_node * node, void* env)
			{
				if (is_Return(node))
				{
					ir_node* returnMemPred = get_Return_mem(node);
					auto valid = (bool*)env;

					if (!is_Proj(returnMemPred) || !is_Start(get_Proj_pred(returnMemPred)))
						*valid = false;
				}
			};

			unsigned int returnNodesCount = 0;
			Worklist::walk_topological(calleeIrg, walkNodes, &returnNodesCount);
			ir_printf("-> %d return nodes found\n", returnNodesCount);

			if (returnNodesCount == 1)
			{
				bool validMemoryChain = true;
				Worklist::walk_topological(calleeIrg, validateMemoryChain, &validMemoryChain);

				if (validMemoryChain)
				{
					ir_printf("-> valid memory chain found\n");
					*constReturnValue = returnValue.getLong();
					return true;
				}
			}
		}

		return false;
	}

	void BasicInliner::inlineFunction(Node callNode, ir_graph* calleeIrg)
	{
		ir_graph* callerIrg = get_irn_irg(callNode); // irg
		long returnValue;

		if (canInline(callNode, calleeIrg, &returnValue))
		{
			auto callOutEdges = callNode.getOuts();

			for (auto edge : callOutEdges)
			{
				// M or T projection succeeding the call node
				Node succProj = edge.first;
				ir_printf("%d: %F (%d)\n", edge.second, edge.first, get_irn_node_nr(edge.first));

				if (succProj.getMode() == mode_M)
				{
					for (auto projChild: succProj.getOuts())
					{
						projChild.first.setChild(projChild.second, get_Call_mem(callNode));
					}
					// memory projection found
					// rewire to child of child of old memory projection of Call node
					// Node oldMemoryProj = get_Call_mem(callNode);
					// ir_printf("old mem proj of call node: %F (%d)\n", oldMemoryProj, get_irn_node_nr(oldMemoryProj));
					// Node oldMemProjChild = get_Proj_pred(oldMemoryProj);
					// ir_printf("old mem proj child: %F (%d)\n", oldMemProjChild, get_irn_node_nr(oldMemProjChild));

					// set_Proj_pred(succProj, oldMemoryProj);
				}
				else if (succProj.getMode() == mode_T)
				{
					// return value projection found
					// replace with new Const node
					Node grandparentProj = succProj.getOuts()[0].first;
					ir_printf("grandparent %F (%d)\n", grandparentProj, get_irn_node_nr(grandparentProj));
					grandparentProj.replaceWith(new_r_Const_long(callerIrg, mode_Is, returnValue));
				}
			}
		}
	}
}
//   only one return node?
//   no memory nodes?
//   what else?
