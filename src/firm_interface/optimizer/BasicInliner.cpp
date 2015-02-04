#include "BasicInliner.hpp"
#include "../Worklist.hpp"
#include "../FirmInterface.hpp"
#include "ConstantFolder.hpp"

namespace firm
{
	BasicInliner::BasicInliner(ir_graph* irg): GraphHandler(irg)
	{
		ir_printf("[Proc %F]\n", irg);
		// irg_verify(irg);
		// FirmInterface::getInstance().outputFirmGraph(irg, "INLINING");
	}

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
			ir_func countReturnNodes = [](ir_node * node, void* env)
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
			Worklist::walk_topological(calleeIrg, countReturnNodes, &returnNodesCount);
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