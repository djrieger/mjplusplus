#include "Optimizer.hpp"
#include "ConstantFolder.hpp"
#include "Worklist.hpp"
#include "ControlFlowOptimizer.hpp"
#include "LocalOptimizer.hpp"
#include "AddressModeOptimizer.hpp"

namespace firm
{
	Optimizer::Optimizer(ir_graph* irg): irg(irg)
	{
		changed = false;
		max_iterations = 10;
	}

	void Optimizer::run()
	{
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
		optimizeAddressMode();

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

	bool Optimizer::optimizeAddressMode()
	{
		AddressModeOptimizer addressModeOptimizer(irg);
		firm::Worklist worklist(irg, addressModeOptimizer);

		worklist.run();

		return addressModeOptimizer.graphChanged();
	}
}
