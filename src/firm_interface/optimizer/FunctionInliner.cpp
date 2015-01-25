#include <cassert>
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
		// see https://github.com/MatzeB/libfirm/blob/master/ir/opt/opt_inline.c#L326

		// ir_printf("%F %d\n", node, get_irn_arity(node));
		if (is_Call(node))
		{
			ir_entity* callee = get_Call_callee(node);
			std::string calleeName(get_entity_name(callee));

			if (calleeName != "calloc" && calleeName != "_COut_Mprintln")
			{
				// ir_printf("  Inlining callee %s (has %d argument(s))...\n", calleeName.c_str(), get_Call_n_params(node));

				ir_graph* calleeIrg = get_entity_irg(callee);

				if (!calleeIrg)
					ir_printf("Callee uninitialized\n");
				else
				{
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

		for (firm::Node child : node.getChildren())
			recurse(child);
	}

	void FunctionInliner::relaxGraphStates(ir_graph* callerIrg, ir_graph* calleeIrg)
	{
		assert(get_irg_pinned(callerIrg) == op_pin_state_pinned);
		assert(get_irg_pinned(calleeIrg) == op_pin_state_pinned);
		clear_irg_properties(callerIrg, IR_GRAPH_PROPERTY_CONSISTENT_DOMINANCE | IR_GRAPH_PROPERTY_CONSISTENT_ENTITY_USAGE);
		set_irg_callee_info_state(callerIrg, irg_callee_info_inconsistent);
		clear_irg_properties(callerIrg, IR_GRAPH_PROPERTY_CONSISTENT_ENTITY_USAGE);
		edges_deactivate(callerIrg);
	}

	bool FunctionInliner::inlineFunction(Node callNode, ir_graph* calleeIrg)
	{
		ir_graph* callerIrg = get_irn_irg(callNode); // irg

		if (calleeIrg == callerIrg)
		{
			ir_printf("Cannot inline recursive call %+F (%+F) into %+F\n", callNode, calleeIrg, callerIrg);
			return false;
		}

		ir_printf("Inlining %+F (%+F) into %+F\n", callNode, calleeIrg, callerIrg);

		relaxGraphStates(callerIrg, calleeIrg);

		/* If the call has parameters, copy all parameter entities */
		ir_entity *calleeEntity = get_irg_entity(calleeIrg); // ent
		ir_type *calleeType = get_entity_type(calleeEntity); // mtp
		int n_params = get_method_n_params(calleeType);
		if (n_params != 0) {
			// TODO: necessary? and if, for what exactly?
			//copy_parameter_entities(callNode, calleeIrg);
		}

		/* create the argument tuple */
		ir_node **args_in = new ir_node*[n_params]; // ALLOCAN(ir_node*, n_params);

		ir_node *callingBlock = get_nodes_block(callNode); // block, post_bl
		for (int i = n_params - 1; i >= 0; --i) {
			ir_node *argument = get_Call_param(callNode, i); // arg
			ir_type *parameterType = get_method_param_type(calleeType, i); // param_tp
			ir_mode *parameterMode = get_type_mode(parameterType); // mode

			// TODO: Can this really happen?
			if (parameterMode != get_irn_mode(argument)) {
				argument = new_r_Conv(callingBlock, argument, parameterMode);
			}
			args_in[i] = argument;
		}


		/* the procedure and later replaces the Start node of the called graph.
		 * Post_call is the old Call node and collects the results of the called
		 * graph. Both will end up being a tuple. */
		/* XxMxPxPxPxT of Start + parameter of Call */
		ir_node *in[pn_Start_max + 1];
		in[pn_Start_M]              = get_Call_mem(callNode);
		in[pn_Start_P_frame_base]   = get_irg_frame(callerIrg);
		in[pn_Start_T_args]         = new_r_Tuple(callingBlock, n_params, args_in);
		ir_node *pre_call = new_r_Tuple(callingBlock, pn_Start_max+1, in);

		// TODO: move further down?
		delete[] args_in;




		/* --
		   The new block gets the ins of the old block, pre_call and all its
		   predecessors and all Phi nodes. -- */
		part_block(pre_call);

		/* increment visited flag for later walk */
		inc_irg_visited(calleeIrg);

		/* link some nodes to nodes in the current graph so instead of copying
		 * the linked nodes will get used.
		 * So the copier will use the created Tuple instead of copying the start
		 * node, similar for singleton nodes like NoMem and Bad.
		 * Note: this will prohibit predecessors to be copied - only do it for
		 *       nodes without predecessors */
		ir_node *start_block = get_irg_start_block(calleeIrg);
		set_irn_link(start_block, get_nodes_block(pre_call));
		mark_irn_visited(start_block);

		ir_node *start = get_irg_start(calleeIrg);
		set_irn_link(start, pre_call);
		mark_irn_visited(start);

		ir_node *nomem = get_irg_no_mem(calleeIrg);
		set_irn_link(nomem, get_irg_no_mem(callerIrg));
		mark_irn_visited(nomem);

		/* copy entities and nodes */
		assert(!irn_visited(get_irg_end(calleeIrg)));
		copy_frame_entities(calleeIrg, callerIrg);
		irg_walk_core(get_irg_end(calleeIrg), &FunctionInliner::copy_node_inline, &FunctionInliner::set_preds_inline,
		              callerIrg);

		irp_free_resources(irp, IRP_RESOURCE_ENTITY_LINK);

		// continue with https://github.com/MatzeB/libfirm/blob/master/ir/opt/opt_inline.c#L444
	}

	/**
	 * copy all entities on the stack frame on 1 irg to the stack frame of another.
	 * Sets entity links of the old entities to the copies
	 */
	void FunctionInliner::copy_frame_entities(ir_graph *from, ir_graph *to)
	{
		ir_type *from_frame = get_irg_frame_type(from);
		ir_type *to_frame   = get_irg_frame_type(to);
		assert(from_frame != to_frame);
		for (size_t i = 0, n_members = get_class_n_members(from_frame);
		     i < n_members; ++i) {
			ir_entity *old_ent = get_class_member(from_frame, i);

			// parameter entities are already copied and the link has been set
			if (!is_parameter_entity(old_ent)) {
				ir_entity *new_ent = copy_entity_own(old_ent, to_frame);
				set_entity_link(old_ent, new_ent);
			}
		}
	}

	/**
	 * Copy node for inlining.  Updates attributes that change when
	 * inlining but not for dead node elimination.
	 *
	 * Copies the node by calling copy_node() and then updates the entity if
	 * it's a local one.  env must be a pointer of the frame type of the
	 * inlined procedure. The new entities must be in the link field of
	 * the entities.
	 */
	void copy_node_inline(ir_node *node, void *env)
	{
		ir_graph *new_irg  = (ir_graph*) env;
		ir_node  *new_node = irn_copy_into_irg(node, new_irg);

		set_irn_link(node, new_node);
		if (is_Member(node)) {
			ir_graph  *old_irg        = get_irn_irg(node);
			ir_type   *old_frame_type = get_irg_frame_type(old_irg);
			ir_entity *old_entity     = get_Member_entity(node);
			assert(is_Member(new_node));
			/* use copied entities from the new frame */
			if (get_entity_owner(old_entity) == old_frame_type) {
				ir_entity *new_entity = (ir_entity*)get_entity_link(old_entity);
				assert(new_entity != NULL);
				set_Member_entity(new_node, new_entity);
			}
		}
	}

	void set_preds_inline(ir_node *node, void *env)
	{
		irn_rewire_inputs(node);

		/* move constants into start block */
		ir_node *new_node = (ir_node*)get_irn_link(node);
		if (is_irn_start_block_placed(new_node)) {
			ir_graph *new_irg     = (ir_graph *) env;
			ir_node  *start_block = get_irg_start_block(new_irg);
			set_nodes_block(new_node, start_block);
		}
	}

	void FunctionInliner::irn_rewire_inputs(ir_node *node)
	{
		ir_node *new_node = (ir_node*)get_irn_link(node);
		if (!is_Block(node)) {
			ir_node *block     = get_nodes_block(node);
			ir_node *new_block = (ir_node*)get_irn_link(block);
			set_nodes_block(new_node, new_block);
		}

		foreach_irn_in(node, i, in) {
			ir_node *new_in = (ir_node*)get_irn_link(in);
			set_irn_n(new_node, i, new_in);
		}

		/* Now the new node is complete. We can add it to the hash table for CSE. */
		//add_identities(new_node);
	}

	ir_node *FunctionInliner::irn_copy_into_irg(const ir_node *node, ir_graph *irg)
	{
		ir_op    *op    = get_irn_op(node);
		ir_node  *block = op != op_Block ? get_nodes_block(node) : NULL;
		dbg_info *dbgi  = get_irn_dbg_info(node);
		ir_mode  *mode  = get_irn_mode(node);
		ir_node **ins   = get_irn_in(node) + 1;
		int       arity = get_irn_arity(node);
		ir_node  *res   = new_ir_node(dbgi, irg, block, op, mode, arity, ins);

		/* copy the attributes */
		copy_node_attr(irg, node, res);

		return res;
	}


	/* Copies parameter entities from the given called graph */
	// void FunctionInliner::copy_parameter_entities(ir_node *call, ir_graph *called_graph)
	// {
	// 	dbg_info *dbgi         = get_irn_dbg_info(call);
	// 	ir_graph *irg          = get_irn_irg(call);
	// 	ir_node  *frame        = get_irg_frame(irg);
	// 	ir_node  *block        = get_nodes_block(call);
	// 	ir_type  *called_frame = get_irg_frame_type(called_graph);
	// 	ir_type  *frame_type   = get_irg_frame_type(irg);
	// 	ir_node  *call_mem     = get_Call_mem(call);
	// 	ir_node **sync_mem     = NULL;

	// 	for (size_t i = 0, n_entities = get_class_n_members(called_frame);
	// 	     i < n_entities; ++i) {
	// 		ir_entity *old_entity = get_class_member(called_frame, i);
	// 		if (!is_parameter_entity(old_entity))
	// 			continue;

	// 		ir_type   *old_type    = get_entity_type(old_entity);
	// 		dbg_info  *entity_dbgi = get_entity_dbg_info(old_entity);
	// 		ident     *old_name    = get_entity_ident(old_entity);
	// 		ident     *name        = id_mangle3("", old_name, "$inlined");
	// 		ir_entity *new_ent     = new_entity(frame_type, name, old_type);
	// 		set_entity_dbg_info(new_ent, entity_dbgi);
	// 		set_entity_link(old_entity, new_ent);

	// 		size_t   n_param_pos = get_entity_parameter_number(old_entity);
	// 		ir_node *param       = get_Call_param(call, n_param_pos);
	// 		ir_node *sel         = new_rd_Member(dbgi, block, frame, new_ent);
	// 		ir_node *new_mem;
			
	// 			/* Store the parameter onto the frame */
	// 			ir_node *store = new_rd_Store(dbgi, block, call_mem, sel, param, old_type, cons_none);
	// 			new_mem = new_r_Proj(store, mode_M, pn_Store_M);

	// 		if (sync_mem) {
	// 			ARR_APP1(ir_node*, sync_mem, new_mem);
	// 		} else {
	// 			sync_mem = NEW_ARR_F(ir_node*, 1);
	// 			sync_mem[0] = new_mem;
	// 		}
	// 	}

	// 	if (sync_mem != NULL) {
	// 		int sync_arity = (int)ARR_LEN(sync_mem);
	// 		if (sync_arity > 1) {
	// 			ir_node *sync = new_r_Sync(block, sync_arity, sync_mem);
	// 			set_Call_mem(call, sync);
	// 		} else {
	// 			set_Call_mem(call, sync_mem[0]);
	// 		}
	// 		DEL_ARR_F(sync_mem);
	// 	}
	// }
}