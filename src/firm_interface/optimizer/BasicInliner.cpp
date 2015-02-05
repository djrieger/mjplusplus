#include "BasicInliner.hpp"
#include "../Worklist.hpp"
#include "../FirmInterface.hpp"
#include "ConstantFolder.hpp"

namespace firm
{

	struct CopyNodeInfo
	{
		ir_graph* destIrg;
		ir_node* callNode;
	};

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
					inlinePureFunction(callNode, calleeIrg, Tarval(tar));
			}
			else
			{
				if (info.noMem)
					inlineSimpleFunction(callNode, calleeIrg);
			}
		}
	}

	static void checkForPhisCallback(ir_node* node, void* env)
	{
		if (is_Phi(node))
		{
			auto hasPhis = (unsigned int*)env;
			*hasPhis = true;
		}
	}

	static ir_node* irn_copy_into_irg(ir_node* node, ir_graph* irg)
	{
		ir_op*    op    = get_irn_op(node);
		ir_node*  block = op != op_Block ? get_nodes_block(node) : NULL;
		dbg_info* dbgi  = get_irn_dbg_info(node);
		ir_mode*  mode  = get_irn_mode(node);

		int       arity = get_irn_arity(node);
		ir_node** ins   = new ir_node*[arity - 1];

		for (int i = 1; i < arity; ++i)
			ins[i - 1] = get_irn_n(node, i);

		ir_node*  res   = new_ir_node(dbgi, irg, block, op, mode, arity, ins);

		/* copy the attributes */
		copy_node_attr(irg, node, res);

		return res;
	}

	static void copyNodesToNewGraphCallback(Node node, void* env)
	{
		// return;
		CopyNodeInfo* info = (CopyNodeInfo*)env;
		Node callNode = Node(info->callNode);
		ir_node* destBlock = get_nodes_block(callNode);
		ir_graph* irg = info->destIrg;
		ir_printf("Copying %F (%d) to main graph...\n", node, get_irn_node_nr(node));

		switch (node.getOpcode())
		{
			case iro_Block:
				break;

			case iro_Start:
				break;

			case iro_Proj:
				if (node.getMode() != mode_M && node.getMode() != mode_T
				        && get_irn_mode(get_Proj_pred(node)) == mode_T
				        && is_Start(get_Proj_pred(get_Proj_pred(node))))
				{
					// found argument
					ir_printf("Found argument %F (%d)\n", node, get_irn_node_nr(node));
					set_irn_link(node, get_Call_param(callNode, get_Proj_num(node)));
				}

				break;

			case iro_Add:
			case iro_Mul:
			case iro_Sub:
			{
				ir_node** inputs = new ir_node*[2];
				inputs[0] = (ir_node*)get_irn_link(node.getChild(0));
				inputs[1] = (ir_node*)get_irn_link(node.getChild(1));
				ir_node* newNode = new_ir_node(get_irn_dbg_info(node), irg, destBlock, get_irn_op(node), node.getMode(), 2, inputs);
				set_irn_link(node, newNode);
			}
			break;

			case iro_Minus:
			case iro_Conv:
			{
				ir_node* input = (ir_node*)get_irn_link(node.getChild(0));
				ir_node* newNode = new_ir_node(get_irn_dbg_info(node), irg, destBlock, get_irn_op(node), node.getMode(), 1, &input);
				set_irn_link(node, newNode);
			}
			break;

			case iro_Const:
			{
				// constants have to reside in a graph's start block
				ir_node* newNode =  new_ir_node(get_irn_dbg_info(node), irg, get_irg_start_block(irg), get_irn_op(node), node.getMode(), 0, NULL);
				set_Const_tarval(newNode, get_Const_tarval(node));
				set_irn_link(node, newNode);
			}
			break;

			case iro_Return:
				for (auto child : node.getChildren())
				{
					// get result child
					if (child.getMode() != mode_M)
					{

						for (auto outEdge : callNode.getOuts())
						{
							// M or T projection succeeding the call node
							Node succProj = outEdge.first;

							if (succProj.getMode() == mode_T)
							{
								// replace this projection's successor with new Const node for the return value
								Node grandparentProj = succProj.getOuts()[0].first;
								grandparentProj.replaceWith((ir_node*)get_irn_link(child));
							}
						}
					}
				}

				break;

			default:
				break;
		}
	}

	void BasicInliner::inlineSimpleFunction(Node callNode, ir_graph* calleeIrg)
	{
		ir_printf("Got %F (%d) to simple function %F\n", callNode, get_irn_node_nr(callNode), calleeIrg);
		edges_activate(calleeIrg);

		// make sure we don't have phis
		bool hasPhis = false;
		Worklist::walk_topological(calleeIrg, &checkForPhisCallback, &hasPhis);

		if (!hasPhis)
		{
			ir_printf("Success, no phis\n");
			changed = true;

			ir_entity* calleeEntity = get_irg_entity(calleeIrg);
			ir_type* calleeType = get_entity_type(calleeEntity);
			int paramsCount = get_method_n_params(calleeType);
			ir_printf("Callee has %d params\n", paramsCount);

			for (int i = 0; i < paramsCount; i++)
			{
				ir_node* argument = get_Call_param(callNode, i);
				ir_type* parameterType = get_method_param_type(calleeType, i);
				ir_mode* parameterMode = get_type_mode(parameterType);

				ir_printf("arg %d = %F (%d), type %F, mode %F\n", i, argument, get_irn_node_nr(argument), parameterType, parameterMode);

			}

			CopyNodeInfo info;
			info.destIrg = irg;
			info.callNode = callNode;
			Worklist::walk_topological(calleeIrg, &copyNodesToNewGraphCallback, &info);

			for (auto outEdge : callNode.getOuts())
			{
				Node succProj = outEdge.first;

				// memory projection
				if (succProj.getMode() == mode_M)
				{
					// rewire each successor of the memory projection to the memory projection above the call node
					for (auto projChild : succProj.getOuts())
						projChild.first.setChild(projChild.second, get_Call_mem(callNode));
				}
			}

		}

		edges_deactivate(calleeIrg);
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
