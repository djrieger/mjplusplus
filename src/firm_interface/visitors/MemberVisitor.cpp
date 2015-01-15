#include "MemberVisitor.hpp"
#include "StatementVisitor.hpp"
#include "../ConstantFolder.hpp"
#include "../Worklist.hpp"
#include "../ControlFlowOptimizer.hpp"
#include "../LocalOptimizer.hpp"


#define MAX_OPTIMIZATION_ITERATIONS 10


namespace firm
{
	namespace visitor
	{
		MemberVisitor::MemberVisitor(ClassVisitor& classVisitor): classVisitor(classVisitor)
		{
			setOwner(classVisitor.getOwner());
		}

		bool MemberVisitor::foldConstants(ir_graph* irg)
		{


			ConstantFolder constantFolder(irg);
			firm::Worklist worklist(irg, constantFolder);

			edges_activate(irg);
			worklist.run();
			edges_deactivate(irg);

			return constantFolder.graphChanged();
		}

		bool MemberVisitor::optimizeControlFlow(ir_graph* irg)
		{
			ControlFlowOptimizer cfOptimizer(irg);
			firm::Worklist worklist(irg, cfOptimizer);

			edges_activate(irg);
			worklist.run();
			edges_deactivate(irg);

			return cfOptimizer.graphChanged();
		}

		bool MemberVisitor::eliminateCommonSubexpressions(ir_graph* irg)
		{
			bool change = true;

			bool made_optimization = false;

			for (int count = 0; change && count < MAX_OPTIMIZATION_ITERATIONS; count++)
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

		bool MemberVisitor::optimizeLocal(ir_graph* irg)
		{
			LocalOptimizer localOpt(irg);
			firm::Worklist worklist(irg, localOpt);

			edges_activate(irg);
			worklist.run();
			edges_deactivate(irg);

			return localOpt.graphChanged();
		}

		void MemberVisitor::visitMethodBodyAndFinalize(shptr<const ast::MethodDeclaration> methodDeclaration, ir_graph* irg)
		{
			// set method start block as current block
			set_cur_block(get_irg_start_block(irg));

			methodDeclaration->createVariablePositions();

			StatementVisitor stmtVisitor(*this);

			if (methodDeclaration->getBlock())
			{
				ir_node* start = get_irg_start(irg);
				ir_node* args = new_Proj(start, mode_T, pn_Start_T_args);
				ir_entity* m_ent = get_irg_entity(irg);
				ir_type* m_type = get_entity_type(m_ent);
				size_t param_count = get_method_n_params(m_type);

				for (size_t i = 0; i < param_count; i++)
				{
					ir_type* p_type = get_method_param_type(m_type, i);
					ir_node* arg = new_Proj(args, get_type_mode(p_type), i);
					set_value(i, arg);
				}

				methodDeclaration->getBlock()->accept(stmtVisitor);
			}

			// current block did not contain a return statement, add one:
			if (get_cur_block())
			{
				ir_node* currentMemState = get_store();
				ir_node* x = new_Return(currentMemState, 0, NULL);
				add_immBlock_pred(get_irg_end_block(irg), x);
			}

			// mature end block as method body is fully converted to Firm nodes
			mature_immBlock(get_irg_end_block(irg));

			irg_finalize_cons(irg);

			// optimize Firm graph
			FirmInterface::getInstance().outputFirmGraph(irg, "orig");

			bool did_change;
			int iterations_count = 0;

			do
			{
				did_change = false;

				did_change = did_change || foldConstants(irg);
				did_change = did_change || optimizeLocal(irg);
				did_change = did_change || eliminateCommonSubexpressions(irg);
				did_change = did_change || optimizeControlFlow(irg);
			}
			while (did_change && ++iterations_count < MAX_OPTIMIZATION_ITERATIONS);

			remove_bads(irg);

			FirmInterface::getInstance().outputFirmGraph(irg, "final");
			irg_verify(irg);
		}

		void MemberVisitor::visit(shptr<const ast::MethodDeclaration> methodDeclaration)
		{
			std::string mangledMethodName = methodDeclaration->mangle();
			//std::cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << std::endl;
			//std::cout << mangledMethodName << std::endl;

			// ignore println as its implementation is generated by FirmInterface::build automatically
			if (mangledMethodName != "_COut_Mprintln")
			{
				ir_entity* methodEntity = FirmInterface::getInstance().getMethodEntity(owner, mangledMethodName);
				function_graph = new_ir_graph(methodEntity, methodDeclaration->countVariableDeclarations());
				set_current_ir_graph(function_graph);

				visitMethodBodyAndFinalize(methodDeclaration, function_graph);
			}
		}

		void MemberVisitor::visit(shptr<const ast::MainMethodDeclaration> mainMethodDecl)
		{
			std::string mainMethodName = "main";

			ir_type* proc_main = new_type_method(0, 0);
			ir_type* globalOwner = get_glob_type();
			ir_entity* mainMethodEntity = new_entity(globalOwner, new_id_from_str(mainMethodName.c_str()), proc_main);
			ir_graph* irg = new_ir_graph(mainMethodEntity, mainMethodDecl->countVariableDeclarations());

			set_current_ir_graph(irg);

			// Build System
			//FirmInterface::getInstance().initSystem();

			visitMethodBodyAndFinalize(mainMethodDecl, irg);
		}

		void MemberVisitor::visit(shptr<const ast::FieldDeclaration>)
		{
			;
		}

		ir_graph* MemberVisitor::getGraph() const
		{
			return function_graph;
		}
	}
}
