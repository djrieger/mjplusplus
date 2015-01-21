#include "BitFiddlingOptimizer.hpp"
#include "ConstantFolder.hpp"
#include "ControlFlowOptimizer.hpp"
#include "LocalOptimizer.hpp"
#include "Optimizer.hpp"
#include "Worklist.hpp"

namespace firm
{
	Optimizer::Optimizer(ir_graph* irg): irg(irg)
	{
		changed = false;
		max_iterations = 10;
		optimizationFlag = DEFAULT;
	}

	void Optimizer::setOptimizationFlag(int flag)
	{
		optimizationFlag = flag;
	}

	void Optimizer::setMaxIterations(int max)
	{
		max_iterations = max;
	}

	void Optimizer::run()
	{
		if (optimizationFlag >= DEFAULT)
		{
			std::cout << "Optimizing with flag = " << optimizationFlag << std::endl;
			unsigned int iterations_count = 0;

			do
			{
				changed = foldConstants() || changed;
				changed = optimizeLocal() || changed;
				changed = eliminateCommonSubexpressions() || changed;
				changed = optimizeControlFlow() || changed;
			}
			while (changed && ++iterations_count < max_iterations);

			remove_bads(irg);

			optimizeBitFiddling();
		}
		else
			std::cout << "No optimization" << std::endl;
	}

	bool Optimizer::graphWasChanged() const
	{
		return  changed;
	}

	bool Optimizer::foldConstants()
	{
		ConstantFolder constantFolder(irg);
		firm::Worklist worklist(irg, constantFolder);

		edges_activate(irg);
		worklist.run();
		edges_deactivate(irg);

		return constantFolder.graphChanged();
	}

	bool Optimizer::optimizeControlFlow()
	{
		ControlFlowOptimizer cfOptimizer(irg);
		firm::Worklist worklist(irg, cfOptimizer);

		edges_activate(irg);
		worklist.run();
		edges_deactivate(irg);

		return cfOptimizer.graphChanged();
	}

	bool Optimizer::eliminateCommonSubexpressions()
	{
		bool change = true;

		bool made_optimization = false;

		for (unsigned int count = 0; change && count < max_iterations; count++)
		{
			CommonSubexpressionEliminator commonSubexpressionEliminator(irg);
			firm::Worklist worklist(irg, commonSubexpressionEliminator);

			edges_activate(irg);
			change = worklist.run();

			edges_deactivate(irg);

			made_optimization = made_optimization || commonSubexpressionEliminator.graphChanged();
		}

		return made_optimization;
	}

	bool Optimizer::optimizeLocal()
	{
		LocalOptimizer localOpt(irg);
		firm::Worklist worklist(irg, localOpt);

		edges_activate(irg);
		worklist.run();
		edges_deactivate(irg);

		return localOpt.graphChanged();
	}

	bool Optimizer::optimizeBitFiddling()
	{
		BitFiddlingOptimizer bfo(irg);
		firm::Worklist worklist(irg, bfo);

		edges_activate(irg);
		worklist.run();
		edges_deactivate(irg);

		return bfo.graphChanged();
	}
}
