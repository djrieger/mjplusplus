#ifndef ADDRESSMODEOPTIMIZER_HPP
#define ADDRESSMODEOPTIMIZER_HPP

#include "../GraphHandler.hpp"

namespace firm
{
	class AddressModeOptimizer : public GraphHandler
	{
		private:
			ir_node* constant;
			ir_node* base;
			ir_node* x;
			ir_node* scale;

		public:
			AddressModeOptimizer(ir_graph* irg);

			bool handle_Mul(Node node);
			bool handle_Add(Node node, bool recurse);
			virtual void handle(Node node);
			virtual void cleanUp(Node node);
	};

}

#endif // ADDRESSMODEOPTIMIZER_HPP
