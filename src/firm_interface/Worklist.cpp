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

	void Worklist::replaceGeneric(Node node)
	{
		std::cout << "found ";
		ir_printf("%F\n", node);

		if (node.getTarval() != tarval_bad && node.getTarval() != tarval_unknown && node.getTarval().isModeIs())
		{
			// ir_printf("parent tarval = %F\n", tarval);
			bool constChildren = true;
			unsigned int i = 0;
			// std::cout << "arity = " << get_irn_arity(node) << std::endl;

			while ( i < node.getChildCount() && constChildren)
			{
				Tarval predTarval = node.getChild(i).getTarval();
				// ir_printf("child tarval = %F\n", predTarval);

				if (!predTarval || get_tarval_mode(predTarval) != get_tarval_mode(node.getTarval()) || predTarval == tarval_bad || predTarval == tarval_unknown)
					constChildren = false;

				i++;
			}

			if (constChildren)
			{
				std::cout << "removed ";
				ir_printf("%F\n", node);
				ir_node* constNode = new_r_Const_long(functionGraph, get_irn_mode(node), node.getTarval().getLong());
				node.replaceWith(constNode, true);
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

	void Worklist::processChildren(Node node, std::function<void (Node leftChild, Node rightChild)> fun)
	{
		ir_printf("node: %F\n", node);
		Node child1 = node.getChild(0);
		//ir_printf("tarval1: %F\n", tarval1);

		if (node.getChildCount() == 1)
		{
			if (node.getTarval().isModeIs())
				fun(child1, NULL);
		} else
		{
			Node child2 = node.getChild(1);
			//ir_printf("tarval2: %F\n", tarval2);
			fun(child1, child2);
		}
	}

	void Worklist::replaceAdd(Node node)
	{
		processChildren(node, [&] (Node leftChild, Node rightChild) -> void
		{
			auto tarvalIsZero = [] (Tarval tarval) -> bool { return tarval && tarval.isModeIs() && tarval.getLong() == 0; };
			if (tarvalIsZero(leftChild.getTarval()))
				node.replaceWith(rightChild);
			else if (tarvalIsZero(rightChild.getTarval()))
			 	node.replaceWith(leftChild);
			else
				replaceGeneric(node);
		});
	}

	void Worklist::replaceSub(Node node)
	{
		processChildren(node, [&] (Node leftChild, Node rightChild) -> void
		{
			// ir_printf("SUB arity = %d\n", get_irn_arity(node));
			// ir_printf("SUB: left tar %F right tar %F\n", leftTarval, rightTarval);
			// ir_printf("SUB: left child %F right child %F\n", leftChild, rightChild);
			auto tarvalIsZero = [] (Tarval tarval) -> bool { return tarval && tarval.isModeIs() && tarval.getLong() == 0; };
			if (tarvalIsZero(leftChild.getTarval()))
				node.replaceWith(new_r_Minus(get_nodes_block(node), rightChild, get_irn_mode(node)));
			else if (tarvalIsZero(rightChild.getTarval()))
				node.replaceWith(leftChild);
			else
				replaceGeneric(node);
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