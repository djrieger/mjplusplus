#include "Worklist.hpp"

namespace firm
{

	static void walk_topo_helper(ir_node* irn, std::function<void (ir_node*, void*)> walker, void* env)
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

	static void walk_topological(ir_graph* irg, std::function<void (ir_node*, void*)> walker, void* env)
	{
		inc_irg_visited(irg);
		walk_topo_helper(get_irg_end(irg), walker, env);
	}

	Worklist::Worklist(ir_graph* functionGraph, ConstantFolder& handler): functionGraph(functionGraph), handler(handler)
	{
		typedef void (*ir_func)(ir_node*, void*);

		struct envMembers
		{
			std::queue<ir_node*>* pQueue;
			std::unordered_map<ir_node*, bool>* pIsQueued;
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

	void Worklist::replaceGeneric(ir_node* node)
	{
		std::cout << "found ";
		ir_printf("%F\n", node);

		ir_tarval* tarval = (ir_tarval*)get_irn_link(node);

		if (tarval != tarval_bad && tarval != tarval_unknown && get_tarval_mode(tarval) == mode_Is)
		{
			ir_printf("parent tarval = %F\n", tarval);
			bool constChildren = true;
			int i = 0;
			std::cout << "arity = " << get_irn_arity(node) << std::endl;

			while ( i < get_irn_arity(node) && constChildren)
			{
				ir_tarval* predTarval = (ir_tarval*)get_irn_link(get_irn_n(node, i));
				ir_printf("child tarval = %F\n", predTarval);

				if (!predTarval || get_tarval_mode(predTarval) != get_tarval_mode(tarval) || predTarval == tarval_bad || predTarval == tarval_unknown)
					constChildren = false;

				i++;
			}

			if (constChildren)
			{
				std::cout << "removed ";
				ir_printf("%F\n", node);
				ir_node* constNode = new_r_Const_long(functionGraph, get_irn_mode(node), get_tarval_long(tarval));
				set_irn_link(constNode, (void*)tarval);
				exchange(node, constNode);
			}
		}
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
			{
				if (!isQueued[newNode])
				{
					worklist.push(newNode);
					isQueued[newNode] = true;
					std::cout << "added node: ";
					ir_printf("%F\n", newNode);
				}
				else
				{
					std::cout << "did not add node: ";
					ir_printf("%F\n", newNode);
				}

			}
		}

		cleanUp();
	}

	void Worklist::processChildren(ir_node* node, std::function<void (ir_node* leftChild, ir_tarval* leftTarval, ir_node* rightChild, ir_tarval* rightTarval)> fun)
	{
		ir_printf("node: %F\n", node);
		ir_node* child1 = get_irn_n(node, 0);
		ir_tarval* tarval1 = (ir_tarval*)get_irn_link(child1);
		ir_printf("tarval1: %F\n", tarval1);

		if (get_irn_arity(node) == 1)
		{
			if (get_tarval_mode(tarval1) == mode_Is)
				fun(child1, tarval1, NULL, NULL);
		} else
		{
			ir_node* child2 = get_irn_n(node, 1);
			ir_tarval* tarval2 = (ir_tarval*)get_irn_link(child2);
			ir_printf("tarval2: %F\n", tarval2);
			/*if (get_tarval_mode(tarval2) != mode_Is)
				fun(child1, tarval1, child2, NULL);
			else */
				fun(child1, tarval1, child2, tarval2);
		}
	}

	void Worklist::replaceAdd(ir_node* node)
	{
		processChildren(node, [&] (ir_node * leftChild, ir_tarval * leftTarval, ir_node * rightChild, ir_tarval * rightTarval) -> void
		{
			if (leftTarval && get_tarval_mode(leftTarval) == mode_Is && get_tarval_long(leftTarval) == 0) {
				//set_irn_link(rightChild, (void*))
				exchange(node, rightChild);	
			} 
			else if (rightTarval && get_tarval_mode(rightTarval) == mode_Is && get_tarval_long(rightTarval) == 0) exchange(node, leftChild);
			else replaceGeneric(node);
		});
	}

	void Worklist::replaceSub(ir_node* node)
	{
		processChildren(node, [&] (ir_node * leftChild, ir_tarval * leftTarval, ir_node * rightChild, ir_tarval * rightTarval) -> void
		{
			ir_printf("SUB arity = %d\n", get_irn_arity(node));
			ir_printf("SUB: left tar %F right tar %F\n", leftTarval, rightTarval);
			ir_printf("SUB: left child %F right child %F\n", leftChild, rightChild);
			if (leftTarval && get_tarval_mode(leftTarval) == mode_Is && get_tarval_long(leftTarval) == 0) exchange(node, new_r_Minus(get_nodes_block(node), rightChild, get_irn_mode(node)));
			else if (rightTarval && get_tarval_mode(rightTarval) == mode_Is && get_tarval_long(rightTarval) == 0) exchange(node, leftChild);
			else replaceGeneric(node);
		});
	}

	void Worklist::replaceMul(ir_node* node)
	{
		processChildren(node, [&] (ir_node * leftChild, ir_tarval * leftTarval, ir_node * rightChild, ir_tarval * rightTarval) -> void
		{
			if (leftTarval && get_tarval_mode(leftTarval) == mode_Is && get_tarval_long(leftTarval) == 0) exchange(node, new_r_Minus(get_nodes_block(node), rightChild, get_irn_mode(node)));
			else if (rightTarval && get_tarval_mode(rightTarval) == mode_Is && get_tarval_long(rightTarval) == 0) exchange(node, leftChild);
			else replaceGeneric(node);

			// If possible, apply the rules:
			//     x * (-1) = -x
			//     x * 1 = x
			//     x * 0 = 0
			if (leftTarval && get_tarval_mode(leftTarval) == mode_Is)
			{
				long value = get_tarval_long(leftTarval);

				if (value == -1)
					updateTarvalAndExchange(node, new_r_Minus(get_nodes_block(node), right, get_irn_mode(node)));
				else if (value == 0)
					updateTarvalAndExchange(node, new_r_Const_long(irg, get_irn_mode(node), 0));
				else if (value == 1)
					updateTarvalAndExchange(node, right);
			}

			// See above...
			if (rightTarval && get_tarval_mode(rightTarval) == mode_Is)
			{
				long value = get_tarval_long(rightTarval);

				if (value == -1)
					updateTarvalAndExchange(node, new_r_Minus(get_nodes_block(node), left, get_irn_mode(node)));
				else if (value == 0)
					updateTarvalAndExchange(node, new_r_Const_long(irg, get_irn_mode(node), 0));
				else if (value == 1)
					updateTarvalAndExchange(node, left);
			}			
		});
	}	

	void Worklist::cleanUp()
	{
		typedef void (*ir_func)(ir_node*, void*);

		auto replaceLambda = [&] (ir_node * node, void*)
		{
			if (get_irn_mode(node) == mode_Is)
			{
				if (is_Phi(node)) replaceGeneric(node);
				else if (is_Add(node)) replaceAdd(node);
				else if (is_Mul(node)) replaceGeneric(node);
				else if (is_Sub(node)) replaceSub(node);

				else if (is_Minus(node)) replaceGeneric(node);
			}
		};
		walk_topological(functionGraph, replaceLambda, NULL);
	}
}