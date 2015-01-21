#ifndef ADDRESSMODEOPTIMIZER_HPP
#define ADDRESSMODEOPTIMIZER_HPP

#include "GraphOptimizer.hpp"

namespace firm
{
	class AddressModeOptimizer : public GraphOptimizer
	{
		private:
			ir_node* constant;
			ir_node* base;
			ir_node* x;
			ir_node* scale;

		public:
			AddressModeOptimizer();

			void handle_Mul(Node node);
			void handle_Add(Node node);
			virtual void handle(Node node);
			virtual void cleanUp(Node node);
	};

}

#endif // ADDRESSMODEOPTIMIZER_HPP
