#ifndef OPTIMIZER_HPP
#define OPTIMIZER_HPP

#include <libfirm/firm.h>
#include "../globals.hpp"

namespace firm
{
	class Optimizer
	{
		protected:
			ir_graph* irg;
			bool changed;
			unsigned int max_iterations;

			/**
			 * Optimize the function graph by folding the constants.
			 * @return true if some optimization was made, false if no optimization was made and the graph didn't change
			 */
			bool foldConstants();

			/**
			 * @return true if some optimization was made, false if no optimization was made and the graph didn't change
			 */
			bool optimizeControlFlow();

			/**
			 * @return true if some optimization was made, false if no optimization was made and the graph didn't change
			 */
			bool eliminateCommonSubexpressions();

			/**
			 * Optimize the function graph by eliminating common subexpressions.
			 * @return true if some optimization was made, false if no optimization was made and the graph didn't change
			 */
			bool optimizeLocal();

			bool optimizeAddressMode();

		public:
			Optimizer(ir_graph* irg);
			void run();
			bool graphWasChanged() const;
	};
}
#endif
