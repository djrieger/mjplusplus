#include "Worklist.hpp"

namespace firm
{
	Worklist::Worklist(ir_graph* functionGraph, FirmNodeHandler& handler): functionGraph(functionGraph), handler(handler)
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
		irg_walk_blkwise_dom_top_down(functionGraph, NULL, addToWorklist, (void*)&this->worklist);
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