#include "Worklist.hpp"

namespace firm
{

	static void walk_topo_helper(ir_node* irn, irg_walk_func* walker, void* env)
	{
		if (irn_visited(irn))
			return;

		/* only break loops at phi/block nodes */
		const bool is_loop_breaker = is_Phi(irn) || is_Block(irn);

		if (is_loop_breaker)
			mark_irn_visited(irn);

		if (!is_Block(irn))
		{
			ir_node* block = get_nodes_block(irn);

			if (block != NULL)
				walk_topo_helper(block, walker, env);
		}

		for (int i = 0; i < get_irn_arity(irn); ++i)
		{
			ir_node* pred = get_irn_n(irn, i);
			walk_topo_helper(pred, walker, env);
		}

		if (is_loop_breaker || !irn_visited(irn))
			walker(irn, env);

		mark_irn_visited(irn);
	}

	static void walk_topological(ir_graph* irg, irg_walk_func* walker, void* env)
	{
		inc_irg_visited(irg);
		walk_topo_helper(get_irg_end(irg), walker, env);
	}
	Worklist::Worklist(ir_graph* functionGraph, ConstantFolder& handler): functionGraph(functionGraph), handler(handler)
	{
		typedef void (*ir_func)(ir_node*, void*);
		ir_func addToWorklist = [](ir_node * node, void* env)
		{
			auto worklist = (std::queue<ir_node*>*)env;
			set_irn_link(node, (void*)tarval_unknown);
			worklist->push(node);
		};

		// post ordering
		//assure_irg_properties(functionGraph, IR_GRAPH_PROPERTY_CONSISTENT_DOMINANCE);
		//irg_walk_blkwise_dom_top_down(functionGraph, NULL, addToWorklist, (void*)&this->worklist);
		walk_topological(functionGraph, addToWorklist, (void*)&this->worklist);
	}

	void Worklist::run()
	{
		while (!worklist.empty())
		{
			ir_node* node = worklist.front();
			handler.handle(node);

			for (auto& newNode : *handler.getNewNodes())
				worklist.push(newNode);

			worklist.pop();
		}
	}


}