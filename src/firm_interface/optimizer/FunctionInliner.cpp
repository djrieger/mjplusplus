#include <cassert>
#include <libfirm/adt/array.h>
#include "FunctionInliner.hpp"

/**
 * Returns size of a static array. Warning: This returns invalid values for
 * dynamically allocated arrays.
 *
 * @param a    static array
 */
#define ARRAY_SIZE(a) (sizeof(a)/sizeof((a)[0]))

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

	ir_node* FunctionInliner::createArgumentsTuple(ir_node* callNode, ir_graph* callerIrg, ir_graph* calleeIrg)
	{
		/* If the call has parameters, copy all parameter entities */
		ir_entity* calleeEntity = get_irg_entity(calleeIrg); // ent
		ir_type* calleeType = get_entity_type(calleeEntity); // mtp
		int n_params = get_method_n_params(calleeType);

		if (n_params != 0)
		{
			// TODO: what does this do exactly?
			copy_parameter_entities(callNode, calleeIrg);
		}

		/* create the argument tuple */

		ir_node** args_in = ALLOCAN(ir_node*, n_params); // maybe better new ir_node*[n_params];  ??

		ir_node* callingBlock = get_nodes_block(callNode); // block, callingBlock

		for (int i = n_params - 1; i >= 0; --i)
		{
			ir_node* argument = get_Call_param(callNode, i); // arg
			ir_type* parameterType = get_method_param_type(calleeType, i); // param_tp
			ir_mode* parameterMode = get_type_mode(parameterType); // mode

			// TODO: Can this really happen?
			if (parameterMode != get_irn_mode(argument))
				argument = new_r_Conv(callingBlock, argument, parameterMode);

			args_in[i] = argument;
		}


		/* the procedure and later replaces the Start node of the called graph.
		 * Post_call is the old Call node and collects the results of the called
		 * graph. Both will end up being a tuple. */
		/* XxMxPxPxPxT of Start + parameter of Call */
		ir_node* in[pn_Start_max + 1];
		in[pn_Start_M]              = get_Call_mem(callNode);
		in[pn_Start_P_frame_base]   = get_irg_frame(callerIrg);
		in[pn_Start_T_args]         = new_r_Tuple(callingBlock, n_params, args_in);
		ir_node* pre_call = new_r_Tuple(callingBlock, pn_Start_max + 1, in);

		//delete[] args_in;

		return pre_call;
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

		/* -- Decide how to handle exception control flow: Is there a handler
		   for the Call node, or do we branch directly to End on an exception?
		   exc_handling:
		   0 There is a handler.
		   2 Exception handling not represented in Firm. -- */
		ir_node *Xproj = NULL;
		for (ir_node *proj = (ir_node*)get_irn_link(callNode); proj != NULL;
			 proj = (ir_node*)get_irn_link(proj)) {
			unsigned proj_nr = get_Proj_num(proj);
			if (proj_nr == pn_Call_X_except) Xproj = proj;
		}
		//enum exc_mode exc_handling = Xproj != NULL ? exc_handler : exc_no_handler;

		/* entity link is used to link entities on old stack frame to the
		 * new stack frame */
		irp_reserve_resources(irp, IRP_RESOURCE_ENTITY_LINK);


		ir_node* pre_call = createArgumentsTuple(callNode, callerIrg, calleeIrg);

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
		ir_node* start_block = get_irg_start_block(calleeIrg);
		set_irn_link(start_block, get_nodes_block(pre_call));
		mark_irn_visited(start_block);

		ir_node* start = get_irg_start(calleeIrg);
		set_irn_link(start, pre_call);
		mark_irn_visited(start);

		ir_node* nomem = get_irg_no_mem(calleeIrg);
		set_irn_link(nomem, get_irg_no_mem(callerIrg));
		mark_irn_visited(nomem);

		/* copy entities and nodes */
		assert(!irn_visited(get_irg_end(calleeIrg)));
		copy_frame_entities(calleeIrg, callerIrg);
		irg_walk_core(get_irg_end(calleeIrg), &FunctionInliner::copy_node_inline, &FunctionInliner::set_preds_inline,
		              callerIrg);

		irp_free_resources(irp, IRP_RESOURCE_ENTITY_LINK);


		/* -- Merge the end of the inlined procedure with the call site -- */
		/* We will turn the old Call node into a Tuple with the following
		   predecessors:
		   -1:  Block of Tuple.
		   0: Phi of all Memories of Return statements.
		   1: Jmp from new Block that merges the control flow from all exception
		   predecessors of the old end block.
		   2: Tuple of all arguments.
		   3: Phi of Exception memories.
		   In case the old Call directly branches to End on an exception we don't
		   need the block merging all exceptions nor the Phi of the exception
		   memories.
		*/

		/* Precompute some values */
		ir_node *end_bl = (ir_node*)get_irn_link(get_irg_end_block(calleeIrg));
		ir_node *end    = (ir_node*)get_irn_link(get_irg_end(calleeIrg));
		int      arity  = get_Block_n_cfgpreds(end_bl); /* arity = n_exc + n_ret  */
		int      n_res  = get_method_n_ress(get_Call_type(callNode));

		ir_node **res_pred = XMALLOCN(ir_node*, n_res);
		ir_node **cf_pred  = XMALLOCN(ir_node*, arity);

		/* archive keepalives */
		int irn_arity = get_irn_arity(end);
		for (int i = 0; i < irn_arity; i++) {
			ir_node *ka = get_End_keepalive(end, i);
			if (!is_Bad(ka))
				add_End_keepalive(get_irg_end(callerIrg), ka);
		}

		/* replace Return nodes by Jump nodes */
		int n_ret = 0;
		for (int i = 0; i < arity; i++) {
			ir_node *ret = get_Block_cfgpred(end_bl, i);
			if (is_Return(ret)) {
				ir_node *block = get_nodes_block(ret);
				cf_pred[n_ret] = new_r_Jmp(block);
				n_ret++;
			}
		}
		/* avoid blocks without any inputs */
		ir_node* callingBlock = get_nodes_block(callNode);
		if (n_ret == 0) {
			ir_node *in[] = { new_r_Bad(callerIrg, mode_X) };
			set_irn_in(callingBlock, ARRAY_SIZE(in), in);
			set_irn_in(callingBlock, 1, in);
		} else {
			set_irn_in(callingBlock, n_ret, cf_pred);
		}

		/* build a Tuple for all results of the method.
		 * add Phi node if there was more than one Return. */
		/* First the Memory-Phi */
		int n_mem_phi = 0;
		for (int i = 0; i < arity; i++) {
			ir_node *ret = get_Block_cfgpred(end_bl, i);
			if (is_Return(ret)) {
				cf_pred[n_mem_phi++] = get_Return_mem(ret);
			}
			/* memory output for some exceptions is directly connected to End */
			if (is_Call(ret)) {
				cf_pred[n_mem_phi++] = new_r_Proj(ret, mode_M, 3);
			} else if (is_fragile_op(ret)) {
				/* We rely that all cfops have the memory output at the same
				 * position. */
				cf_pred[n_mem_phi++] = new_r_Proj(ret, mode_M, 0);
			} else if (is_Raise(ret)) {
				cf_pred[n_mem_phi++] = new_r_Proj(ret, mode_M, 1);
			}
		}
		ir_node *const call_mem =
			n_mem_phi > 0 ? new_r_Phi(callingBlock, n_mem_phi, cf_pred, mode_M)
			              : new_r_Bad(callerIrg, mode_M);
		/* Conserve Phi-list for further inlining -- but might be optimized */
		if (get_nodes_block(call_mem) == callingBlock) {
			set_irn_link(call_mem, get_irn_link(callingBlock));
			set_irn_link(callingBlock, call_mem);
		}
		/* Now the real results */
		ir_type *ctp      = get_Call_type(callNode);
		ir_node *call_res;
		if (n_res > 0) {
			for (int j = 0; j < n_res; j++) {
				ir_type *res_type     = get_method_res_type(ctp, j);
				//bool     is_aggregate = is_aggregate_type(res_type);
				ir_mode *res_mode     = get_type_mode(res_type);
				int n_ret = 0;
				for (int i = 0; i < arity; i++) {
					ir_node *ret = get_Block_cfgpred(end_bl, i);
					if (is_Return(ret)) {
						ir_node *res = get_Return_res(ret, j);
						if (get_irn_mode(res) != res_mode) {
							ir_node *block = get_nodes_block(res);
							res = new_r_Conv(block, res, res_mode);
						}
						cf_pred[n_ret] = res;
						n_ret++;
					}
				}
				ir_node *const phi = n_ret > 0
					? new_r_Phi(callingBlock, n_ret, cf_pred, res_mode)
					: new_r_Bad(callerIrg, res_mode);
				res_pred[j] = phi;
				/* Conserve Phi-list for further inlining -- but might be
				 * optimized */
				if (get_nodes_block(phi) == callingBlock) {
					set_Phi_next(phi, get_Block_phis(callingBlock));
					set_Block_phis(callingBlock, phi);
				}
			}
			call_res = new_r_Tuple(callingBlock, n_res, res_pred);
		} else {
			call_res = new_r_Bad(callerIrg, mode_T);
		}
		// continue with https://github.com/MatzeB/libfirm/blob/master/ir/opt/opt_inline.c#L444
		// FIXME: determine correct return value!!!
		return true;
	}

	/**
	 * copy all entities on the stack frame on 1 irg to the stack frame of another.
	 * Sets entity links of the old entities to the copies
	 */
	void FunctionInliner::copy_frame_entities(ir_graph* from, ir_graph* to)
	{
		ir_type* from_frame = get_irg_frame_type(from);
		ir_type* to_frame   = get_irg_frame_type(to);
		assert(from_frame != to_frame);

		for (size_t i = 0, n_members = get_class_n_members(from_frame);
		        i < n_members; ++i)
		{
			ir_entity* old_ent = get_class_member(from_frame, i);

			// parameter entities are already copied and the link has been set
			if (!is_parameter_entity(old_ent))
			{
				ir_entity* new_ent = copy_entity_own(old_ent, to_frame);
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
	void FunctionInliner::copy_node_inline(ir_node* node, void* env)
	{
		ir_graph* new_irg  = (ir_graph*) env;
		ir_node*  new_node = irn_copy_into_irg(node, new_irg);

		set_irn_link(node, new_node);

		if (is_Member(node))
		{
			ir_graph*  old_irg        = get_irn_irg(node);
			ir_type*   old_frame_type = get_irg_frame_type(old_irg);
			ir_entity* old_entity     = get_Member_entity(node);
			assert(is_Member(new_node));

			/* use copied entities from the new frame */
			if (get_entity_owner(old_entity) == old_frame_type)
			{
				ir_entity* new_entity = (ir_entity*)get_entity_link(old_entity);
				assert(new_entity != NULL);
				set_Member_entity(new_node, new_entity);
			}
		}
	}

	void FunctionInliner::set_preds_inline(ir_node* node, void* env)
	{
		irn_rewire_inputs(node);

		/* move constants into start block */
		ir_node* new_node = (ir_node*)get_irn_link(node);

		if (is_irn_start_block_placed(new_node))
		{
			ir_graph* new_irg     = (ir_graph*) env;
			ir_node*  start_block = get_irg_start_block(new_irg);
			set_nodes_block(new_node, start_block);
		}
	}

	void FunctionInliner::irn_rewire_inputs(ir_node* node)
	{
		ir_node* new_node = (ir_node*)get_irn_link(node);

		if (!is_Block(node))
		{
			ir_node* block     = get_nodes_block(node);
			ir_node* new_block = (ir_node*)get_irn_link(block);
			set_nodes_block(new_node, new_block);
		}

#define foreach_irn_in(irn, idx, pred) \
	for (bool pred##__b = true; pred##__b;) \
		for (ir_node const *const pred##__irn = (irn); pred##__b; pred##__b = false) \
			for (int idx = 0, pred##__n = get_irn_arity(pred##__irn); pred##__b && idx != pred##__n; ++idx) \
				for (ir_node *const pred = (pred##__b = false, get_irn_n(pred##__irn, idx)); !pred##__b; pred##__b = true)

		foreach_irn_in(node, i, in)
		{
			ir_node* new_in = (ir_node*)get_irn_link(in);
			set_irn_n(new_node, i, new_in);
		}

#undef foreach_irn_in

		/* Now the new node is complete. We can add it to the hash table for CSE. */
		//add_identities(new_node);
	}

	ir_node* FunctionInliner::irn_copy_into_irg(ir_node* node, ir_graph* irg)
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


	/* Copies parameter entities from the given called graph */
	void FunctionInliner::copy_parameter_entities(ir_node *call, ir_graph *calleeIrg)
	{
		dbg_info *dbgi         = get_irn_dbg_info(call);
		ir_graph *irg          = get_irn_irg(call);
		ir_node  *frame        = get_irg_frame(irg);
		ir_node  *block        = get_nodes_block(call);
		ir_type  *called_frame = get_irg_frame_type(calleeIrg);
		ir_type  *frame_type   = get_irg_frame_type(irg);
		ir_node  *call_mem     = get_Call_mem(call);
		ir_node **sync_mem     = NULL;

		for (size_t i = 0, n_entities = get_class_n_members(called_frame);
		     i < n_entities; ++i) {
			ir_entity *old_entity = get_class_member(called_frame, i);
			if (!is_parameter_entity(old_entity))
				continue;

			ir_type   *old_type    = get_entity_type(old_entity);
			dbg_info  *entity_dbgi = get_entity_dbg_info(old_entity);
			ident     *old_name    = get_entity_ident(old_entity);
			ident     *name        = id_mangle3("", old_name, "$inlined");
			ir_entity *new_ent     = new_entity(frame_type, name, old_type);
			set_entity_dbg_info(new_ent, entity_dbgi);
			set_entity_link(old_entity, new_ent);

			size_t   n_param_pos = get_entity_parameter_number(old_entity);
			ir_node *param       = get_Call_param(call, n_param_pos);
			ir_node *sel         = new_rd_Member(dbgi, block, frame, new_ent);
			ir_node *new_mem;

				/* Store the parameter onto the frame */
				ir_node *store = new_rd_Store(dbgi, block, call_mem, sel, param, old_type, cons_none);
				new_mem = new_r_Proj(store, mode_M, pn_Store_M);

			if (sync_mem) {
				ARR_APP1(ir_node*, sync_mem, new_mem);
			} else {
				sync_mem = NEW_ARR_F(ir_node*, 1);
				sync_mem[0] = new_mem;
			}
		}

		if (sync_mem != NULL) {
			int sync_arity = (int)ARR_LEN(sync_mem);
			if (sync_arity > 1) {
				ir_node *sync = new_r_Sync(block, sync_arity, sync_mem);
				set_Call_mem(call, sync);
			} else {
				set_Call_mem(call, sync_mem[0]);
			}
			DEL_ARR_F(sync_mem);
		}
	}
}