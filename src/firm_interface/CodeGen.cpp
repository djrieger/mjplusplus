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
	 *   div/mod in ?
	 *
	 * each node puts it's result in a new register
	 * phis and nodes with multiple children merge registers
	 *
	 *
	 * irn_link used as key for register usage lookup
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

	char const* CodeGen::constraintToRegister(Constraint c)
	{
		static std::vector<char const*> registers = {"!!!NONE!!!", "%rax", "%rcx", "%rdx", "%rdi", "%rsi", "%r8", "%r9"};
		return registers[c];
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
		// no code for end block, just find returns
		ir_node* eb = get_irg_end_block(irg);
		set_irn_link(eb, NULL);

		for (int i = get_irn_arity(eb) - 1; i >= 0; i--)
			assemble(get_irn_n(eb, i));

		//TODO: keepalives

		edges_deactivate(irg);

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
				printf("[%s, %zu], ", constraintToRegister(w.constraint), w.reg);

			printf("\n\treads: ");

			for (auto& r : u.second.second)
				printf("[%s, %zu], ", constraintToRegister(r.constraint), r.reg);

			printf("\n");
		}

		printf("\n----------\ncondensing\n----------\n");

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
				printf("[%s, %zu], ", constraintToRegister(w.constraint), w.reg);

			printf("\n\treads: ");

			for (auto& r : u.second.second)
				printf("[%s, %zu], ", constraintToRegister(r.constraint), r.reg);

			printf("\n");
		}

		output(irg);
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

		auto children = FirmInterface::getInstance().getOuts(irn);

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

		if (partial[irn] == 0)
		{
			ir_printf("assembling (%ld) %F in %F\n", get_irn_node_nr(irn), irn, block);
			//all children seen - handle it now
			//default: node doesn't use registers. generally wrong, but allows unimplemented nodes to not explode instantly
			set_irn_link(irn, NULL);

			//merge registers of children
			size_t current_reg = 0;

			for (auto& child : children)
			{
				ir_node* key = (ir_node*) get_irn_link(child.first);

				if (!key || is_Anchor(child.first) || !usage[key].second[child.second].reg)
					continue;//no data dependency, skip

				if (!current_reg)
					current_reg = usage[key].second[child.second].reg;
				else
				{
					// we actually have to merge
					size_t merge = usage[key].second[child.second].reg;
					merge_register(current_reg, merge);
				}
			}

			//actual handling of the node
			//TODO: complete this
			//missing: phis, booleans and control flow
			if (is_Return(irn))
			{
				if (get_irn_arity(irn) == 2)
				{
					set_irn_link(irn, irn);
					// return value (which we read) must be in RAX
					size_t reg  = new_register();
					usage[irn] = {{}, {{NONE, 0}, {RAX, reg}}};
					registers[reg].reads.push_back(irn);
				}

				code[block].push_back(irn);
			}
			else if (is_Proj(irn))
			{
				if (get_irn_mode(irn) == mode_M || get_irn_mode(irn) == mode_T)
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
				usage[irn] = {{}, {{NONE, current_reg}}};//pseudo read, so parent nodes can find their registers
				registers[current_reg].reads.push_back(irn);
			}
			else if (is_Start(irn))
			{
				std::vector<Access> writes;

				for (auto& child : children)
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
							printf("more than 6 arguments...");
							continue;
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

				for (auto& child : children)
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

				for (int i = 2; i < get_irn_arity(irn); i++)
				{
					size_t reg = new_register();
					reads.push_back({arg_order[i - 2], reg});
					registers[reg].reads.push_back(irn);
				}

				set_irn_link(irn, irn);
				usage[irn] = {writes, reads};
				code[block].push_back(irn);
			}
			else if (is_Const(irn))
			{
				set_irn_link(irn, irn);
				usage[irn] = {{{NONE, current_reg}}, {}};
				registers[current_reg].writes.push_back(irn);
				code[block].push_back(irn);
			}
			else if (is_Add(irn) || is_Sub(irn) || is_Mul(irn))
			{
				set_irn_link(irn, irn);
				size_t a = new_register();
				size_t b = new_register();
				registers[current_reg].writes.push_back(irn);
				registers[a].reads.push_back(irn);
				registers[b].reads.push_back(irn);
				usage[irn] = {{{NONE, current_reg}}, {{NONE, a}, {NONE, b}}};
				code[block].push_back(irn);
			}
			else if (is_Minus(irn))
			{
				set_irn_link(irn, irn);
				size_t a = new_register();
				registers[current_reg].writes.push_back(irn);
				registers[a].reads.push_back(irn);
				usage[irn] = {{{NONE, current_reg}}, {{NONE, a}}};
				code[block].push_back(irn);
			}
			else if (is_Div(irn) || is_Mod(irn))
			{
				set_irn_link(irn, irn);
				size_t a = new_register();
				size_t b = new_register();
				registers[current_reg].writes.push_back(irn);
				registers[a].reads.push_back(irn);
				registers[b].reads.push_back(irn);
				usage[irn] = {{{(is_Div(irn) ? RAX : RDX), current_reg}}, {{NONE, 0}, {RAX, a}, {NONE, b}}};
				code[block].push_back(irn);
			}
			else if (is_Load(irn))
			{
				if (!current_reg)
					printf("\tLoad with unused value\n");
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
				size_t val = new_register();
				registers[addr].reads.push_back(irn);
				registers[val].reads.push_back(irn);
				usage[irn] = {{}, {{NONE, 0}, {NONE, addr}, {NONE, val}}};
				code[block].push_back(irn);
			}
			else if (is_Address(irn))
			{
				//dummy, silence not implemented yet line
				;
			}
			else
				printf("\tnot implemented yet\n");

			// take care of parents
			for (int i = get_irn_arity(irn) - 1; i >= 0; i--)
				assemble(get_irn_n(irn, i));
		}
		else
		{
			printf("\twaiting for:\n");

			for (auto& child : children)
				ir_printf("\t\t(%lu) %F %p\n", get_irn_node_nr(child.first), child.first, child.first);
		}
	}

	void CodeGen::output(ir_graph* irg)
	{
		//TODO: %n$ syntax not in standart C(++)
		fprintf(out, "\t.p2align 4,,15\n\t.globl  %1$s\n\t.type   %1$s, @function\n%1$s:\n", get_entity_name(get_irg_entity(irg)));
		// adjust stack
		fprintf(out, "\tsub $%zd, %%rsp\n", 8 * registers.size());

		for (auto& block : code)
		{
			fprintf(out, ".L_%s_%zu:\n", get_entity_name(get_irg_entity(irg)), get_irn_node_nr(block.first));

			for (auto it = block.second.rbegin(); it != block.second.rend(); it++)
				output(*it);
		}

		fprintf(out, "\t.size   %1$s, .-%1$s\n\n", get_entity_name(get_irg_entity(irg)));
	}

	void CodeGen::output(ir_node* irn)
	{
		ir_fprintf(out, "\t# %F\n", irn);

		if (is_Return(irn))
		{
			if (get_irn_arity(irn) == 2)
				fprintf(out, "\tmov %zd(%%rsp), %%rax\n", 8 * usage[irn].second[1].reg - 8);

			// free stack
			fprintf(out, "\tadd $%zd, %%rsp\n", 8 * registers.size());
			// and return
			fprintf(out, "\tret\n");
		}
		else if (is_Start(irn))
		{
			auto& writes = usage[irn].first;

			for (auto& w : writes)
			{
				if (w.reg)
					fprintf(out, "\tmov %s, %zd(%%rsp)\n", constraintToRegister(w.constraint), 8 * w.reg - 8);
			}
		}
		else if (is_Call(irn))
		{
			auto& reads = usage[irn].second;

			for (auto& r : reads)
			{
				if (r.reg)
					fprintf(out, "\tmov %zd(%%rsp), %s\n", 8 * r.reg - 8, constraintToRegister(r.constraint));
			}

			fprintf(out, "\tcall %s\n", get_entity_name(get_Call_callee(irn)));

			if (!usage[irn].first.empty())
				fprintf(out, "\tmov %%rax, %zd(%%rsp)\n", 8 * usage[irn].first[0].reg - 8);
		}
		else if (is_Const(irn))
			fprintf(out, "\tmov%c $%ld, %zd(%%rsp)\n", get_irn_mode(irn) == mode_Is ? 'l' : 'q', get_tarval_long(get_Const_tarval(irn)), 8 * usage[irn].first[0].reg - 8);
		else if (is_Add(irn) || is_Sub(irn) || is_Mul(irn))
		{
			char const* op = is_Add(irn) ? "add" : is_Sub(irn) ? "sub" : "imul";
			//sub a, b seems to store b - a in b
			fprintf(out, "\tmov %zd(%%rsp), %%rax\n", 8 * usage[irn].second[0].reg - 8);
			fprintf(out, "\t%s %zd(%%rsp), %%rax\n", op, 8 * usage[irn].second[1].reg - 8);
			fprintf(out, "\tmov %%rax, %zd(%%rsp)\n", 8 * usage[irn].first[0].reg - 8);
		}
		else if (is_Minus(irn))
		{
			fprintf(out, "\tmov %zd(%%rsp), %%rax\n", 8 * usage[irn].second[0].reg - 8);
			fprintf(out, "\tneg %%rax\n");
			fprintf(out, "\tmov %%rax, %zd(%%rsp)\n", 8 * usage[irn].first[0].reg - 8);
		}
		else if (is_Div(irn) || is_Mod(irn))
		{
			fprintf(out, "\tmov %zd(%%rsp), %%rax\n\txor %%rdx, %%rdx\n", 8 * usage[irn].second[1].reg - 8);
			fprintf(out, "\tmov %zd(%%rsp), %%rbx\n", 8 * usage[irn].second[2].reg - 8);
			fprintf(out, "\tidivl %%ebx\n");
			fprintf(out, "\tmov %%r%cx, %zd(%%rsp)\n", is_Div(irn) ? 'a' : 'd', 8 * usage[irn].first[0].reg - 8);
		}
		else if (is_Load(irn))
		{
			fprintf(out, "\tmov %zd(%%rsp), %%rax\n", 8 * usage[irn].second[1].reg - 8);
			fprintf(out, "\tmov (%%rax), %%rax\n");
			fprintf(out, "\tmov %%rax, %zd(%%rsp)\n", 8 * usage[irn].first[0].reg - 8);
		}
		else if (is_Store(irn))
		{
			fprintf(out, "\tmov %zd(%%rsp), %%rax\n", 8 * usage[irn].second[1].reg - 8);
			fprintf(out, "\tmov %zd(%%rsp), %%rbx\n", 8 * usage[irn].second[2].reg - 8);
			fprintf(out, "\tmov %%rbx, (%%rax)\n");
		}
	}
}
