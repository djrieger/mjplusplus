#include "BasicInliner.hpp"
#include "../Worklist.hpp"
#include "../FirmInterface.hpp"
#include "ConstantFolder.hpp"

namespace firm
{
	BasicInliner::BasicInliner(ir_graph* irg): GraphHandler(irg)
	{
	}

	// else if (is_Return(node))
	// {
	// 	for (Node child : node.getChildren())
	// 	{
	// 		// ir_printf("// Child of return node: %F (%d) with mode %F\n", child, get_irn_node_nr(child), child.getMode());
	// 		// Ignore memory projections/nodes attached to return nodes
	// 		if (child.getMode() != mode_M && child.getTarval().isNumericOrBool())
	// 		{
	// 			// now that we have found a non-memory child node with a constant tarval,
	// 			// copy its tarval to this return node and abort the loop
	// 			node.setTarval(child.getTarval());
	// 			//ir_tarval* irgLink = (ir_tarval*)get_irg_link(irg);
	// 			// if (tarval_is_constant(irgLink) && tarval_is_long(irgLink))
	// 			// 	// link was already set from another Return node -> bad
	// 			// 	set_irg_link(irg, tarval_bad);
	// 			// else
	// 			set_irg_link(irg, (ir_tarval*)child.getTarval());
	// 			changed = true;
	// 			break;
	// 		}
	// 	}
	// }

	bool BasicInliner::canInline(Node callNode, ir_graph* calleeIrg, long* constReturnValue)
	{
		ir_printf("[Proc %F] Checking if %F (%d) to %s can be inlined...\n", irg, callNode, get_irn_node_nr(callNode), get_entity_name(get_Call_callee(callNode)));

		typedef void (*ir_func)(ir_node*, void*);
		ir_func countReturnNodes = [](ir_node * node, void* env)
		{
			// ir_printf("Visiting %F (%d)\n", node, get_irn_node_nr(node));
			if (is_Return(node))
			{
				auto returnNodesCount = (unsigned int*)env;
				*returnNodesCount += 1;
			}
		};

		// After walking with this function, env is a ir_tarval* with value
		//   tarval_bad			if the memory chain cannot be inlined
		//   constant tarval 	if the memory chain was valid and the return node had a single Const child
		ir_func validateMemoryChainAndGetReturnValue = [](ir_node * node, void* env)
		{
			auto returnValue = (ir_tarval**)env;

			if (is_Return(node))
			{
				ir_node* returnMemPred = get_Return_mem(node);

				if (is_Proj(returnMemPred) && is_Start(get_Proj_pred(returnMemPred)))
				{
					ir_node* returnChild = get_Return_res(node, 0);

					if (is_Const(returnChild))
						*returnValue = get_Const_tarval(returnChild);
				}
			}
		};

		unsigned int returnNodesCount = 0;
		Worklist::walk_topological(calleeIrg, countReturnNodes, &returnNodesCount);
		ir_printf("-> %d return nodes found\n", returnNodesCount);

		if (returnNodesCount == 1)
		{
			ir_tarval* tar = tarval_bad;
			Worklist::walk_topological(calleeIrg, validateMemoryChainAndGetReturnValue, &tar);

			if (tar != tarval_bad && tarval_is_long(tar))
			{
				ir_printf("-> valid memory chain found\n");
				*constReturnValue = Tarval(tar).getLong();
				return true;
			}
		}

		return false;
	}

	void BasicInliner::inlineFunction(Node callNode, ir_graph* calleeIrg)
	{
		long returnValue;

		if (canInline(callNode, calleeIrg, &returnValue))
		{
			for (auto edge : callNode.getOuts())
			{
				// M or T projection succeeding the call node
				Node succProj = edge.first;
				ir_printf("%d: %F (%d)\n", edge.second, edge.first, get_irn_node_nr(edge.first));

				if (succProj.getMode() == mode_M)
				{
					for (auto projChild : succProj.getOuts())
						projChild.first.setChild(projChild.second, get_Call_mem(callNode));
				}
				else if (succProj.getMode() == mode_T)
				{
					// return value projection found
					// replace with new Const node
					Node grandparentProj = succProj.getOuts()[0].first;
					replaceNode(grandparentProj, new_r_Const_long(irg, mode_Is, returnValue));
				}
			}
		}
	}

	void BasicInliner::cleanUp(Node )
	{

	}

	void BasicInliner::handle(Node node)
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
					throw "Callee uninitialized";
				else
					inlineFunction(node, calleeIrg);
			}
		}
	}
}
