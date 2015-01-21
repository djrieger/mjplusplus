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

		ir_node* new_scale;
		ir_node* new_x;
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
					base = node;
				else
					x = node;
			}
			else if (!base)
				base = node;
			else if (!x)
				x = node;
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

		if (!handle_Add(node, true))
			return;

		if (!x)
			return;

		ir_node* l = new_r_Lea(get_nodes_block(node), constant, base, x, scale, node.getMode());
	}

	void AddressModeOptimizer::cleanUp(Node node)
	{
		;
	}
}
