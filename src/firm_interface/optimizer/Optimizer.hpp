#ifndef OPTIMIZER_HPP
#define OPTIMIZER_HPP

#include <libfirm/firm.h>
#include "../../globals.hpp"

namespace firm
{
	class Optimizer
	{
		protected:
			ir_graph* irg;
			bool changed;
			unsigned int max_iterations;
			int optimizationFlag;

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
			 * Optimize the function graph by eliminating common subexpressions.
			 * @return true if some optimization was made, false if no optimization was made and the graph didn't change
			 */
			bool eliminateCommonSubexpressions();

			/**
			 * Optimize the function graph by performing load-store optimization.
			 * @return true if some optimization was made, false if no optimization was made and the graph didn't change
			 */
			bool optimizeLoadStore();

			/**
			 * @return true if some optimization was made, false if no optimization was made and the graph didn't change
			 */
			bool optimizeLocal();

			bool optimizeAddressMode();
			bool optimizeBitFiddling();

			bool optimizeJumpChains();
		public:

			Optimizer(ir_graph* irg);
			void setOptimizationFlag(int flag);
			void setMaxIterations(int max);
			void run();
			bool graphWasChanged() const;
	};
}
#endif
