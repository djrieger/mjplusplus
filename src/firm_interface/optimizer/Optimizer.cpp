#include "AddressModeOptimizer.hpp"
#include "BitFiddlingOptimizer.hpp"
#include "CommonSubexpressionEliminator.hpp"
#include "ConstantFolder.hpp"
#include "ControlFlowOptimizer.hpp"
#include "LocalOptimizer.hpp"
#include "LoadStoreOptimizer.hpp"
#include "JumpChainOptimizer.hpp"
#include "Optimizer.hpp"
#include "../Worklist.hpp"
#include "../FirmInterface.hpp"

namespace firm
{
	Optimizer::Optimizer(ir_graph* irg): irg(irg)
	{
		changed = false;
		max_iterations = 10;
		optimizationFlag = FirmInterface::OptimizationFlags::DEFAULT;
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
		printf("opt flag: %d\n", optimizationFlag);

		if (optimizationFlag != FirmInterface::OptimizationFlags::NONE)
		{
			unsigned int iterations_count = 0;

			do
			{
				changed = foldConstants() || changed;
				changed = optimizeLocal() || changed;
				FirmInterface::getInstance().handleConvNodes(irg);
				changed = eliminateCommonSubexpressions() || changed;
				changed = optimizeLoadStore() || changed;
				changed = optimizeControlFlow() || changed;
			}
			while (changed && ++iterations_count < max_iterations);
			
			optimizeJumpChains();
			remove_bads(irg);

			if (!(optimizationFlag & FirmInterface::OptimizationFlags::FIRM_COMPATIBLE))
				optimizeAddressMode();

			optimizeBitFiddling();
		}
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

	bool Optimizer::optimizeLoadStore()
	{
		LoadStoreOptimizer lsOptimizer(irg);
		firm::Worklist worklist(irg, lsOptimizer);

		edges_activate(irg);
		worklist.run();
		edges_deactivate(irg);

		return lsOptimizer.graphChanged();
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

	bool Optimizer::optimizeBitFiddling()
	{
		BitFiddlingOptimizer bfo(irg);
		firm::Worklist worklist(irg, bfo);

		edges_activate(irg);
		worklist.run();
		edges_deactivate(irg);

		return bfo.graphChanged();
	}

	bool Optimizer::optimizeJumpChains()
	{
		JumpChainOptimizer jco(irg);
		firm::Worklist worklist(irg, jco);

		edges_activate(irg);
		worklist.run();
		edges_deactivate(irg);

		return jco.graphChanged();
	}
}
