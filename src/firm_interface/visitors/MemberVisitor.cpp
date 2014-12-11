#include "MemberVisitor.hpp"
#include "StatementVisitor.hpp"

namespace firm
{
	namespace visitor
	{
		MemberVisitor::MemberVisitor(ClassVisitor& classVisitor): classVisitor(classVisitor)
		{
			setOwner(classVisitor.getOwner());
		}

		void MemberVisitor::foldConstants(ir_graph* irg)
		{
			dump_ir_graph(irg, "pre-optimize");

			// IR_GRAPH_PROPERTY_CONSISTENT_DOMINANCE property is necessary for walking the graph
			add_irg_properties(irg, IR_GRAPH_PROPERTY_CONSISTENT_DOMINANCE);

			//irg_finalize_cons(irg); not working, aborts with Assertion failed: (irg_is_constrained(irg, IR_GRAPH_CONSTRAINT_CONSTRUCTION)), function set_r_cur_block, file ir/ir/ircons.c, line 435.

			auto worklist = FirmInterface::getInstance().getWorklist();

			while (!worklist.empty())
			{
				ir_node* node = worklist.front();

				ir_printf("%F\n", node);

				if (is_Minus(node))
				{
					ir_node* child = get_irn_n(node, 0);

					if (is_Const(child))
						exchange(node, new_Const_long(mode_Is, get_tarval_long(computed_value(node))));
				}

				if (is_Add(node))
				{
					ir_tarval* tarVal = computed_value(node);
					ir_printf("%F mode: %F\n", node, get_tarval_mode(tarVal));

					// Both arguments are constants.
					if (get_tarval_mode(tarVal) == mode_Is)
						exchange(node, new_Const_long(mode_Is, get_tarval_long(tarVal)));
					else
					{
						// Check whether at least one argument is 0, and if so,
						// apply the rule x + 0 = x (or 0 + x = x).
						ir_node* left = get_irn_n(node, 0);
						ir_node* right = get_irn_n(node, 1);

						if (is_Const(left) && get_tarval_long(computed_value(left)) == 0)
							exchange(node, right);

						if (is_Const(right) && get_tarval_long(computed_value(right)) == 0)
							exchange(node, left);
					}
				}
				else if (is_Sub(node))
				{
					ir_tarval* tarVal = computed_value(node);
					ir_printf("%F mode: %F\n", node, get_tarval_mode(tarVal));

					if (get_tarval_mode(tarVal) == mode_Is)
						exchange(node, new_Const_long(mode_Is, get_tarval_long(tarVal)));
					else
					{
						// Check whether at least one argument is 0, and if so,
						// apply the rule x - 0 = x (or 0 - x = -x).
						ir_node* left = get_irn_n(node, 0);
						ir_node* right = get_irn_n(node, 1);

						if (is_Const(left) && get_tarval_long(computed_value(left)) == 0)
							exchange(node, new_Minus(right, mode_Is));

						if (is_Const(right) && get_tarval_long(computed_value(right)) == 0)
							exchange(node, left);
					}
				}
				else if (is_Mul(node))
				{
					ir_tarval* tarVal = computed_value(node);
					ir_printf("%F mode: %F\n", node, get_tarval_mode(tarVal));

					if (get_tarval_mode(tarVal) == mode_Is)
						exchange(node, new_Const_long(mode_Is, get_tarval_long(tarVal)));
					else
					{
						ir_node* left = get_irn_n(node, 0);
						ir_node* right = get_irn_n(node, 1);

						// If possible, apply the rules:
						//     x * (-1) = -x
						//     x * 1 = x
						//     x * 0 = 0
						if (is_Const(left))
						{
							long value = get_tarval_long(computed_value(left));

							if (value == -1)
								exchange(node, new_Minus(right, mode_Is));
							else if (value == 0)
								exchange(node, new_Const_long(mode_Is, 0));
							else if (value == 1)
								exchange(node, right);
						}

						// See above...
						if (is_Const(right))
						{
							long value = get_tarval_long(computed_value(right));

							if (value == -1)
								exchange(node, new_Minus(left, mode_Is));
							else if (value == 0)
								exchange(node, new_Const_long(mode_Is, 0));
							else if (value == 1)
								exchange(node, left);
						}
					}
				}
				else if (is_Proj(node))
				{
					// Get first child of proj node
					ir_node* divNode = get_irn_n(node, 0);

					if (is_Div(divNode))
					{
						// Get children of div node (operands)
						ir_node* dividend = get_irn_n(divNode, 1);
						ir_node* divisor = get_irn_n(divNode, 2);

						if (is_Const(dividend) && is_Const(divisor))
						{
							long divisorValue = get_tarval_long(computed_value(divisor));

							// Optimize if not dividing by zero, otherwise simply leave the original Div node alone
							if (divisorValue != 0)
							{
								long division = get_tarval_long(computed_value(dividend)) / divisorValue;
								exchange(node, new_Const_long(mode_Is, division));
							}
						}
					}
				}
				// TODO: a +/- 0 -> a
				// not working yet:
				else if (is_Cmp(node))
				{
					ir_tarval* tarVal = computed_value(node);
					//ir_printf("%F\n", get_tarval_mode(tarVal));

					if (get_tarval_mode(tarVal) == mode_b)
					{
						// computed value == true?
						if (tarVal == get_tarval_b_true())
						{
							//exchange(node, new_Cmp(NULL, NULL, ir_relation::ir_relation_true));
						}
					}
				}

				// asd
				worklist.pop();
			}

			dump_ir_graph(irg, "post-optimize");
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

			// optimize Firm graph
			foldConstants(irg);

			irg_finalize_cons(irg);
			irg_verify(irg);
		}

		void MemberVisitor::visit(shptr<const ast::MethodDeclaration> methodDeclaration)
		{
			std::string mangledMethodName = methodDeclaration->mangle();

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
			FirmInterface::getInstance().initSystem();

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
