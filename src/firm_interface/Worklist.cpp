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

		struct envMembers {
 			std::queue<ir_node*>* pQueue;
 			std::unordered_map<ir_node*,bool>* pIsQueued;
 		};

		envMembers envInstance;
		envInstance.pQueue = &this->worklist;
		envInstance.pIsQueued = &this->isQueued;

		ir_func addToWorklist = [](ir_node * node, void* env)
		{
			auto envInstance = (envMembers*)env;

			ir_tarval* tarval;
			// TODO: Support other modes such as Bu, Lu
			if (is_Const(node) && get_irn_mode(node) == mode_Is) 
				tarval = get_Const_tarval(node);
			else
				tarval = tarval_unknown;

			set_irn_link(node, (void*)tarval);

			envInstance->pQueue->push(node);
			(*envInstance->pIsQueued)[node] = true;
		};

		walk_topological(functionGraph, addToWorklist, (void*)&envInstance);
	}

	void Worklist::run()
	{
		while (!worklist.empty())
		{
			ir_node* node = worklist.front();
			this->isQueued[node] = false;
			worklist.pop();
			handler.handle(node);

			for (auto& newNode : *handler.getNewNodes())
				if (!isQueued[newNode]) {
					worklist.push(newNode);
					isQueued[newNode] = true;
					//std::cout << "added node: ";
					//ir_printf("%F\n", newNode);					
				} else {
					//std::cout << "did not add node: ";
					//ir_printf("%F\n", newNode);
				}
		}
	}
}