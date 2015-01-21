#include "AddressModeOptimizer.hpp"
#include "FirmInterface.hpp"

namespace firm
{
	AddressModeOptimizer::AddressModeOptimizer(ir_graph* irg) : GraphOptimizer(irg)
	{
	}

	bool AddressModeOptimizer::handle_Mul(Node node)
	{
		if (x || scale)
		{
			if (base)
				return false;

			base = node;
			return true;
		}

		ir_node* new_scale = NULL;
		ir_node* new_x = NULL;
		bool use_base = false;

		for (auto& child : node.getChildren())
		{
			if (child.getOpcode() == iro_Const)
			{
				long val = child.getValue().getLong();

				switch (val)
				{
					case 1:
					case 2: // scale of 2 may be expressed either way
					case 4:
					case 8:
						new_scale = child;
						break;

					case 3:
					case 5:
					case 9:
						if (base)
							return false;

						new_scale = new_r_Const_long(get_irn_irg(node), node.getMode(), val - 1);
						use_base = true;
						break;

					default:
						if (base)
							return false;

						base = node;
						return true;
						break;
				}
			}
			else
				new_x = child;
		}

		if (!new_x || !new_scale)
			return false;

		x = new_x;
		scale = new_scale;

		if (use_base)
			base = new_x;

		return true;
	}

	bool AddressModeOptimizer::handle_Add(Node node, bool recurse)
	{
		for (auto& child : node.getChildren())
		{
			if (child.getOpcode() == iro_Const)
			{
				if (constant)
					return false;

				constant = child;
			}
			else if (child.getOpcode() == iro_Mul)
			{
				if (!handle_Mul(child))
					return false;
			}
			else if (child.getOpcode() == iro_Add)
			{
				if (recurse)
				{
					if (!handle_Add(child, false))
						return false;
				}
				else if (!base)
					base = child;
				else
					x = child;
			}
			else if (!base)
				base = child;
			else if (!x)
				x = child;
			else
				return false;
		}

		return true;
	}

	void AddressModeOptimizer::handle(Node node)
	{
		constant = NULL;
		base = NULL;
		x = NULL;
		scale = NULL;


		if (node.getOpcode() != iro_Add)
			return;

		ir_printf("(%ld) %F:\n", get_irn_node_nr(node), (ir_node*) node);

		if (!handle_Add(node, true))
			return;

		ir_printf("succsess? (%ld) %F, (%ld) %F, (%ld) %F, (%ld) %F\n", !constant ? 0 : get_irn_node_nr(constant), (ir_node*) constant, !base ? 0 : get_irn_node_nr(base), (ir_node*) base, !x ? 0 : get_irn_node_nr(x), (ir_node*) x, !scale ? 0 : get_irn_node_nr(scale), (ir_node*) scale);

		if (!x)
			return;

		if (!constant)
			constant = new_r_Const_long(get_irn_irg(node), node.getMode(), 0);

		if (!base)
		{
			if (get_tarval_long(get_Const_tarval(scale)) != 2)
				return;

			base = x;
			scale = new_r_Const_long(get_irn_irg(node), node.getMode(), 1);
		}

		ir_printf("using (%ld) %F, (%ld) %F, (%ld) %F, (%ld) %F\n", get_irn_node_nr(constant), (ir_node*) constant, get_irn_node_nr(base), (ir_node*) base, get_irn_node_nr(x), (ir_node*) x, !scale ? 0 : get_irn_node_nr(scale), (ir_node*) scale);
		ir_node* l = new_r_Lea(get_nodes_block(node), constant, base, x, scale, node.getMode());
		set_irn_link(node, l);
	}

	void AddressModeOptimizer::cleanUp(Node node)
	{
		ir_node* l = (ir_node*) get_irn_link(node);

		if (l && get_irn_op(l) == FirmInterface::getInstance().getLeaOp())
			replaceNode(node, l);
	}
}
