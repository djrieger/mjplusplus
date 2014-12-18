#include <algorithm>
#include <string>

#include "CodeGen.hpp"

#include "FirmInterface.hpp"

//using namespace std::string_literals;

namespace firm
{
	CodeGen::CodeGen(FILE* out) : out(out)
	{
		;
	}

	/*
	 * general notes:
	 * I use parent/child in terms of execution order, a parent must be executed before it's children.
	 *
	 * For now I assume an infinite amount of registers / stack space,
	 * nodes with register constraints are annotated with them
	 * known constraints:
	 *   return in RAX;
	 *   call first args in RDI, RSI, RDX, RCX, R8, R9, remainder on stack, result in rax;
	 *   div/mod in ?
	 *
	 * each node puts it's result in a new register
	 * phis and nodes with multiple children merge registers
	 *
	 *
	 * irn_link used as key for register usage lookup
	 */

	void CodeGen::assemble(FILE* out)
	{
		CodeGen cg(out);

		for (size_t i = 0; i < get_irp_n_irgs(); i++)
			cg.assemble(get_irp_irg(i));
	}

	void CodeGen::assemble(ir_graph* irg)
	{
		partial.clear();
		usage.clear();
		registers.clear();
		code.clear();

		// fake register for non-data parents
		registers.push_back({NONE, {}, {}});

		edges_activate(irg);
		// no code for end block, just find returns
		ir_node* eb = get_irg_end_block(irg);
		set_irn_link(eb, NULL);

		for (int i = get_irn_arity(eb) - 1; i >= 0; i--)
			assemble(get_irn_n(eb, i));

		//TODO: keepalives

		edges_deactivate(irg);
		//TODO: emit function
		//TODO: %n$ syntax not in standart C(++)
		fprintf(out, "\t.p2align 4,,15\n\t.globl  %1$s\n\t.type   %1$s, @function\n%1$s:\n", get_entity_name(get_irg_entity(irg)));

		for (auto& block : code)
		{
			fprintf(out, ".L_%s_%zu:\n", get_entity_name(get_irg_entity(irg)), get_irn_node_nr(block.first));

			for (auto it = block.second.rbegin(); it != block.second.rend(); it++)
				fprintf(out, "\t%s\n", it->c_str());
		}

		fprintf(out, "\t.size   %1$s, .-%1$s\n\n", get_entity_name(get_irg_entity(irg)));
	}

	void CodeGen::assemble(ir_node* irn)
	{
		ir_node* block = irn;

		if (!is_Block(irn))
			block = get_nodes_block(irn);

		/*if(code.find(block) == code.end())
		{
			//pointless if, but this might be useful for jumps...
			code[block].push_back("jmp .L_" + get_entity_name(get_irg_entity(irg)) + "_" + to_string(get_irn_node_nr(block)));
		}*/

		ir_printf("assembling (%ld) %F in %F\n", get_irn_node_nr(irn), irn, block);
		auto children = FirmInterface::getInstance().getOuts(irn);

		auto it = partial.find(irn);

		if (it != partial.end())
			it->second--;
		else
			partial[irn] = children.size() - 1;

		if (partial[irn] == 0)
		{
			//all children seen - handle it now
			//default: node doesn't use registers. generally wrong, but allows unimplemented nodes to not explode instantly
			set_irn_link(irn, NULL);

			//merge registers of children
			size_t current_reg = 0;

			for (auto& child : children)
			{
				ir_node* key = (ir_node*) get_irn_link(child.first);

				if (!key || !usage[key].second[child.second])
					continue;//no data dependency, skip

				if (!current_reg)
					current_reg = usage[key].second[child.second];
				else
				{
					// we actually have to merge
					size_t merge = usage[key].second[child.second];

					if (current_reg == merge)
						continue;// ... or maybe not

					auto& cur_reg = registers[current_reg];
					auto& merge_reg = registers[merge];

					if (cur_reg.constraint && merge_reg.constraint && cur_reg.constraint != merge_reg.constraint)
					{
						//different constraints: we can't merge, generate move
						code[block].push_back("mov " + std::to_string(current_reg) + " to " + std::to_string(merge));
						//todo: represent this in usage?
						//TODO: remember failed merges, two failed merges might be mergable
					}
					else
					{
						//same constraint or at least one is unconstrained: merge
						cur_reg.constraint = cur_reg.constraint ? cur_reg.constraint : merge_reg.constraint;
						cur_reg.writes.insert(cur_reg.writes.end(), merge_reg.writes.begin(), merge_reg.writes.end());

						for (ir_node* w : merge_reg.writes)
							usage[w].first = current_reg;

						cur_reg.reads.insert(cur_reg.reads.end(), merge_reg.reads.begin(), merge_reg.reads.end());

						for (ir_node* r : merge_reg.reads)
							std::replace(usage[r].second.begin(), usage[r].second.end(), merge, current_reg);
					}
				}
			}

			//actual handling of the node
			//TODO...
			if (is_Return(irn))
			{
				code[block].push_back("return");

				if (get_irn_arity(irn) == 2)
				{
					set_irn_link(irn, irn);
					// return value (which we read) must be in RAX
					usage[irn] = {0, {0, registers.size()}};
					registers.push_back({RAX, {}, {irn}});
				}
				else
					set_irn_link(irn, NULL);
			}
			else if (is_Proj(irn))
			{
				if (get_irn_mode(irn) == mode_M || get_irn_mode(irn) == mode_T)
					;//only required for ordering
				else
				{
					//parent is Proj T T_args: add register contraint
					//parent is Proj T T_result: rax constraint
					//TODO: all other cases(?) ignore Proj
					ir_node* parent = get_irn_n(irn, 0);

					if (is_Proj(parent) && get_irn_mode(parent) == mode_T)
					{
						Constraint c = NONE;

						if (get_irn_n(parent, 0) == get_irg_start(get_irn_irg(irn)))
						{
							// function arguments
							if (get_Proj_num(irn) >= 6)
								printf("more than 6 arguments...");
							else
							{
								static const Constraint arg_order[] = {RDI, RSI, RDX, RCX, R8, R9};
								c = arg_order[get_Proj_num(irn)];
							}
						}
						else
							c = RAX;

						if (!current_reg)
							printf("unused?!");
						else
						{
							auto& reg = registers[current_reg];

							if (reg.constraint == NONE)
							{
								reg.constraint = c;
								usage[irn] = {0, {current_reg}};//pseudo read
							}
							else
							{
								//conflicting constraints, generate move
								code[block].push_back("mov " + std::to_string(registers.size()) + " to " + std::to_string(current_reg));
								usage[irn] = {current_reg, {registers.size()}};
								registers.push_back({c, {}, {irn}});
								reg.writes.push_back(irn);
							}

							set_irn_link(irn, irn);
						}
					}
				}
			}
			else if (is_Start(irn))
				;//we're done
			else
				printf("\tnot implemented yet\n");

			// take care of parents
			for (int i = get_irn_arity(irn) - 1; i >= 0; i--)
				assemble(get_irn_n(irn, i));
		}
	}
}
