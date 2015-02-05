#include "BasicInliner.hpp"
#include "../Worklist.hpp"
#include "../FirmInterface.hpp"
#include "ConstantFolder.hpp"

namespace firm
{
	BasicInliner::BasicInliner(ir_graph* irg): GraphHandler(irg)
	{
	}

	static void countReturnNodesCallback(ir_node* node, void* env)
	{
		if (is_Return(node))
		{
			auto returnNodesCount = (unsigned int*)env;
			*returnNodesCount += 1;
		}
	}

	static void validateMemoryChainAndGetReturnValueCallback(ir_node* node, void* env)
	{
		auto returnValue = (BasicInliner::inlineInfo*) env;

		if (is_Return(node))
		{
			ir_node* returnMemPred = get_Return_mem(node);

			if (get_Return_n_ress(node) > 0)
			{
				ir_node* returnChild = get_Return_res(node, 0);

				if (is_Const(returnChild))
					returnValue->tar = get_Const_tarval(returnChild);
			}
			else
				returnValue->tar = new_tarval_from_long(0, mode_Is);

			if (is_Proj(returnMemPred) && is_Start(get_Proj_pred(returnMemPred)))
				returnValue->noMem = true;
		}
	}

	void BasicInliner::tryInline(Node callNode, ir_graph* calleeIrg)
	{
		// ir_printf("[Proc %F] Checking if %F (%d) to %s can be inlined...\n", irg, callNode, get_irn_node_nr(callNode), get_entity_name(get_Call_callee(callNode)));

		unsigned int returnNodesCount = 0;
		Worklist::walk_topological(calleeIrg, &countReturnNodesCallback, &returnNodesCount);

		if (returnNodesCount == 1)
		{
			inlineInfo info;
			Worklist::walk_topological(calleeIrg, &validateMemoryChainAndGetReturnValueCallback, &info);
			ir_tarval* tar = info.tar;

			if (tarval_is_constant(tar))
			{
				// we have a valid memory chain
				if (info.noMem)
				{
					inlinePureFunction(callNode, calleeIrg, Tarval(tar));
				}
			}
		}
	}

	void BasicInliner::inlinePureFunction(Node callNode, ir_graph* calleeIrg, Tarval returnValue)
	{
		std::cerr << "Inlinining" << std::endl;
		// if (canInline(calleeIrg, &returnValue))
		{
			for (auto outEdge : callNode.getOuts())
			{
				// M or T projection succeeding the call node
				Node succProj = outEdge.first;

				// memory projection
				if (succProj.getMode() == mode_M)
				{
					// rewire each successor of the memory projection to the memory projection above the call node
					for (auto projChild : succProj.getOuts())
						projChild.first.setChild(projChild.second, get_Call_mem(callNode));
				}
				// projection for return value from call
				else if (succProj.getMode() == mode_T)
				{
					// replace this projection's successor with new Const node for the return value
					Node grandparentProj = succProj.getOuts()[0].first;
					replaceNode(grandparentProj, new_r_Const(irg, returnValue));
				}
			}
		}
	}

	void BasicInliner::cleanUp(Node )
	{

	}

	void BasicInliner::handle(Node node)
	{
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
					tryInline(node, calleeIrg);
			}
		}
	}
}
