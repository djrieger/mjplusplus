#include "Worklist.hpp"
/*
namespace firm
{
	Worklist::Worklist(ir_graph* functionGraph): functionGraph(functionGraph)
	{
		typedef void (*ir_func)(ir_node*, void*);
		ir_func addToWorklist = [](ir_node * node, void* env)
		{
			auto worklist = (std::queue<ir_node*>*)env;
			set_irn_link(node, (void*)tarval_unknown);
			worklist->push(node);
		};

		// post ordering
		assure_irg_properties(functionGraph, IR_GRAPH_PROPERTY_CONSISTENT_DOMINANCE);
		irg_walk_blkwise_dom_top_down(functionGraph, NULL, addToWorklist, (void*)this->worklist);
	}

	void Worklist::run(std::function<void (ir_node*)> handleFunction);
	{
		while (!worklist.empty())
		{
			ir_node* node = worklist.front();
			handleFunction(node);
			worklist.pop();
		}
	}
}*/