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
			<<< <<< < HEAD
			== == == =
			    int optimizationFlag;
			>>> >>> > 7b54fdb6d95d3be1d126b7d54c8237ece29bce0a

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
			<<<<<<< HEAD
			=======
			 * Optimize the function graph by eliminating common subexpressions.
			>>>>>>> 7b54fdb6d95d3be1d126b7d54c8237ece29bce0a
			 * @return true if some optimization was made, false if no optimization was made and the graph didn't change
			 */
			bool eliminateCommonSubexpressions();

			/**
			<<<<<<< HEAD
			 * Optimize the function graph by eliminating common subexpressions.
			=======
			 * Optimize the function graph by performing load-store optimization.
			 * @return true if some optimization was made, false if no optimization was made and the graph didn't change
			 */
			bool optimizeLoadStore();

			/**
			>>>>>>> 7b54fdb6d95d3be1d126b7d54c8237ece29bce0a
			 * @return true if some optimization was made, false if no optimization was made and the graph didn't change
			 */
			bool optimizeLocal();

			<<< <<< < HEAD
			bool optimizeAddressMode();

		public:
			Optimizer(ir_graph* irg);
			== == == =
			    bool optimizeBitFiddling();

			void handleConvNodes();

		public:

			static const int NONE = 0;
			static const int DEFAULT = 1;

			Optimizer(ir_graph* irg);
			void setOptimizationFlag(int flag);
			void setMaxIterations(int max);
			>>> >>> > 7b54fdb6d95d3be1d126b7d54c8237ece29bce0a
			void run();
			bool graphWasChanged() const;
	};
}
#endif
