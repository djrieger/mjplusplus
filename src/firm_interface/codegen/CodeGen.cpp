#include <algorithm>
#include <string>
#include <set>
#include <queue>
#include <cassert>

#include "CodeGen.hpp"

#include "../FirmInterface.hpp"

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
	 *   return in rax;
	 *   call first args in rdi, rsi, rdx, rcx, r8, r9, remainder on stack, result in rax;
	 *   div/mod in operands in rax:rdx, any; result in rax (div), rdx (mod)
	 *
	 * other conventions:
	 *   callee saved registers: rbx, rbp, r12, r13, r14, r15
	 *   caller saved registers: r10, r11 (, rax, unused argument registers)
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
		if (a == b)
			return;

		auto& a_reg = registers[a];
		auto& b_reg = registers[b];

		for (auto& w : b_reg.writes)
		{
			for (auto& ww : usage[w].first)
			{
				if (ww.reg == b)
					ww.reg = a;
			}
		}

		a_reg.writes.insert(a_reg.writes.end(), b_reg.writes.begin(), b_reg.writes.end());

		for (auto& r : b_reg.reads)
		{
			for (auto& rr : usage[r].second)
			{

				if (rr.reg == b)
					rr.reg = a;
			}
		}

		a_reg.reads.insert(a_reg.reads.end(), b_reg.reads.begin(), b_reg.reads.end());

		if (add_to_free)
			free_registers.insert(b);
	}

	void CodeGen::swap_register(size_t a, size_t b)
	{
		if (a == b)
			return;

		auto& a_reg = registers[a];
		auto& b_reg = registers[b];

		std::vector<ir_node*> writes(a_reg.writes.size() + b_reg.writes.size());
		writes.resize(std::distance(writes.begin(), std::set_union(a_reg.writes.begin(), a_reg.writes.end(), b_reg.writes.begin(), b_reg.writes.end(), writes.begin())));

		for (auto& w : writes)
		{
			for (auto& ww : usage[w].first)
			{
				if (ww.reg == a)
					ww.reg = b;
				else if (ww.reg == b)
					ww.reg = a;
			}
		}

		std::vector<ir_node*> reads(a_reg.reads.size() + b_reg.reads.size());
		reads.resize(std::distance(reads.begin(), std::set_union(a_reg.reads.begin(), a_reg.reads.end(), b_reg.reads.begin(), b_reg.reads.end(), reads.begin())));

		for (auto& r : reads)
		{
			for (auto& rr : usage[r].second)
			{
				if (rr.reg == a)
					rr.reg = b;
				else if (rr.reg == b)
					rr.reg = a;
			}
		}

		a_reg.writes.swap(b_reg.writes);
		a_reg.reads.swap(b_reg.reads);
	}

	char const* CodeGen::constraintToRegister(Constraint c, ir_mode* mode)
	{
		if (c >= STACK)
			return "!!Stack!!";

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

	char const* CodeGen::constraintToRegister(size_t c, ir_mode* mode)
	{
		if (c <= 0 || c > 16 )
		{
			printf("invalid register %zu used as constraint\n", c);
			abort();
		}

		return constraintToRegister((Constraint) c, mode);
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

		// fake register for non-data parents and actual cpu registers
		registers.insert(registers.end(), 17, {{}, {}});

		edges_activate(irg);

		//handle keepalives
		ir_node* en = get_irg_end(irg);
		set_irn_link(en, NULL);

		for (int i = 0; i < get_irn_arity(en); i++)
		{
			ir_node* enp = get_irn_n(en, i);
			workstack.push(enp);
		}

		// no code for end block, just find returns
		ir_node* eb = get_irg_end_block(irg);
		set_irn_link(eb, NULL);

		for (int i = 0; i < get_irn_arity(eb); i++)
			workstack.push(get_irn_n(eb, i));

		while (!workstack.empty())
		{
			ir_node* irn = workstack.top();
			workstack.pop();
			assemble(irn);
		}

		auto regdump = [&]
		{
			printf("----------\n");
			size_t i = 0;

			for (auto& reg : registers)
			{
				if (i <= 16)
					printf("%s writes: ", constraintToRegister((Constraint)i, mode_P));
				else
					printf("%zu writes: ", i - 16);

				for (ir_node* w : reg.writes)
					ir_printf("(%ld) %F, ", get_irn_node_nr(w), w);

				printf("\nreads: ");

				for (ir_node* r : reg.reads)
					ir_printf("(%ld) %F, ", get_irn_node_nr(r), r);

				printf("\n\n");
				++i;
			}

			for (auto& u : usage)
			{
				ir_printf("(%ld) %F:\n\twrites: ", get_irn_node_nr(u.first), u.first);

				for (auto& w : u.second.first)
					printf("[%s, %zu], ", constraintToRegister(w.constraint, mode_P), w.reg);

				printf("\n\treads: ");

				for (auto& r : u.second.second)
					printf("[%s, %zu], ", constraintToRegister(r.constraint, mode_P), r.reg);

				printf("\n");
			}

			printf("\n\n");
		};

		// register condensing
		while (!free_registers.empty())
		{
			if (*free_registers.rbegin() == registers.size() - 1)
				free_registers.erase(--free_registers.end());
			else
			{
				size_t reg = *free_registers.begin();
				free_registers.erase(free_registers.begin());
				registers[reg].writes.clear();
				registers[reg].reads.clear();
				merge_register(reg, registers.size() - 1, false);
			}

			registers.resize(registers.size() - 1);
		}

		size_t args = get_method_n_params(get_entity_type(get_irg_entity(irg)));

		regdump();

		for (auto& block : code)
			sort_phis(block.second.phi, block.first);

		auto live_intervals = compute_live_intervals(irg);
		allocate(irg, live_intervals);

		// register condensing
		while (!free_registers.empty())
		{
			if (*free_registers.rbegin() == registers.size() - 1)
				free_registers.erase(--free_registers.end());
			else
			{
				size_t reg = *free_registers.begin();
				free_registers.erase(free_registers.begin());
				registers[reg].writes.clear();
				registers[reg].reads.clear();
				merge_register(reg, registers.size() - 1, false);
			}

			registers.resize(registers.size() - 1);
		}

		regdump();

		if (args > 6)
		{
			printf("more than 6 args currently broken\n");
			abort();
			/*
			// if arguments are passed on the stack:
			// - filter out stack arguments and move them to the end of the register vector
			// - seperate them from the remaining registers with an additional dummy register
			//     (actually the gap is used for the return address pushed by call)
			// if some stack arguments are unused we'll use their registers for other variables
			size_t last = new_register();

			while (last <= args - 6 + 16)
				// ensure register allocation for stack arguments
				last = new_register();

			size_t gap = last - (args - 6 + 16);
			swap_register(gap, last);
			auto& writes = usage[get_irg_start(irg)].first;

			for (auto& w : writes)
			{
				if (w.constraint >= STACK && w.reg > 16)
					swap_register(gap + w.constraint - STACK + 1, w.reg);
			}*/
		}

		regdump();

		printf("\n=======================\n\n");

		output(irg);
		edges_deactivate(irg);
	}

	std::vector<std::pair<size_t, size_t>> CodeGen::compute_live_intervals(ir_graph* irg)
	{
		std::vector<std::pair<size_t, size_t>> live_intervals(registers.size());
		size_t pos = 1;

		auto isInLoopHeader = [&](ir_node * node)
		{
			auto block = get_nodes_block(node);

			if (get_irn_arity(block) > 1)
			{
				auto endNode = get_irg_end(irg);

				for (int i = 0; i < get_irn_arity(endNode); ++i)
				{
					if (get_irn_n(endNode, i) == block)
						return true;
				}

				return false;
			}
			else
				return false;
		};

		auto getPred = [&](ir_node * block)
		{
			//auto block = get_nodes_block(node);
			ir_node* ret = NULL;

			if (get_irn_arity(block) > 1)
				return get_irn_n(block, 1);

			return ret;
		};

		std::function<ir_node*(ir_node*)> traverseUntilLoopHead = [&](ir_node * node) -> ir_node*
		{
			if (isInLoopHeader(node))
				return get_nodes_block(node);
			else if (get_irg_start_block(irg) == get_nodes_block(node))
				return NULL;
			else {
				ir_node* block = get_nodes_block(node);

				for (int i = 0; i < get_irn_arity(block); ++i)
				{
					ir_node* res = traverseUntilLoopHead(get_irn_n(block, i));

					if (res) return res;
				}

				return NULL;
			}
		};

		auto handlePreLoopOperands = [&](ir_node * node, ir_node * block)
		{
			std::vector<std::pair<ir_node*, ir_node*>> ret;

			for (int i = 0; i < get_irn_arity(node); ++i)
			{
				ir_node* child = get_irn_n(node, i);

				if (is_Const(child))
					continue;

				while (is_Conv(child)) child = get_irn_n(child, 0);

				ir_node* childBlock = get_nodes_block(child);

				if (block != childBlock)
				{
					//ir_printf("%F (%d) has operand out of loop ",node,get_irn_node_nr(node));
					if (get_irn_node_nr(childBlock) < get_irn_node_nr(block))
					{
						//ir_printf(" and it's before the loop; child: %F (%d)",child,get_irn_node_nr(child));
						ret.push_back({child, traverseUntilLoopHead(child)});
					}

					//printf("\n");
				}
			}

			return ret;
		};

		compute_doms(irg);
		compute_postdoms(irg);

		std::vector<ir_node*> ordered_blocks;

		for (auto& block : code)
			ordered_blocks.push_back(block.first);

		std::function<bool(ir_node*, ir_node*)> followControlFlowUntil = [&](ir_node * srcBlock, ir_node * targetBlock) -> bool
		{
			assert(is_Block(srcBlock));
			assert(is_Block(targetBlock));
			bool targetBlockFound = false;
			size_t i = 0;
			auto& controlFlowNodes = code[srcBlock].control;
			ir_printf("%+F has %zu control flow nodes\n", srcBlock, controlFlowNodes.size());
			// if we have only one control flow node, it must be either a jmp or a return node.
			if (controlFlowNodes.size() == 1)
			{
				assert(is_Jmp(controlFlowNodes[0]) || is_Return(controlFlowNodes[0]));
				ir_node* block = FirmInterface::getInstance().getOuts(controlFlowNodes[i])[0].first;
				block = (!is_Block(block)) ? get_nodes_block(block) : block;
				targetBlockFound = followControlFlowUntil(block, targetBlock);
			}
			else if (controlFlowNodes.size() > 1)
			{
				// there might be cmp and cond nodes, only care about successors of conds
				while (!targetBlockFound && i < controlFlowNodes.size())
				{
					if (is_Cond(controlFlowNodes[i]))
					{
						auto projs = FirmInterface::getInstance().getOuts(controlFlowNodes[i]);

						//ir_printf("%+F with succ:\t",controlFlowNodes[i]);
						for (auto& proj : projs)
						{
							auto succ = FirmInterface::getInstance().getOuts(proj.first)[0].first;
							// make sure that this is really a block
							succ = (!is_Block(succ)) ? get_nodes_block(succ) : succ;

							//ir_printf("%+F, ",succ.first);
							// if the successor is the end block, we look at the other successors
							if (succ == get_irg_end_block(irg)) continue;

							// call recursively on the successor node.
							if (followControlFlowUntil(succ, targetBlock))
							{
								targetBlockFound = true;
								break;
							}
						}

						//ir_printf("\n");
					}

					++i;
				}
			}

			return targetBlockFound;
		};

		std::sort(ordered_blocks.begin(), ordered_blocks.end(), [&](ir_node const * a, ir_node const * b)
		{
			ir_printf("cmp %+F %+F: %d %d, %d %d\n", a, b, block_dominates(a, b), block_postdominates(b, a), block_dominates(b, a), block_postdominates(a, b));

			if (block_dominates(a, b) || (block_postdominates(b, a) && !block_dominates(b, a)))
				return true;
			else if (block_dominates(b, a) || block_postdominates(a, b))
				return false;
			else
			{
				ir_node* head = ir_deepest_common_dominator((ir_node*) a, (ir_node*) b);
				ir_node* rep_0 = (ir_node*) get_irn_link(code[head].control[0]);
				ir_node* rep_1 = (ir_node*) get_irn_link(code[head].control[1]);

				if (block_dominates(rep_0, a) && block_dominates(rep_1, b))
				{
					a = rep_0;
					b = rep_1;
				}
				else if (block_dominates(rep_1, a) && block_dominates(rep_0, b))
				{
					a = rep_1;
					b = rep_0;
				}

				// the following two for-loops are there to check if one rep precedes the other.
				// then we know that one block belongs to the loop condition.
				for (int i = 0; i < get_irn_arity(a); ++i)
				{
					if (get_irn_n(a, i) == b) return false;
				}

				for (int i = 0; i < get_irn_arity(b); ++i)
				{
					if (get_irn_n(b, i) == a) return true;
				}

				// another criterion to identify loop bodies is, whether the block's control flow ends
				// in a jump targeting the deepest common dominator (i.e. the loop head in this case)
				if (isInLoopHeader(code[head].control[0]))
				{
					//return followControlFlowUntil((ir_node*)a,head);
				}

				return get_irn_node_nr(a) < get_irn_node_nr(b);
			}
		});

		for (auto& b : ordered_blocks)
			ir_printf("%+F\n", b);

		printf("program order:\n");

		std::vector<std::pair<ir_node*, ir_node*>> postprocessing;
		std::set<ir_node*> endNodes;
		std::unordered_map<ir_node*, size_t> loopEndPos;
		// we have to add a special case for this pointer and parameters
		size_t lastPos = 0;

		for (auto& blockNode : ordered_blocks)
		{
			auto& block = code[blockNode];

			//ir_printf("%zu: %F (%d)\n",pos,blockNode,get_irn_node_nr(blockNode));
			auto handle_list = [&](std::vector<ir_node*> const & list, size_t& pos)
			{
				for (auto it = list.rbegin(); it != list.rend(); it++, pos++)
				{
					lastPos = pos;

					auto cand = handlePreLoopOperands(*it, blockNode);

					if (cand.size() > 0)
					{
						for (auto& x : cand)
						{
							ir_node* loopHeadNode = traverseUntilLoopHead(block.control[0]);
							ir_printf("x.s: %+F  lHN: %+F\n", x.second, loopHeadNode);

							if (loopHeadNode != x.second)
							{
								ir_node* lastLoopHead = loopHeadNode;

								while (loopHeadNode != x.second)
								{
									lastLoopHead = loopHeadNode;
									loopHeadNode = traverseUntilLoopHead(get_irn_n(loopHeadNode, 0));
									ir_printf("-- x.s: %+F  lHN: %+F  last: %+F\n", x.second, loopHeadNode, lastLoopHead);
								}

								auto endNode = getPred(lastLoopHead);
								//ir_printf("%+F\t%F (%d)\t%F (%d)\n",x.first, loopHeadNode,get_irn_node_nr(loopHeadNode),endNode,get_irn_node_nr(endNode));
								postprocessing.push_back({x.first, endNode});
								endNodes.insert(endNode);
							}
						}
					}

					if (isInLoopHeader(*it))
					{

						auto endNode = getPred(get_nodes_block(*it));
						//ir_printf("%F (%d)\t%F (%d)\n", *it, get_irn_node_nr(*it), endNode, get_irn_node_nr(endNode));
						postprocessing.push_back({*it, endNode});
						endNodes.insert(endNode);
					}

					if (endNodes.find(*it) != endNodes.end())
						loopEndPos[*it] = pos;

					for (auto& w : usage[*it].first)
					{
						if (!live_intervals[w.reg].first)
							live_intervals[w.reg].first = pos;
					}

					for (auto& r : usage[*it].second)
						live_intervals[r.reg].second = pos;

					ir_printf("%zu: %F (%d); %s\n", pos, *it, get_irn_node_nr(*it), isInLoopHeader(*it) ? "is in loop header" : "");
				}
			};
			handle_list(block.normal, pos);
			handle_list(block.phi, pos);
			handle_list(block.control, pos);
		}

		for (auto& head : postprocessing)
		{
			for (auto& r : usage[head.first].first)
				live_intervals[r.reg].second = std::max(live_intervals[r.reg].second, loopEndPos[head.second]);

			for (auto& r : usage[head.first].second)
				live_intervals[r.reg].second = std::max(live_intervals[r.reg].second, loopEndPos[head.second]);
		}

		ir_node* start = get_irg_start(irg);
		ir_printf("last pos is %zu; %F (%d)'s live intervals get altered:\t", lastPos, start, get_irn_node_nr(start));

		for (auto& w : usage[start].first)
		{
			ir_printf("[%zu(%zu,%zu to %zu)]\t", w.reg, live_intervals[w.reg].first, live_intervals[w.reg].second, lastPos);
			live_intervals[w.reg].second = lastPos;
		}

		printf("\n");

		printf("live intervals of virtual registers:\n");
		int i = 0;

		for (auto& live : live_intervals)
			if (i > 16)printf("%d: (%zu, %zu) \tis ok? %s\n", i++, live.first, live.second, (live.second < live.first) ? "no" : "yes");

		return live_intervals;
	}

	void CodeGen::allocate(ir_graph* irg, std::vector<std::pair<size_t, size_t>> const& live_intervals)
	{
		std::set<std::pair<size_t, size_t>> active;
		std::vector<std::pair<std::pair<size_t, size_t>, std::pair<size_t, Constraint>>> sorted_live;

		for (size_t i = 17; i < live_intervals.size(); i++)
		{
			sorted_live.push_back({live_intervals[i], {i, NONE}});
		}

		std::sort(sorted_live.begin(), sorted_live.end());

		std::set<Constraint> free_regs = {/*RAX, RCX, RDX,*/ RBX, /*RSP,*/ RBP, /*RSI, RDI, R8, R9,*/ R10, R11, R12, R13, R14, R15};
		size_t num_regs = free_regs.size();

		auto expire_old_intervals = [&](size_t i)
		{
			for (auto it = active.begin(); it != active.end(); it++)
			{
				if (it->first > sorted_live[i].first.first)
					return;

				free_regs.insert(sorted_live[it->second].second.second);
				active.erase(it);
			}
		};

		auto spill_at_interval = [&](size_t i)
		{
			auto spill_it = --active.end();

			// paper states ">=" instead of ">"
			// probably because of different interpretation of live intervals
			if (spill_it->first > sorted_live[i].first.second)
			{
				sorted_live[i].second.second = sorted_live[spill_it->second].second.second;
				sorted_live[spill_it->second].second.second = NONE;
				active.erase(spill_it);
				active.insert({sorted_live[i].first.second, i});
			}
			else
				sorted_live[i].second.second = NONE;
		};

		for (size_t i = 0; i < sorted_live.size(); i++)
		{
			expire_old_intervals(i);

			if (active.size() == num_regs)
				spill_at_interval(i);
			else
			{
				Constraint reg = *free_regs.begin();
				free_regs.erase(free_regs.begin());
				sorted_live[i].second.second = reg;
				active.insert({sorted_live[i].first.second, i});
			}

		}

		for (auto& reg : sorted_live)
		{
			if (reg.second.second != NONE)
				merge_register(reg.second.second, reg.second.first);
		}
	}

	void CodeGen::assemble(ir_node* irn)
	{

		ir_node* block = irn;

		if (!is_Block(irn))
			block = get_nodes_block(irn);

		if (get_irn_link(block) != block)
		{
			// handle the block
			// requeue the node if it isn't the block itself
			if (irn != block)
				workstack.push(irn);

			set_irn_link(block, block);

			for (ir_node* phi = get_Block_phis(block); phi && is_Phi(phi); phi = get_Phi_next(phi))
			{
				auto it = partial.find(phi);

				if (it != partial.end())
					it->second++;
				else
					partial[phi] = 1 + FirmInterface::getInstance().getOuts(phi).size();

				workstack.push(phi);
			}

			// take care of block parents
			for (int i = 0; i < get_irn_arity(block); i++)
				workstack.push(get_irn_n(block, i));

			return;
		}

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

		if (partial[irn] == 0 || (is_Phi(irn) && get_irn_link(irn) != irn))
		{
			// take care of parents
			if (!is_Phi(irn) || (is_Phi(irn) && get_irn_link(irn) != irn))
			{
				for (int i = 0; i < get_irn_arity(irn); i++)
				{
					ir_node* p = get_irn_n(irn, i);
					workstack.push(p);
				}
			}

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
				for (auto& child : children)
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
					// for each parent: add mov/load new_reg -> phi_reg in parent block
					for (int pred = 0; pred < get_irn_arity(irn); pred++)
					{
						code[get_nodes_block(get_irn_n(block, pred))].phi.push_back(irn);
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

				code[block].control.push_back(irn);
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

				code[block].control.push_back(irn);
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

				code[block].control.push_back(irn);
			}
			else if (is_Jmp(irn))
			{
				set_irn_link(irn, children[0].first);
				code[block].control.push_back(irn);
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
					code[block].normal.push_back(irn);
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

				for (auto& child : children)
				{
					if (!is_Proj(child.first) || get_irn_mode(child.first) != mode_T || get_Proj_num(child.first) != pn_Start_T_args)
						continue;

					auto used_args = FirmInterface::getInstance().getOuts(child.first);

					for (auto& used_arg : used_args)
					{
						if (!is_Proj(used_arg.first))
							continue;

						// function arguments
						unsigned int arg = get_Proj_num(used_arg.first);
						Constraint c = arg < 6 ? arg_order[arg] : (Constraint) (STACK + (-6 + arg));
						size_t reg = usage[used_arg.first].second[0].reg;
						registers[reg].writes.push_back(irn);
						writes.push_back({c, reg});
					}
				}

				usage[irn] = {writes, {}};
				code[block].normal.push_back(irn);
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
					size_t reg = is_Const(get_irn_n(irn, i)) ? 0 : new_register();
					reads.push_back({i < 8 ? arg_order[i - 2] : (Constraint) (STACK - 8 + i), reg});

					if (reg)
						registers[reg].reads.push_back(irn);
				}

				set_irn_link(irn, irn);
				usage[irn] = {writes, reads};
				code[block].normal.push_back(irn);
			}
			else if (is_Add(irn) || is_Sub(irn) || is_Mul(irn) || is_Shr(irn) || is_Shrs(irn) || is_Shl(irn))
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
				code[block].normal.push_back(irn);
			}
			else if (is_Minus(irn))
			{
				set_irn_link(irn, irn);
				size_t a = is_Const(get_irn_n(irn, 0)) ? 0 : new_register();
				registers[current_reg].writes.push_back(irn);

				if (a)
					registers[a].reads.push_back(irn);

				usage[irn] = {{{NONE, current_reg}}, {{NONE, a}}};
				code[block].normal.push_back(irn);
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
				code[block].normal.push_back(irn);
			}
			else if (is_Lea(irn))
			{
				set_irn_link(irn, irn);
				registers[current_reg].writes.push_back(irn);
				std::vector<Access> reads;

				for (int i = 0; i < get_irn_arity(irn); i++)
				{
					size_t r = is_Const(get_irn_n(irn, i)) ? 0 : new_register();
					reads.push_back({NONE, r});

					if (r)
						registers[r].reads.push_back(irn);
				}

				usage[irn] = {{{NONE, current_reg}}, reads};
				code[block].normal.push_back(irn);
			}
			else if (is_Load(irn))
			{
				if (!current_reg)
					printf("\tLoad with unused value?!\n");
				else
				{
					set_irn_link(irn, irn);
					size_t addr = is_Const(get_irn_n(irn, 1)) ? 0 : new_register();

					if (addr)
						registers[addr].reads.push_back(irn);

					registers[current_reg].writes.push_back(irn);
					usage[irn] = {{{NONE, current_reg}}, {{NONE, 0}, {NONE, addr}}};
					code[block].normal.push_back(irn);
				}
			}
			else if (is_Store(irn))
			{
				set_irn_link(irn, irn);
				size_t addr = is_Const(get_irn_n(irn, 1)) ? 0 : new_register();
				size_t val = is_Const(get_irn_n(irn, 2)) ? 0 : new_register();

				if (addr)
					registers[addr].reads.push_back(irn);

				if (val)
					registers[val].reads.push_back(irn);

				usage[irn] = {{}, {{NONE, 0}, {NONE, addr}, {NONE, val}}};
				code[block].normal.push_back(irn);
			}
			else if (is_Address(irn) || is_Const(irn))
			{
				if (current_reg)
				{
					ir_printf("\n!!!!!!!!!!!!!\n(%ld) %F\n\tthis shouldn't use a register...\n", get_irn_node_nr(irn), irn);
					abort();
				}
			}
			else
			{
				ir_printf("\n(%ld) %F\n\tnot implemented yet\n", get_irn_node_nr(irn), irn);
				abort();
			}
		}
	}

	void CodeGen::load_or_imm(ir_node* node, size_t reg)
	{
		if (!reg && is_Const(node))
			fprintf(out, "$%ld", get_tarval_long(get_Const_tarval(node)));
		else if (reg <= 16)
			fprintf(out, "%s", constraintToRegister(reg, get_irn_mode(node)));
		else
			fprintf(out, "%zd(%%rsp)", 8 * reg - 8 * 17);
	}

	void CodeGen::load_or_reg(ir_mode* mode, size_t reg)
	{
		if (reg <= 16)
			fprintf(out, "%s", constraintToRegister(reg, mode));
		else
			fprintf(out, "%zd(%%rsp)", 8 * reg - 8 * 17);
	}

	void CodeGen::gen_mov(ir_mode* mode, ir_node* node, size_t src, size_t dst)
	{
		char const* os = operationSuffix(mode);
		fprintf(out, "\tmov%s ", os);

		if (node)
			load_or_imm(node, src);
		else
			load_or_reg(mode, src);

		if (src > 16 && dst > 16)
		{
			char const* rax = constraintToRegister(RAX, mode);
			fprintf(out, ", %s\n\tmov%s %s", rax, os, rax);
		}

		fprintf(out, ", ");
		load_or_reg(mode, dst);
		fprintf(out, "\n");
	}

	void CodeGen::gen_binary_op(ir_node* irn, ir_mode* mode, char const* op, bool commutative)
	{
		char const* os = operationSuffix(mode);
		auto& u = usage[irn];
		char const* rax = constraintToRegister(RAX, mode);

		if (u.second[0].reg == u.first[0].reg)
		{
			if (u.second[0].reg > 16 && u.second[1].reg > 16)
			{
				//c4
				fprintf(out, "\tmov%s ", os);
				load_or_imm(get_irn_n(irn, 1), u.second[1].reg);
				fprintf(out, ", %s\n\t%s%s %s, ", rax, op, os, rax);
				load_or_imm(get_irn_n(irn, 0), u.second[0].reg);
				fprintf(out, "\n");
			}
			else
			{
				//c1
				fprintf(out, "\t%s%s ", op, os);
				load_or_imm(get_irn_n(irn, 1), u.second[1].reg);
				fprintf(out, ", ");
				load_or_imm(get_irn_n(irn, 0), u.second[0].reg);
				fprintf(out, "\n");
			}
		}
		else if (u.second[1].reg == u.first[0].reg)
		{
			if (/*u.second[1].reg &&*/ u.second[1].reg <= 16 && u.second[0].reg <= 16) //never a constant
			{
				//c5
				if (commutative)
				{
					fprintf(out, "\t%s%s ", op, os);
					load_or_imm(get_irn_n(irn, 0), u.second[0].reg);
					fprintf(out, ", ");
					load_or_imm(get_irn_n(irn, 1), u.second[1].reg);
					fprintf(out, "\n");
				}
				else
				{
					fprintf(out, "\tmov%s ", os);
					load_or_imm(get_irn_n(irn, 1), u.second[1].reg);
					fprintf(out, ", %s\n\tmov%s ", rax, os);
					load_or_imm(get_irn_n(irn, 0), u.second[0].reg);
					fprintf(out, ", ");
					load_or_imm(get_irn_n(irn, 1), u.second[1].reg);
					fprintf(out, "\n\t%s%s %s, ", op, os, rax);
					load_or_imm(get_irn_n(irn, 1), u.second[1].reg);
					fprintf(out, "\n");
				}
			}
			else if (u.second[0].reg > 16 || (/*u.second[1].reg &&*/ u.second[1].reg <= 16)) //never a constant
			{
				//c6
				if (commutative)
				{
					fprintf(out, "\t%s%s ", op, os);
					load_or_imm(get_irn_n(irn, 0), u.second[0].reg);
					fprintf(out, ", ");
					load_or_imm(get_irn_n(irn, 1), u.second[1].reg);
					fprintf(out, "\n");
				}
				else
				{
					fprintf(out, "\tmov%s ", os);
					load_or_imm(get_irn_n(irn, 0), u.second[0].reg);
					fprintf(out, ", %s\n\t%s%s ", rax, op, os);
					load_or_imm(get_irn_n(irn, 1), u.second[1].reg);
					fprintf(out, ", %s\n\tmov%s %s, ", rax, os, rax);
					load_or_imm(get_irn_n(irn, 1), u.second[1].reg);
					fprintf(out, "\n");
				}
			}
			else
			{
				//c7
				if (commutative)
				{
					fprintf(out, "\tmov%s ", os);
					load_or_imm(get_irn_n(irn, 0), u.second[0].reg);
					fprintf(out, ", %s\n\t%s%s %s, ", rax, op, os, rax);
					load_or_imm(get_irn_n(irn, 1), u.second[1].reg);
					fprintf(out, "\n");
				}
				else
				{
					fprintf(out, "\tmov%s ", os);
					load_or_imm(get_irn_n(irn, 0), u.second[0].reg);
					fprintf(out, ", %s\n\t%s%s ", rax, op, os);
					load_or_imm(get_irn_n(irn, 1), u.second[1].reg);
					fprintf(out, ", %s\n\tmov%s %s, ", rax, os, rax);
					load_or_imm(get_irn_n(irn, 1), u.second[1].reg);
					fprintf(out, "\n");
				}
			}
		}
		else
		{
			if (/*u.first[0].reg &&*/ u.first[0].reg <= 16) //never a constant
			{
				//c2
				fprintf(out, "\tmov%s ", os);
				load_or_imm(get_irn_n(irn, 0), u.second[0].reg);
				fprintf(out, ", ");
				load_or_imm(irn, u.first[0].reg);
				fprintf(out, "\n\t%s%s ", op, os);
				load_or_imm(get_irn_n(irn, 1), u.second[1].reg);
				fprintf(out, ", ");
				load_or_imm(irn, u.first[0].reg);
				fprintf(out, "\n");
			}
			else if (u.second[0].reg == u.second[1].reg && u.second[0].reg > 16)
			{
				//c8
				fprintf(out, "\tmov%s ", os);
				load_or_imm(get_irn_n(irn, 0), u.second[0].reg);
				fprintf(out, ", %s\n\t%s%s %s, %s\n\tmov%s %s, ", rax, op, os, rax, rax, os, rax);
				load_or_imm(get_irn_n(irn, 1), u.second[1].reg);
				fprintf(out, "\n");
			}
			else
			{
				//c3
				fprintf(out, "\tmov%s ", os);
				load_or_imm(get_irn_n(irn, 0), u.second[0].reg);
				fprintf(out, ", %s\n\t%s%s ", rax, op, os);
				load_or_imm(get_irn_n(irn, 1), u.second[1].reg);
				fprintf(out, ", %s\n\tmov%s %s, ", rax, os, rax);
				load_or_imm(irn, u.first[0].reg);
				fprintf(out, "\n");
			}
		}
	}

	void CodeGen::output(ir_graph* irg)
	{
		char const* name = get_entity_name(get_irg_entity(irg));
#ifndef __APPLE__
		fprintf(out, "\t.p2align 4,,15\n\t.globl %s\n\t.type %s, @function\n%s:\n", name, name, name);
#else
		fprintf(out, "\t.p2align 4,,15\n\t.globl %s%s\n_%s:\n", std::string(name) == "main" ? "_" : "", name, name);
#endif

		// adjust stack
		size_t reg_count = registers.size() - 17;

		if (get_method_n_params(get_entity_type(get_irg_entity(irg))) > 6)
			reg_count -= get_method_n_params(get_entity_type(get_irg_entity(irg))) - 6 + 1;

		if (reg_count)
			fprintf(out, "\tsub $%zd, %%rsp\n", 8 * reg_count);

		// order blocks to remove pointless jumps
		std::vector<std::pair<ir_node* const, Codelist>> ordered_blocks;
		// start block must be first
		auto current = code.find(get_irg_start_block(irg));
		std::stack<ir_node*> workstack;

		while (true)
		{
			ordered_blocks.push_back(*current);
			code.erase(current);

			for (auto it = ordered_blocks.back().second.control.begin() + 1; it != ordered_blocks.back().second.control.end(); it++)
				workstack.push((ir_node*) get_irn_link(*it));

			auto next = code.find((ir_node*) get_irn_link(ordered_blocks.back().second.control[0]));

			if (next != code.end())
			{
				ordered_blocks.back().second.control.erase(ordered_blocks.back().second.control.begin());
				current = next;
				continue;
			}

			do
			{
				if (workstack.empty())
				{
					goto done;// == break 2; if c++ would allow that...
				}

				current = code.find(workstack.top());
				workstack.pop();
			}
			while (current == code.end());
		}

	done:

		if (!code.empty())
		{
			printf("graph with unreachable blocks?!\n");
			abort();
		}

		for (auto& block : ordered_blocks)
		{
			fprintf(out, ".L_%s_%zu:\n", get_entity_name(get_irg_entity(irg)), get_irn_node_nr(block.first));

			for (auto it = block.second.normal.rbegin(); it != block.second.normal.rend(); it++)
				output_normal(*it);

			if (!block.second.control.empty())
			{
				for (auto it = block.second.control.rbegin(); it != block.second.control.rend(); it++)
					output_control(*it, block.second);
			}
			else
				output_phis(block.second.phi, block.second);
		}

#ifndef __APPLE__
		fprintf(out, "\t.size %s, .-%s\n\n", name, name);
#endif
	}

	void CodeGen::sort_phis(std::vector<ir_node*>& phis, ir_node* block)
	{
		if (phis.empty())
			return;

		// figure out currently relevant predecessor
		ir_node* phi_block = get_nodes_block(phis[0]);
		int pred;

		for (pred = 0; pred < get_irn_arity(phis[0]); pred++)
		{
			if (get_nodes_block(get_irn_n(phi_block, pred)) == block)
				break;
		}

		code[block].phi_pred = pred;
		auto& circle_nodes = code[block].circle_nodes;

		// sort phis: phis belonging to the same loop or chain are grouped together
		//   and within these groups ordered for easy copying
		std::sort(phis.begin(), phis.end(), [&](ir_node const * a, ir_node const * b)
		{
			auto foo = [&](ir_node const * phi, ir_node const * other) -> std::tuple<int, long, ir_node const*, size_t>
			{
				bool has_circle = false;
				bool on_circle = false;
				bool found_other = false;

				ir_node const* entry = NULL;
				long head = get_irn_node_nr(phi);
				long min = head;
				size_t count_phi = 0;
				size_t count_other = 0;
				size_t count_head = 0;

				std::set<ir_node const*> visited;
				ir_node const* cur = phi;

				while (is_Phi(cur) && get_nodes_block(cur) == phi_block)
				{
					if (cur == other)
					{
						found_other = true;
						count_other = 0;
					}

					head = get_irn_node_nr(cur);

					if (head < min)
					{
						min = head;
						count_head = 0;
					}

					auto it = visited.find(cur);

					if (it != visited.end())
					{
						if (!has_circle)
						{
							has_circle = true;
							entry = cur;
							min = head;//reset min
							count_head = 0;
						}

						if (cur == phi)
						{
							on_circle = true;
							count_phi = 0;
							circle_nodes.insert(phi);
						}

						visited.erase(it);
					}
					else
					{
						if (has_circle)
							return std::make_tuple(!found_other ? 0 : !on_circle ? 1 : ((count_head < count_phi) == (count_head < count_other)) ? count_phi - count_other : count_other - count_phi, min, entry, visited.size());

						visited.insert(cur);
					}

					cur = get_irn_n(cur, pred);
					count_phi++;
					count_other++;
					count_head++;
				}

				return std::make_tuple(found_other ? 1 : 0, head, entry, 0U);
			};

			int ra;
			long head_a;
			ir_node const* entry_a;
			size_t dist_a;
			std::tie(ra, head_a, entry_a, dist_a) = foo(a, b);

			if (ra)
				return ra < 0;

			int rb;
			long head_b;
			ir_node const* entry_b;
			size_t dist_b;
			std::tie(rb, head_b, entry_b, dist_b) = foo(b, a);

			if (rb)
				return rb > 0;

			// cases reaching this line:
			// different chain/loop
			// different chains attached to same loop
			// -> arbitrary but consistent order
			if (head_a == head_b)
			{
				if (entry_a == entry_b)
				{
					if (dist_a == dist_b)
						return get_irn_node_nr(a) < get_irn_node_nr(b);

					return dist_a < dist_b;
				}

				return get_irn_node_nr(entry_a) < get_irn_node_nr(entry_b);
			}

			return head_a < head_b;
		});
	}

	void CodeGen::output_phis(std::vector<ir_node*> const& phis, Codelist const& block)
	{
		if (phis.empty())
			return;

		auto const& pred = block.phi_pred;
		auto const& circle_nodes = block.circle_nodes;

		for (auto it = phis.rbegin() ; it != phis.rend(); it++)
		{
			ir_node* value = get_irn_n(*it, pred);
			ir_mode* value_mode = get_irn_mode(value);

			ir_fprintf(out, "\t# (%ld) %F\n", get_irn_node_nr(*it), *it);

			if (circle_nodes.count(*it) && circle_nodes.count(value) && (it + 1 == phis.rend() || value != *(it + 1)))
			{
				// phi from the same block, but not the next in phis
				// -> value is head of a phi-loop, use saved value
				gen_mov(value_mode, value, RDX, usage[*it].first[0].reg);
			}
			else
			{
				for (auto& child : FirmInterface::getInstance().getOuts(*it))
				{
					if (circle_nodes.count(*it) && circle_nodes.count(child.first) && (it == phis.rbegin() || child.first != *(it - 1)))
					{
						// value is tail of a phi-loop, rescue current value
						gen_mov(value_mode, NULL, usage[*it].first[0].reg, RDX);
						break;
					}
				}

				gen_mov(value_mode, value, usage[*it].second[pred].reg, usage[*it].first[0].reg);
			}
		}
	}

	void CodeGen::output_control(ir_node* irn, Codelist const& block)
	{
		ir_fprintf(out, "\t# (%ld) %F\n", get_irn_node_nr(irn), irn);

		if (is_Return(irn))
		{
			if (get_irn_arity(irn) == 2)
			{
				ir_node* value = get_irn_n(irn, 1);
				ir_mode* value_mode = get_irn_mode(value);
				gen_mov(value_mode, value, usage[irn].second[1].reg, RAX);
			}
			else if (!strcmp("main", get_entity_name(get_irg_entity(get_irn_irg(irn)))))
				fprintf(out, "\txorq %%rax, %%rax\n");

			// free stack
			if (registers.size() > 1)
			{
				size_t reg_count = registers.size() - 17;

				if (get_method_n_params(get_entity_type(get_irg_entity(get_irn_irg(irn)))) > 6)
					reg_count -= get_method_n_params(get_entity_type(get_irg_entity(get_irn_irg(irn)))) - 6 + 1;

				if (reg_count)
					fprintf(out, "\tadd $%zd, %%rsp\n", 8 * reg_count);
			}

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

			if (is_Const(get_irn_n(irn, 0)))
			{
				ir_node* tmp = get_irn_n(irn, 1);
				set_irn_n(irn, 1, get_irn_n(irn, 0));
				set_irn_n(irn, 0, tmp);
				set_Cmp_relation(irn, get_inversed_relation(get_Cmp_relation(irn)));
				std::swap(usage[irn].second[0], usage[irn].second[1]);
			}

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
			if (usage[irn].second[0].reg > 16 && usage[irn].second[1].reg > 16)
				gen_mov(mode, get_irn_n(irn, 0), usage[irn].second[0].reg, RAX);

			fprintf(out, "\tcmp%s ", operationSuffix(mode));
			load_or_imm(get_irn_n(irn, 1), usage[irn].second[1].reg);
			fprintf(out, ", ");

			if (usage[irn].second[0].reg > 16 && usage[irn].second[1].reg > 16)
				fprintf(out, "%s", constraintToRegister(RAX, mode));
			else
				load_or_reg(mode, usage[irn].second[0].reg);

			fprintf(out, "\n");
			output_phis(block.phi, block);
			fprintf(out, "\tj%s .L_%s_%ld\n", cond, get_entity_name(get_irg_entity(get_irn_irg(irn))), get_irn_node_nr((ir_node*) get_irn_link(irn)));
		}
		else if (is_Cond(irn))
			fprintf(out, "\tjmp .L_%s_%ld\n", get_entity_name(get_irg_entity(get_irn_irg(irn))), get_irn_node_nr((ir_node*) get_irn_link(irn)));
		else if (is_Jmp(irn))
		{
			output_phis(block.phi, block);
			fprintf(out, "\tjmp .L_%s_%ld\n", get_entity_name(get_irg_entity(get_irn_irg(irn))), get_irn_node_nr((ir_node*) get_irn_link(irn)));
		}
	}
	void CodeGen::output_normal(ir_node* irn)
	{
		ir_fprintf(out, "\t# (%ld) %F\n", get_irn_node_nr(irn), irn);

		if (is_Start(irn))
		{
			auto& writes = usage[irn].first;

			for (auto& w : writes)
			{
				if (w.reg && w.constraint < STACK)
					//TODO: get actual mode (from start <- proj T args <- proj wanted_mode arg_n ?)
					gen_mov(mode_P, NULL, w.constraint, w.reg);
			}
		}
		else if (is_Call(irn))
		{
			auto& reads = usage[irn].second;
			bool is_println = !strcmp("_COut_Mprintln", get_entity_name(get_Call_callee(irn)));

			//rescue registers
			std::vector<Constraint> to_push = {/*RAX, RCX, RDX,*/ RBX, /*RSP,*/ RBP, /*RSI, RDI, R8, R9,*/ R10, R11, R12, R13, R14, R15};

			for (auto it = to_push.begin(); it != to_push.end(); it++)
				fprintf(out, "\tpush %s\n", constraintToRegister(*it, mode_P));

			for (int i = 2; i < get_irn_arity(irn); i++)
			{
				ir_mode* arg_mode = get_irn_mode(get_irn_n(irn, i));
				char const* os = operationSuffix(arg_mode);

				if (i < 8)
				{
					// first 6 args in registers
					if (i == 2 && is_println)
					{
#ifndef __APPLE__
						fprintf(out, "\tmovq $.LC0, %%rdi\n");
#else
						fprintf(out, "\tmovabs $.LC0, %%rdi\n");
#endif
					}
					else
						gen_mov(arg_mode, get_irn_n(irn, i), reads[i].reg + (reads[i].reg <= 16 ? 0 : 8), reads[i].constraint);
				}
				else
				{
					// remaining args on stack
					fprintf(out, "\tmov%s ", os);
					load_or_imm(get_irn_n(irn, i), reads[i].reg + (reads[i].reg <= 16 ? 0 : 8));

					if (reads[i].reg > 16)
					{
						char const* rax = constraintToRegister(RAX, arg_mode);
						fprintf(out, ", %s\n\tmov%s %s", rax, os, rax);
					}

					fprintf(out, ", %zd(%%rsp)\n", (ssize_t) - 8 * (get_irn_arity(irn) - i));
				}
			}

			if (is_println)
			{
				fprintf(out, "\tpushq %%rsp\n\tpushq (%%rsp)\n\tandq $-16, %%rsp\n");
#ifdef __APPLE__
				fprintf(out, "\tcall _printf\n");
#else
				fprintf(out, "\tcall printf\n");
#endif
				fprintf(out, "\tmovq 8(%%rsp), %%rsp\n");

				//restore registers
				for (auto it = to_push.rbegin(); it != to_push.rend(); it++)
					fprintf(out, "\tpop %s\n", constraintToRegister(*it, mode_P));
			}
			else
			{
				if (get_irn_arity(irn) > 8)
					fprintf(out, "\tsub $%zd, %%rsp\n", (ssize_t) 8 * (get_irn_arity(irn) - 8));

#ifdef __APPLE__
				fprintf(out, "\tcall _%s\n", get_entity_name(get_Call_callee(irn)));
#else
				fprintf(out, "\tcall %s\n", get_entity_name(get_Call_callee(irn)));
#endif

				if (get_irn_arity(irn) > 8)
					fprintf(out, "\tadd $%zd, %%rsp\n", (ssize_t) 8 * (get_irn_arity(irn) - 8));

				//restore registers
				for (auto it = to_push.rbegin(); it != to_push.rend(); it++)
					fprintf(out, "\tpop %s\n", constraintToRegister(*it, mode_P));

				if (!usage[irn].first.empty())
				{
					//TODO: get actual mode (from call <- proj T result <- proj wanted_mode 0 ?)
					fprintf(out, "\tmov %%rax, ");
					load_or_reg(mode_P, usage[irn].first[0].reg);
					fprintf(out, "\n");
				}
			}
		}
		else if (is_Add(irn) || is_Sub(irn) || is_Mul(irn) || is_Shr(irn) || is_Shrs(irn) || is_Shl(irn))
		{
			char const* op = is_Add(irn) ? "add" : is_Sub(irn) ? "sub" : is_Mul(irn) ? "imul" : is_Shr(irn) ? "shr" : is_Shrs(irn) ? "sar" : "shl";
			ir_mode* mode = get_irn_mode(irn);
			//sub a, b stores b - a in b -> output "op second first"
			gen_binary_op(irn, mode, op, is_Add(irn) || is_Mul(irn));
		}
		else if (is_Minus(irn))
		{
			/* TODO: continue here stack to reg/stack conversion here */
			ir_mode* mode = get_irn_mode(irn);
			char const* os = operationSuffix(mode);

			if (usage[irn].second[0].reg == usage[irn].first[0].reg)
			{
				fprintf(out, "\tneg%s ", os);
				load_or_reg(mode, usage[irn].second[0].reg);
				fprintf(out, "\n");
			}
			else if (usage[irn].first[0].reg <= 16)
			{
				char const* rs = constraintToRegister(usage[irn].first[0].reg, mode);
				fprintf(out, "\tmov%s ", os);
				load_or_imm(get_irn_n(irn, 0), usage[irn].second[0].reg);
				fprintf(out, ", %s\n\tneg%s %s\n", rs, os, rs);
			}
			else
			{
				char const* rs = constraintToRegister(RAX, mode);
				fprintf(out, "\tmov%s ", os);
				load_or_imm(get_irn_n(irn, 0), usage[irn].second[0].reg);
				fprintf(out, ", %s\n\tneg%s %s\n\tmov%s %s, ", rs, os, rs, os, rs);
				load_or_reg(mode, usage[irn].first[0].reg);
				fprintf(out, "\n");
			}
		}
		else if (is_Div(irn) || is_Mod(irn))
		{
			ir_mode* mode = is_Div(irn) ? get_Div_resmode(irn) : get_Mod_resmode(irn);
			char const* os = operationSuffix(mode);
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

			ir_node* dividend_node = get_irn_n(irn, 2);

			if (is_Const(dividend_node) && __builtin_popcount(std::abs(get_tarval_long(get_Const_tarval(dividend_node)))) == 1)
			{
				long dividend = get_tarval_long(get_Const_tarval(dividend_node));

				if (get_mode_sign(mode))
				{
					gen_mov(mode, get_irn_n(irn, 1), usage[irn].second[1].reg, RAX);
					char const* rax = constraintToRegister(RAX, mode);
					char const* rdx = constraintToRegister(RDX, mode);

					unsigned long abs_dividend = std::abs(dividend);

					if (is_Div(irn))
					{
						fprintf(out, "\tlea%s %ld(%s), %s\n", os, abs_dividend - 1, rax, rdx);
						fprintf(out, "\ttest%s %s, %s\n", os, rax, rax);
						fprintf(out, "\tcmovs%s %s, %s\n", os, rdx, rax);
						fprintf(out, "\tsar%s $%d, %s\n", os, __builtin_ctzl(abs_dividend), rax);

						if (dividend < 0)
							fprintf(out, "\tneg%s %s\n", os, rax);
					}
					else
					{
						fprintf(out, "\tc%s\n", conv);
						fprintf(out, "\tshr%s $%d, %s\n", os, get_mode_size_bits(mode) - __builtin_ctzl(abs_dividend), rdx);
						fprintf(out, "\tadd%s %s, %s\n", os, rdx, rax);
						fprintf(out, "\tand%s $%ld, %s\n", os, abs_dividend - 1, rax);
						fprintf(out, "\tsub%s %s, %s\n", os, rdx, rax);
					}

					gen_mov(mode, NULL, RAX, usage[irn].first[0].reg);
				}
				else
				{
					char const* op = is_Div(irn) ? "shr" : "and";
					char const* rax = constraintToRegister(RAX, mode);
					long imm = is_Div(irn) ? __builtin_ctzl(dividend) : dividend - 1;

					if (usage[irn].second[1].reg == usage[irn].first[0].reg)
						fprintf(out, "\t%s%s $%ld, %s\n", op, os, imm, rax);
					else if (usage[irn].first[0].reg > 16)
					{
						fprintf(out, "\tmov%s ", os);
						load_or_imm(get_irn_n(irn, 1), usage[irn].second[1].reg);
						fprintf(out, ", %s\n\t%s%s $%ld, %s\n\tmov%s ", rax, op, os, imm, rax, os);
						load_or_reg(mode, usage[irn].first[0].reg);
						fprintf(out, "\n");
					}
					else
					{
						char const* reg = constraintToRegister(usage[irn].first[0].reg, mode);
						fprintf(out, "\tmov%s ", os);
						load_or_imm(get_irn_n(irn, 1), usage[irn].second[1].reg);
						fprintf(out, ", %s\n\t%s%s $%ld, %s\n", reg, op, os, imm, reg);
					}
				}
			}
			else
			{
				gen_mov(mode, get_irn_n(irn, 1), usage[irn].second[1].reg, RAX);

				if (!usage[irn].second[2].reg)
					gen_mov(mode, get_irn_n(irn, 2), 0, RCX);

				fprintf(out, "\tc%s\n\t%sdiv%s ", conv, get_mode_sign(mode) ? "i" : "", os);
				load_or_reg(mode, usage[irn].second[2].reg ? usage[irn].second[2].reg : (size_t) RCX);
				fprintf(out, "\n");
				gen_mov(mode, NULL, is_Div(irn) ? RAX : RDX, usage[irn].first[0].reg);
			}
		}
		else if (is_Conv(irn))
		{
			// conversion to a larger mode, zero or sign expand depending on signedness of smaller node
			ir_mode* old_mode = get_irn_mode(get_irn_n(irn, 0));
			ir_mode* mode = get_irn_mode(irn);
			char const* old_os = operationSuffix(old_mode);

			if (usage[irn].first[0].reg > 16)
			{
				char const* rax = constraintToRegister(RAX, mode);
				fprintf(out, "\tmov%cx%s ", get_mode_sign(old_mode) ? 's' : 'z', old_os);
				load_or_imm(get_irn_n(irn, 0), usage[irn].second[0].reg);
				fprintf(out, ", %s\n\tmov%s %s, ", rax, operationSuffix(mode), rax);
				load_or_reg(mode, usage[irn].second[0].reg);
				fprintf(out, "\n");
			}
			else
			{
				fprintf(out, "\tmov%cx%s ", get_mode_sign(old_mode) ? 's' : 'z', old_os);
				load_or_reg(old_mode, usage[irn].second[0].reg);
				fprintf(out, ", %s\n", constraintToRegister(usage[irn].first[0].reg, mode));
			}
		}
		else if (is_Lea(irn))
		{
			ir_mode* mode = get_irn_mode(irn);
			char const* os = operationSuffix(mode);
			char const* rax = constraintToRegister(RAX, mode);
			char const* rdx = constraintToRegister(RDX, mode);

			if (usage[irn].second[1].reg > 16)
				gen_mov(mode, get_irn_n(irn, 1), usage[irn].second[1].reg, RAX);

			if (get_irn_arity(irn) >= 3 && usage[irn].second[2].reg > 16)
				gen_mov(mode, get_irn_n(irn, 2), usage[irn].second[2].reg, RDX);

			fprintf(out, "\tlea%s %ld(%s", os, get_tarval_long(get_Const_tarval(get_irn_n(irn, 0))), usage[irn].second[1].reg > 16 ? rax : constraintToRegister(usage[irn].second[1].reg, mode));

			if (get_irn_arity(irn) >= 3)
				fprintf(out, ", %s", usage[irn].second[2].reg > 16 ? rdx : constraintToRegister(usage[irn].second[2].reg, mode));

			if (get_irn_arity(irn) == 4)
				fprintf(out, ", %ld", get_tarval_long(get_Const_tarval(get_irn_n(irn, 3))));

			fprintf(out, "), %s\n", usage[irn].first[0].reg > 16 ? rax : constraintToRegister(usage[irn].first[0].reg, mode));

			if (usage[irn].first[0].reg > 16)
				gen_mov(mode, NULL, RAX, usage[irn].first[0].reg);
		}
		else if (is_Load(irn))
		{
			ir_mode* mode = get_Load_mode(irn);
			char const* os = operationSuffix(mode);
			char const* rax = constraintToRegister(RAX, mode_P);
			char const* rdx = constraintToRegister(RDX, mode);

			if (!usage[irn].second[1].reg || usage[irn].second[1].reg > 16)
				gen_mov(mode_P, get_irn_n(irn, 1), usage[irn].second[1].reg, RAX);

			fprintf(out, "\tmov%s (%s), %s\n", os, !usage[irn].second[1].reg || usage[irn].second[1].reg > 16 ? rax : constraintToRegister(usage[irn].second[1].reg, mode_P), usage[irn].first[0].reg > 16 ? rdx : constraintToRegister(usage[irn].first[0].reg, mode));

			if (usage[irn].first[0].reg > 16)
				gen_mov(mode, NULL, RDX, usage[irn].first[0].reg);
		}
		else if (is_Store(irn))
		{
			ir_mode* mode = get_irn_mode(get_irn_n(irn, 2));
			char const* os = operationSuffix(mode);
			char const* rax = constraintToRegister(RAX, mode_P);
			char const* rdx = constraintToRegister(RDX, mode);

			if (!usage[irn].second[1].reg || usage[irn].second[1].reg > 16)
				gen_mov(mode_P, get_irn_n(irn, 1), usage[irn].second[1].reg, RAX);

			if (usage[irn].second[2].reg > 16)
			{
				gen_mov(mode, get_irn_n(irn, 2), usage[irn].second[2].reg, RDX);
				fprintf(out, "\tmov%s %s", os, rdx);
			}
			else
			{
				fprintf(out, "\tmov%s ", os);
				load_or_imm(get_irn_n(irn, 2), usage[irn].second[2].reg);
			}

			fprintf(out, ", (%s)\n", !usage[irn].second[1].reg || usage[irn].second[1].reg > 16 ? rax : constraintToRegister(usage[irn].second[1].reg, mode_P));
		}
		else
		{
			ir_printf("\nNo idea how to emit code for (%ld) %F\n", get_irn_node_nr(irn), irn);
			abort();
		}
	}
}
