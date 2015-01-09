#include <algorithm>
#include <string>

#include "CodeGen.hpp"

#include "FirmInterface.hpp"

//using namespace std::string_literals;

namespace firm
{
	const CodeGen::Constraint CodeGen::arg_order[] = {RDI, RSI, RDX, RCX, R8, R9};

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
	 *   div/mod in operands in rax:rdx, any; result in rax (div), rdx (mod)
	 *
	 * each node puts it's result in a new register
	 * nodes with multiple children merge registers
	 *
	 * irn_link used as boolean indicating register usage or storing jump targets for control flow nodes
	 *
	 * register names:
	 * 64bit    32bit    16bit    8bit
	 * r[acdb]x e[acdb]x [acdb]x  [acdb][hl] (=r[0-3])  !! order is A, C, D, B !!
	 * r[sb]p   e[sb]p   [sb]p    [sb]pl     (=r[4-5])
	 * r[sd]i   e[sd]i   [sd]i    [sd]il     (=r[6-7])
	 * r[8-15]  r[8-15]d r[8-15]w r[8-15]b
	 */

	size_t CodeGen::new_register()
	{
		if (!free_registers.empty())
		{
			size_t reg = *free_registers.begin();
			free_registers.erase(free_registers.begin());
			registers[reg].writes.clear();
			registers[reg].reads.clear();
			return reg;
		}
		else
		{
			size_t reg = registers.size();
			registers.push_back({{}, {}});
			return reg;
		}
	}

	void CodeGen::merge_register(size_t a, size_t b, bool add_to_free)
	{
		printf("\t\tmerging %zu and %zu\n", a, b);

		if (a == b)
			return;

		auto& a_reg = registers[a];
		auto& b_reg = registers[b];

		printf("\t\twrites: ");

		for (auto& w : b_reg.writes)
		{
			ir_printf("[%p %F]: ", w, w);

			for (auto& ww : usage[w].first)
			{
				printf("%zu", ww.reg);

				if (ww.reg == b)
				{
					ww.reg = a;
					printf("->%zu", ww.reg);
				}

				printf(", ");
			}

			printf("; ");
		}

		a_reg.writes.insert(a_reg.writes.end(), b_reg.writes.begin(), b_reg.writes.end());

		printf("\n\t\treads: ");

		for (auto& r : b_reg.reads)
		{
			ir_printf("[%p %F]: ", r, r);

			for (auto& rr : usage[r].second)
			{
				printf("%zu", rr.reg);

				if (rr.reg == b)
				{
					rr.reg = a;
					printf("->%zu", rr.reg);
				}

				printf(", ");
			}

			printf("; ");
		}

		printf("\n");
		a_reg.reads.insert(a_reg.reads.end(), b_reg.reads.begin(), b_reg.reads.end());

		if (add_to_free)
			free_registers.insert(b);
	}

	char const* CodeGen::constraintToRegister(Constraint c, ir_mode* mode)
	{
		unsigned size = get_mode_size_bytes(mode);

		switch (size)
		{
			case 1:
				size = 3;
				break;

			case 2:
				size = 2;
				break;

			case 4:
				size = 1;
				break;

			case 8:
				size = 0;
				break;

			default:
				return "!!!";
		}

		static char const* registers[4][17] =
		{
			// must be in sync with Constraint enum
			{"!!!NONE!!!", "%rax", "%rcx", "%rdx", "%rbx", "%rsp", "%rbp", "%rsi", "%rdi", "%r8", "%r9", "%r10", "%r11", "%r12", "%r13", "%r14", "%r15"},
			{"!!!NONE!!!", "%eax", "%ecx", "%edx", "%ebx", "%esp", "%ebp", "%esi", "%edi", "%r8d", "%r9d", "%r10d", "%r11d", "%r12d", "%r13d", "%r14d", "%r15d"},
			{"!!!NONE!!!", "%ax", "%cx", "%dx", "%bx", "%sp", "%bp", "%si", "%di", "%r8w", "%r9w", "%r10w", "%r11w", "%r12w", "%r13w", "%r14w", "%r15w"},
			{"!!!NONE!!!", "%al", "%cl", "%dl", "%bl", "%spl", "%bpl", "%sil", "%dil", "%r8b", "%r9b", "%r10b", "%r11b", "%r12b", "%r13b", "%r14b", "%r15b"}
		};
		return registers[size][c];
	}

	char const* CodeGen::operationSuffix(ir_mode* mode)
	{
		switch (get_mode_size_bytes(mode))
		{
			case 1:
				return "b";

			case 2:
				return "w";

			case 4:
				return "l";

			case 8:
				return "q";

			default:
				return "!!!";
		}
	}

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
		free_registers.clear();
		code.clear();

		// fake register for non-data parents
		registers.push_back({{}, {}});

		edges_activate(irg);

		//force phi ordering
		//code for phis in a specific block must be generated after all reads from that phi in the same block
		irg_walk_graph(irg, [](ir_node * irn, void*)
		{
			if (!is_Phi(irn) || get_irn_mode(irn) == mode_M)
				return;

			std::set<ir_node*> phi_blocks;
			ir_node* block = get_nodes_block(irn);

			for (int i = 0; i < get_irn_arity(irn); i++)
				phi_blocks.insert(get_nodes_block(get_irn_n(block, i)));

			for (auto& e : FirmInterface::getInstance().getOuts(irn))
			{
				if (e.first != irn && phi_blocks.count(get_nodes_block(e.first)))
					add_irn_dep(irn, e.first);
			}
		}, NULL, NULL
		              );
		dump_ir_graph(irg, "dep");

		//handle keepalives
		ir_node* en = get_irg_end(irg);
		set_irn_link(en, NULL);

		for (int i = 0; i < get_irn_arity(en); i++)
		{
			ir_node* enp = get_irn_n(en, i);

			if (is_Block(enp) || is_Cmp(enp) || is_Cond(enp) || get_irn_mode(enp) == mode_X)
				stack_cf.push(enp);
			else if (is_Phi(enp))
				stack_phi.push(enp);
			else
				stack_normal.push(enp);
		}

		// no code for end block, just find returns
		ir_node* eb = get_irg_end_block(irg);
		set_irn_link(eb, NULL);

		for (int i = 0; i < get_irn_arity(eb); i++)
			stack_cf.push(get_irn_n(eb, i));

		auto print_stack = [&]
		{
			printf("TODO: ");
			std::stack<ir_node*> tmp = stack_cf;

			while (!tmp.empty())
			{
				printf("%ld, ", get_irn_node_nr(tmp.top()));
				tmp.pop();
			}

			printf("// ");
			std::stack<ir_node*> tmp2 = stack_phi;

			while (!tmp2.empty())
			{
				printf("%ld, ", get_irn_node_nr(tmp2.top()));
				tmp2.pop();
			}

			printf("// ");
			std::stack<ir_node*> tmp3 = stack_normal;

			while (!tmp3.empty())
			{
				printf("%ld, ", get_irn_node_nr(tmp3.top()));
				tmp3.pop();
			}

			printf("\n");
		};

		while (!stack_cf.empty() || !stack_phi.empty() || !stack_normal.empty())
		{
			while (!stack_cf.empty() || !stack_phi.empty())
			{
				while (!stack_cf.empty())
				{
					print_stack();

					ir_node* irn = stack_cf.top();
					stack_cf.pop();
					assemble(irn);
				}

				print_stack();

				if (!stack_phi.empty())
				{
					ir_node* irn = stack_phi.top();
					stack_phi.pop();
					assemble(irn);
				}
			}

			print_stack();

			if (!stack_normal.empty())
			{
				ir_node* irn = stack_normal.top();
				stack_normal.pop();
				assemble(irn);
			}
		}

		edges_deactivate(irg);

		//******** register use debug/dump code pt 1 begin
		printf("----------\n");

		for (auto& reg : registers)
		{
			printf("writes: ");

			for (ir_node* w : reg.writes)
				ir_printf("%F, ", w);

			printf("\nreads: ");

			for (ir_node* r : reg.reads)
				ir_printf("%F, ", r);

			printf("\n\n");
		}

		printf("free: ");

		for (size_t f : free_registers)
			printf("%zu, ", f);

		printf("\n\n");

		for (auto& u : usage)
		{
			ir_printf("%p - %F:\n\twrites: ", u.first, u.first);

			for (auto& w : u.second.first)
				printf("[%s, %zu], ", constraintToRegister(w.constraint, mode_P), w.reg);

			printf("\n\treads: ");

			for (auto& r : u.second.second)
				printf("[%s, %zu], ", constraintToRegister(r.constraint, mode_P), r.reg);

			printf("\n");
		}

		printf("\n----------\ncondensing\n----------\n");
		//******** register use debug/dump code pt 1 end

		// register condensing
		while (!free_registers.empty())
		{
			printf("free: %zu\n", free_registers.size());

			if (*free_registers.rbegin() == registers.size() - 1)
			{
				printf("\tdropping %zu at end\n", *free_registers.rbegin());
				free_registers.erase(--free_registers.end());
			}
			else
			{
				size_t reg = *free_registers.begin();
				printf("\tfilling %zu with last (%zu)\n", *free_registers.begin(), registers.size() - 1);
				free_registers.erase(free_registers.begin());
				registers[reg].writes.clear();
				registers[reg].reads.clear();
				merge_register(reg, registers.size() - 1, false);
			}

			registers.resize(registers.size() - 1);
		}

		//******** register use debug/dump code pt 2 begin
		for (auto& reg : registers)
		{
			printf("writes: ");

			for (ir_node* w : reg.writes)
				ir_printf("%F, ", w);

			printf("\nreads: ");

			for (ir_node* r : reg.reads)
				ir_printf("%F, ", r);

			printf("\n\n");
		}

		printf("free: ");

		for (size_t f : free_registers)
			printf("%zu, ", f);

		printf("\n\n");

		for (auto& u : usage)
		{
			ir_printf("%p - %F:\n\twrites: ", u.first, u.first);

			for (auto& w : u.second.first)
				printf("[%s, %zu], ", constraintToRegister(w.constraint, mode_P), w.reg);

			printf("\n\treads: ");

			for (auto& r : u.second.second)
				printf("[%s, %zu], ", constraintToRegister(r.constraint, mode_P), r.reg);

			printf("\n");
		}

		//******** register use debug/dump code pt 2 end

		output(irg);
	}

	void CodeGen::assemble(ir_node* irn)
	{
		auto push_node = [&](ir_node * irn)
		{
			if (is_Block(irn) || is_Cmp(irn) || is_Cond(irn) || get_irn_mode(irn) == mode_X)
				stack_cf.push(irn);
			else if (is_Phi(irn))
				stack_phi.push(irn);
			else
				stack_normal.push(irn);
		};

		ir_node* block = irn;

		if (!is_Block(irn))
			block = get_nodes_block(irn);

		if (get_irn_link(block) != block)
		{
			// handle the block
			// requeue the node if it isn't the block itself
			if (irn != block)
				push_node(irn);

			set_irn_link(block, block);

			printf("Block %ld - phis\n", get_irn_node_nr(block));

			for (ir_node* phi = get_Block_phis(block); phi && is_Phi(phi); phi = get_Phi_next(phi))
			{
				ir_printf("\t%ld: %F\n", get_irn_node_nr(phi), phi);
				auto it = partial.find(phi);

				if (it != partial.end())
					it->second++;
				else
					partial[phi] = 1 + FirmInterface::getInstance().getOuts(phi).size();

				stack_phi.push(phi);
			}

			// take care of block parents
			printf("Block %ld - control flow\n", get_irn_node_nr(block));

			for (int i = 0; i < get_irn_arity(block); i++)
			{
				ir_printf("\t%ld: %F\n", get_irn_node_nr(get_irn_n(block, i)), get_irn_n(block, i));
				stack_cf.push(get_irn_n(block, i));
			}

			printf("Block %ld - done\n", get_irn_node_nr(block));
			return;
		}

		auto children = FirmInterface::getInstance().getOuts(irn);
		auto children_dep = FirmInterface::getInstance().getOuts(irn, EDGE_KIND_DEP);
		children.insert(children.end(), children_dep.begin(), children_dep.end());

		auto it = partial.find(irn);

		if (it != partial.end())
			it->second--;
		else
		{
			partial[irn] = children.size() - 1;

			for (auto& child : children)
			{
				if (is_Anchor(child.first))
					partial[irn]--;
			}

			if (is_Start(irn))
				// start nodes seem to have an extra Proj T and Proj P64 child...
				partial[irn] -= 2;
		}

		ir_printf("testing (%ld) %F, %zu of %zu left\n", get_irn_node_nr(irn), irn, partial[irn], children.size());

		if (partial[irn] == 0 || (is_Phi(irn) && get_irn_link(irn) != irn))
		{
			// take care of parents
			if (!is_Phi(irn) || (is_Phi(irn) && get_irn_link(irn) != irn))
			{
				printf("\tqueueing:\n");

				for (int i = 0; i < get_irn_n_deps(irn); i++)
				{
					ir_node* p = get_irn_dep(irn, i);
					push_node(p);

					ir_printf("\t\t(%lu) %F %p\n", get_irn_node_nr(p), p, p);
				}

				for (int i = 0; i < get_irn_arity(irn); i++)
				{
					ir_node* p = get_irn_n(irn, i);
					push_node(p);

					ir_printf("\t\t(%lu) %F %p\n", get_irn_node_nr(p), p, p);
				}
			}

			ir_printf("assembling (%ld) %F in %F\n", get_irn_node_nr(irn), irn, block);
			//all children seen - handle it now
			//phis are handeled twice, first directly after generating control flow for generating code
			//    second (like all other nodes) when all children have been handled, for merging the output registers

			//merge registers of children
			size_t current_reg = 0;

			if (is_Phi(irn) && get_irn_mode(irn) != mode_M)
			{
				// force a phi output register (excluding memory phis)
				// (if we don't need one the phi shouldn't exist...)
				if (get_irn_link(irn) == irn)
					current_reg = usage[irn].first[0].reg;
				else
				{
					current_reg = new_register();
					usage[irn].first.push_back({NONE, current_reg});
					registers[current_reg].writes.push_back(irn);
				}
			}
			else
				//default: node doesn't use registers. generally wrong, but allows unimplemented nodes to not explode instantly
				set_irn_link(irn, NULL);

			// don't merge: memory projs/phis (note: div/mod/load/... are tuples), control flow, basic blocks, the start node and its children, compares (control flow children but not mode_X itself)
			// (basicaly all things that never needs (output) registers or mustn't have them merged (start))
			if (get_irn_mode(irn) != mode_M
			        && get_irn_mode(irn) != mode_X
			        && get_irn_mode(irn) != mode_BB
			        && !(get_irn_mode(irn) == mode_T && (irn == get_irg_start(get_irn_irg(irn)) || get_irn_n(irn, 0) == get_irg_start(get_irn_irg(irn))))
			        && !is_Cmp(irn))
			{
				for (auto& child : FirmInterface::getInstance().getOuts(irn))
				{
					ir_node* key = (ir_node*) get_irn_link(child.first);

					if (!key || is_Anchor(child.first) || !usage[child.first].second[child.second].reg)
						continue;//no data dependency, skip

					if (!current_reg)
						current_reg = usage[child.first].second[child.second].reg;
					else
					{
						// we actually have to merge
						size_t merge = usage[child.first].second[child.second].reg;
						merge_register(current_reg, merge);
					}
				}
			}

			//actual handling of the node
			if (is_Phi(irn))
			{
				if (get_irn_mode(irn) != mode_M && get_irn_link(irn) != irn)
				{
					// for each parent: ensure control flow is done
					bool cf = false;

					for (int pred = 0; pred < get_irn_arity(irn); pred++)
					{
						ir_node* pblock = get_nodes_block(get_irn_n(block, pred));

						if (get_irn_link(pblock) != pblock)
						{
							if (!cf)
								stack_phi.push(irn);

							cf = true;
							stack_cf.push(pblock);
						}
					}

					if (cf)
						// undone control flow
						return;

					// for each parent: add mov/load new_reg -> phi_reg in parent block
					for (int pred = 0; pred < get_irn_arity(irn); pred++)
					{
						code[get_nodes_block(get_irn_n(block, pred))].push_back(irn);
						size_t a = is_Const(get_irn_n(irn, pred)) ? 0 : new_register();
						usage[irn].second.push_back({NONE, a});

						if (a)
							registers[a].reads.push_back(irn);
					}
				}

				set_irn_link(irn, irn);
			}
			else if (is_Return(irn))
			{
				if (get_irn_arity(irn) == 2)
				{
					// return value (which we read) must be in RAX
					set_irn_link(irn, irn);
					size_t reg = is_Const(get_irn_n(irn, 1)) ? 0 : new_register();
					usage[irn] = {{}, {{NONE, 0}, {RAX, reg}}};

					if (reg)
						registers[reg].reads.push_back(irn);
				}

				code[block].push_back(irn);
			}
			else if (is_Cmp(irn))
			{
				size_t a = is_Const(get_irn_n(irn, 0)) ? 0 : new_register();
				size_t b = is_Const(get_irn_n(irn, 1)) ? 0 : new_register();
				usage[irn] = {{}, {{NONE, a}, {NONE, b}}};

				if (a)
					registers[a].reads.push_back(irn);

				if (b)
					registers[b].reads.push_back(irn);

				// find true jump target
				// cmp <- cond <- proj true/false <- block
				for (auto& cc : FirmInterface::getInstance().getOuts(children[0].first))
				{
					if (get_Proj_num(cc.first) == pn_Cond_true)
					{
						set_irn_link(irn, FirmInterface::getInstance().getOuts(cc.first)[0].first);
						break;
					}
				}

				code[block].push_back(irn);
			}
			else if (is_Cond(irn))
			{
				// find false jump target
				// [cmp <-] cond <- proj true/false <- block
				for (auto& cc : children)
				{
					if (get_Proj_num(cc.first) == pn_Cond_false)
					{
						set_irn_link(irn, FirmInterface::getInstance().getOuts(cc.first)[0].first);
						break;
					}
				}

				code[block].push_back(irn);
			}
			else if (is_Jmp(irn))
			{
				set_irn_link(irn, children[0].first);
				code[block].push_back(irn);
			}
			else if (is_Proj(irn))
			{
				if (get_irn_mode(irn) == mode_M || get_irn_mode(irn) == mode_T || get_irn_mode(irn) == mode_X)
					;//only required for ordering
				else if (current_reg)
				{
					set_irn_link(irn, irn);
					usage[irn] = {{}, {{NONE, current_reg}}};//pseudo read, so Start, Call, Load nodes can find their registers
					registers[current_reg].reads.push_back(irn);
				}
			}
			else if (is_Conv(irn))
			{
				set_irn_link(irn, irn);

				// we need code for conv if the new mode is wider (either sign or zero extend, based on the old mode)
				if (get_mode_size_bytes(get_irn_mode(get_irn_n(irn, 0))) < get_mode_size_bytes(get_irn_mode(irn)))
				{
					size_t a = new_register();
					usage[irn] = {{{NONE, current_reg}}, {{NONE, a}}};
					registers[a].reads.push_back(irn);
					registers[current_reg].writes.push_back(irn);
					code[block].push_back(irn);
				}
				else
				{
					usage[irn] = {{}, {{NONE, current_reg}}};//pseudo read, so parent nodes can find their registers
					registers[current_reg].reads.push_back(irn);
				}
			}
			else if (is_Start(irn))
			{
				std::vector<Access> writes;

				for (auto& child : FirmInterface::getInstance().getOuts(irn))
				{
					ir_printf("\t\t(%lu) %F %p\n", get_irn_node_nr(child.first), child.first, child.first);

					if (!is_Proj(child.first) || get_irn_mode(child.first) != mode_T || get_Proj_num(child.first) != pn_Start_T_args)
						continue;

					auto used_args = FirmInterface::getInstance().getOuts(child.first);

					for (auto& used_arg : used_args)
					{
						ir_printf("\t\t\t(%lu) %F %p\n", get_irn_node_nr(used_arg.first), used_arg.first, used_arg.first);

						if (!is_Proj(used_arg.first))
							continue;

						// function arguments
						if (get_Proj_num(used_arg.first) >= 6)
						{
							//TODO: 7th.. args are on stack, latter args at higher addresses
							printf("more than 6 arguments...\n");
							abort();
						}

						Constraint c = arg_order[get_Proj_num(used_arg.first)];
						size_t reg = usage[used_arg.first].second[0].reg;
						registers[reg].writes.push_back(irn);
						writes.push_back({c, reg});
					}
				}

				usage[irn] = {writes, {}};
				code[block].push_back(irn);
			}
			else if (is_Call(irn))
			{
				std::vector<Access> writes;

				for (auto& child : FirmInterface::getInstance().getOuts(irn))
				{
					if (get_irn_mode(child.first) == mode_M)
						continue;

					auto return_tuple = FirmInterface::getInstance().getOuts(child.first);

					for (auto& used_return : return_tuple)
					{
						if (!get_irn_link(used_return.first))
							continue;

						// call return value
						size_t reg = usage[used_return.first].second[0].reg;
						registers[reg].writes.push_back(irn);
						writes.push_back({RAX, reg});
					}
				}

				std::vector<Access> reads = {{NONE, 0}, {NONE, 0}};

				if (get_irn_arity(irn) > 8)
				{
					printf("more than 6 arguments...\n");
					abort();
				}

				for (int i = 2; i < std::min(8, get_irn_arity(irn)); i++)
				{
					size_t reg = is_Const(get_irn_n(irn, i)) ? 0 : new_register();
					reads.push_back({arg_order[i - 2], reg});

					if (reg)
						registers[reg].reads.push_back(irn);
				}

				set_irn_link(irn, irn);
				usage[irn] = {writes, reads};
				code[block].push_back(irn);
			}
			else if (is_Add(irn) || is_Sub(irn) || is_Mul(irn))
			{
				set_irn_link(irn, irn);
				size_t a = is_Const(get_irn_n(irn, 0)) ? 0 : new_register();
				size_t b = is_Const(get_irn_n(irn, 1)) ? 0 : new_register();
				registers[current_reg].writes.push_back(irn);

				if (a)
					registers[a].reads.push_back(irn);

				if (b)
					registers[b].reads.push_back(irn);

				usage[irn] = {{{NONE, current_reg}}, {{NONE, a}, {NONE, b}}};
				code[block].push_back(irn);
			}
			else if (is_Minus(irn))
			{
				set_irn_link(irn, irn);
				size_t a = is_Const(get_irn_n(irn, 0)) ? 0 : new_register();
				registers[current_reg].writes.push_back(irn);

				if (a)
					registers[a].reads.push_back(irn);

				usage[irn] = {{{NONE, current_reg}}, {{NONE, a}}};
				code[block].push_back(irn);
			}
			else if (is_Div(irn) || is_Mod(irn))
			{
				set_irn_link(irn, irn);
				size_t a = is_Const(get_irn_n(irn, 1)) ? 0 : new_register();
				size_t b = is_Const(get_irn_n(irn, 2)) ? 0 : new_register();
				registers[current_reg].writes.push_back(irn);

				if (a)
					registers[a].reads.push_back(irn);

				if (b)
					registers[b].reads.push_back(irn);

				usage[irn] = {{{(is_Div(irn) ? RAX : RDX), current_reg}}, {{NONE, 0}, {RAX, a}, {NONE, b}}};
				code[block].push_back(irn);
			}
			else if (is_Load(irn))
			{
				if (!current_reg)
					printf("\tLoad with unused value?!\n");
				else
				{
					set_irn_link(irn, irn);
					size_t addr = new_register();
					registers[addr].reads.push_back(irn);
					registers[current_reg].writes.push_back(irn);
					usage[irn] = {{{NONE, current_reg}}, {{NONE, 0}, {NONE, addr}}};
					code[block].push_back(irn);
				}
			}
			else if (is_Store(irn))
			{
				set_irn_link(irn, irn);
				size_t addr = new_register();
				size_t val = is_Const(get_irn_n(irn, 2)) ? 0 : new_register();
				registers[addr].reads.push_back(irn);

				if (val)
					registers[val].reads.push_back(irn);

				usage[irn] = {{}, {{NONE, 0}, {NONE, addr}, {NONE, val}}};
				code[block].push_back(irn);
			}
			else if (is_Address(irn) || is_Const(irn))
			{
				if (current_reg)
				{
					printf("!!!!!!!!!!!!!\n\tthis shouldn't use a register...\n");
					abort();
				}
			}
			else
			{
				printf("\tnot implemented yet\n");
				abort();
			}
		}
		else
		{
			printf("\twaiting for:\n");

			for (auto& child : children)
				ir_printf("\t\t(%lu) %F %p\n", get_irn_node_nr(child.first), child.first, child.first);
		}
	}

	void CodeGen::load_or_imm(ir_node* node, size_t reg)
	{
		if (is_Const(node))
			fprintf(out, "$%ld", get_tarval_long(get_Const_tarval(node)));
		else
			fprintf(out, "%zd(%%rsp)", 8 * reg - 8);
	}

	void CodeGen::gen_bin_op(ir_node* irn, ir_mode* mode, char const* op)
	{
		char const* os = operationSuffix(mode);
		char const* rs = constraintToRegister(RAX, mode);

		fprintf(out, "\tmov%s ", os);
		load_or_imm(get_irn_n(irn, 0), usage[irn].second[0].reg);
		fprintf(out, ", %s\n\t%s%s ", rs, op, os);
		load_or_imm(get_irn_n(irn, 1), usage[irn].second[1].reg);
		fprintf(out, ", %s\n", rs);
	}

	void CodeGen::output(ir_graph* irg)
	{
		char const* name = get_entity_name(get_irg_entity(irg));
		fprintf(out, "\t.p2align 4,,15\n\t.globl  %s\n\t.type   %s, @function\n%s:\n", name, name, name);

		// adjust stack
		if (registers.size() > 1)
			fprintf(out, "\tsub $%zd, %%rsp\n", 8 * registers.size() - 8);

		for (auto& block : code)
		{
			fprintf(out, ".L_%s_%zu:\n", get_entity_name(get_irg_entity(irg)), get_irn_node_nr(block.first));

			for (auto it = block.second.rbegin(); it != block.second.rend(); it++)
				output(*it, block.first);
		}

		fprintf(out, "\t.size   %s, .-%s\n\n", name, name);
	}

	void CodeGen::output(ir_node* irn, ir_node* block)
	{
		ir_fprintf(out, "\t# %F\n", irn);

		if (is_Phi(irn))
		{
			// the only thing that needs the block argument...
			ir_node* phi_block = get_nodes_block(irn);
			int pred;

			for (pred = 0; pred < get_irn_arity(phi_block); pred++)
			{
				if (get_nodes_block(get_irn_n(phi_block, pred)) == block)
					break;
			}

			ir_node* value = get_irn_n(irn, pred);
			ir_mode* value_mode = get_irn_mode(value);

			if (is_Const(value))
				fprintf(out, "\tmov%s $%ld, %zd(%%rsp)\n", operationSuffix(value_mode), get_tarval_long(get_Const_tarval(value)), 8 * usage[irn].first[0].reg - 8);
			else
			{
				fprintf(out, "\tmov%s %zd(%%rsp), %s\n", operationSuffix(value_mode), 8 * usage[irn].second[pred].reg - 8, constraintToRegister(RAX, value_mode));
				fprintf(out, "\tmov%s %s, %zd(%%rsp)\n", operationSuffix(value_mode), constraintToRegister(RAX, value_mode), 8 * usage[irn].first[0].reg - 8);
			}
		}
		else if (is_Return(irn))
		{
			if (get_irn_arity(irn) == 2)
			{
				ir_node* value = get_irn_n(irn, 1);
				ir_mode* value_mode = get_irn_mode(value);
				fprintf(out, "\tmov%s ", operationSuffix(value_mode));
				load_or_imm(value, usage[irn].second[1].reg);
				fprintf(out, ", %s\n", constraintToRegister(RAX, value_mode));
			}

			// free stack
			if (registers.size() > 1)
				fprintf(out, "\tadd $%zd, %%rsp\n", 8 * registers.size() - 8);

			// and return
			fprintf(out, "\tret\n");
		}
		else if (is_Cmp(irn))
		{
			//          <  <=  =  !=  >=  >
			//signed:   l  le  e  ne  ge  g
			//unsigned: b  be  e  ne  ae  a
			ir_mode* mode = get_irn_mode(get_irn_n(irn, 0));
			char const* cond;

			switch (get_Cmp_relation(irn))
			{
				case ir_relation_unordered:
				case ir_relation_false:
					// if there were a jump never instruction...
					// workaround: jump always and get target from cond node
					cond = "mp";
					set_irn_link(irn, get_irn_link(FirmInterface::getInstance().getOuts(irn)[0].first));
					break;

				case ir_relation_unordered_equal:
				case ir_relation_equal:
					cond = "e";
					break;

				case ir_relation_unordered_greater:
				case ir_relation_greater:
					cond = get_mode_sign(mode) ? "g" : "a";
					break;

				case ir_relation_unordered_greater_equal:
				case ir_relation_greater_equal:
					cond = get_mode_sign(mode) ? "ge" : "ae";
					break;

				case ir_relation_unordered_less:
				case ir_relation_less:
					cond = get_mode_sign(mode) ? "l" : "b";
					break;

				case ir_relation_unordered_less_equal:
				case ir_relation_less_equal:
					cond = get_mode_sign(mode) ? "le" : "be";
					break;

				case ir_relation_true:
				case ir_relation_less_equal_greater:
					cond = "mp";
					break;

				case ir_relation_unordered_less_greater:
				case ir_relation_less_greater:
					cond = "ne";
					break;

				default:
					// needed? all enum values should be present, but we're talking C here...
					cond = "???default???";
					break;
			}

			// notes:
			// sub a, b stores b - a in b, same for cmp
			// cmp a, b  /  jl foo   jumps when b < a
			gen_bin_op(irn, mode, "cmp");
			fprintf(out, "\tj%s .L_%s_%ld\n", cond, get_entity_name(get_irg_entity(get_irn_irg(irn))), get_irn_node_nr((ir_node*) get_irn_link(irn)));
		}
		else if (is_Cond(irn) || is_Jmp(irn))
			fprintf(out, "\tjmp .L_%s_%ld\n", get_entity_name(get_irg_entity(get_irn_irg(irn))), get_irn_node_nr((ir_node*) get_irn_link(irn)));
		else if (is_Start(irn))
		{
			auto& writes = usage[irn].first;

			for (auto& w : writes)
			{
				if (w.reg)
					//TODO: get actual mode (from start <- proj T args <- proj wanted_mode arg_n ?)
					fprintf(out, "\tmov %s, %zd(%%rsp)\n", constraintToRegister(w.constraint, mode_P), 8 * w.reg - 8);
			}
		}
		else if (is_Call(irn))
		{
			auto& reads = usage[irn].second;

			for (int i = 2; i < get_irn_arity(irn); i++)
			{
				ir_mode* arg_mode = get_irn_mode(get_irn_n(irn, i));
				fprintf(out, "\tmov%s ", operationSuffix(arg_mode));
				load_or_imm(get_irn_n(irn, i), reads[i].reg);
				fprintf(out, ", %s\n", constraintToRegister(reads[i].constraint, arg_mode));
			}

			fprintf(out, "\tcall %s\n", get_entity_name(get_Call_callee(irn)));

			if (!usage[irn].first.empty())
				//TODO: get actual mode (from call <- proj T result <- proj wanted_mode 0 ?)
				fprintf(out, "\tmov %%rax, %zd(%%rsp)\n", 8 * usage[irn].first[0].reg - 8);
		}
		else if (is_Add(irn) || is_Sub(irn) || is_Mul(irn))
		{
			char const* op = is_Add(irn) ? "add" : is_Sub(irn) ? "sub" : "imul";
			ir_mode* mode = get_irn_mode(irn);
			char const* os = operationSuffix(mode);
			char const* rs = constraintToRegister(RAX, mode);
			//sub a, b stores b - a in b -> output "op second first"
			gen_bin_op(irn, mode, op);
			fprintf(out, "\tmov%s %s, %zd(%%rsp)\n", os, rs, 8 * usage[irn].first[0].reg - 8);
		}
		else if (is_Minus(irn))
		{
			ir_mode* mode = get_irn_mode(irn);
			char const* os = operationSuffix(mode);
			char const* rs = constraintToRegister(RAX, mode);
			fprintf(out, "\tmov%s ", os);
			load_or_imm(get_irn_n(irn, 0), usage[irn].second[0].reg);
			fprintf(out, ", %s\n\tneg%s %s\n", rs, os, rs);
			fprintf(out, "\tmov%s %s, %zd(%%rsp)\n", os, rs, 8 * usage[irn].first[0].reg - 8);
		}
		else if (is_Div(irn) || is_Mod(irn))
		{
			ir_mode* mode = is_Div(irn) ? get_Div_resmode(irn) : get_Mod_resmode(irn);
			char const* os = operationSuffix(mode);
			char const* rs = constraintToRegister(RBX, mode);
			char const* conv;

			switch (get_mode_size_bytes(mode))
			{
				case 2:
					conv = "wd";
					break;

				case 4:
					conv = "dq";
					break;

				case 8:
					conv = "qo";
					break;

				default:
					//TODO: no idea how to handle 8 bit operands (cbw doesn't exist) (luckily we'll never need them)
					//and if we ever need an unsigned div we need something else too
					conv = "??";
					break;
			}

			fprintf(out, "\tmov%s ", os);
			load_or_imm(get_irn_n(irn, 1), usage[irn].second[1].reg);
			fprintf(out, ", %s\n\tmov%s ", constraintToRegister(RAX, mode), os);
			load_or_imm(get_irn_n(irn, 2), usage[irn].second[2].reg);
			fprintf(out, ", %s\n\tc%s\n\tidiv%s %s\n", rs, conv, os, rs);
			fprintf(out, "\tmov%s %s, %zd(%%rsp)\n", os, constraintToRegister(is_Div(irn) ? RAX : RDX, mode), 8 * usage[irn].first[0].reg - 8);
		}
		else if (is_Conv(irn))
		{
			// conversion to a larger mode, zero or sign expand depending on signedness of smaller node
			ir_mode* old_mode = get_irn_mode(get_irn_n(irn, 0));
			char const* old_rs = constraintToRegister(RAX, old_mode);
			ir_mode* mode = get_irn_mode(irn);
			char const* rs = constraintToRegister(RAX, mode);

			fprintf(out, "\tmov%s ", operationSuffix(old_mode));
			load_or_imm(get_irn_n(irn, 0), usage[irn].second[0].reg);
			fprintf(out, ", %s\n\tmov%cx %s, %s\n", old_rs, get_mode_sign(old_mode) ? 's' : 'z', old_rs, rs);
			fprintf(out, "\tmov%s %s, %zd(%%rsp)\n", operationSuffix(mode), rs, 8 * usage[irn].first[0].reg - 8);
		}
		else if (is_Load(irn))
		{
			ir_mode* mode = get_Load_mode(irn);
			char const* os = operationSuffix(mode);
			char const* rs = constraintToRegister(RAX, mode);
			fprintf(out, "\tmov %zd(%%rsp), %%rax\n", 8 * usage[irn].second[1].reg - 8);
			fprintf(out, "\tmov%s (%%rax), %s\n", os, rs);
			fprintf(out, "\tmov%s %s, %zd(%%rsp)\n", os, rs, 8 * usage[irn].first[0].reg - 8);
		}
		else if (is_Store(irn))
		{
			ir_mode* mode = get_irn_mode(get_irn_n(irn, 2));
			char const* os = operationSuffix(mode);
			char const* rs = constraintToRegister(RBX, mode);
			fprintf(out, "\tmov %zd(%%rsp), %%rax\n\tmov%s ", 8 * usage[irn].second[1].reg - 8, os);
			load_or_imm(get_irn_n(irn, 2), usage[irn].second[2].reg);
			fprintf(out, ", %s\n\tmov%s %s, (%%rax)\n", rs, os, rs);
		}
		else
		{
			ir_printf("\nNo idea how to emit code for %F\n", irn);
			abort();
		}
	}
}
