#include "Node.hpp"

namespace firm
{
	Node::Node(ir_node* node): node(node)
	{

	}

	Tarval Node::getTarval() const
	{
		ir_tarval* tarval = (ir_tarval*)get_irn_link(node);

		if (!tarval)
			return Tarval();
		else
			return Tarval(tarval);
	}

	Tarval Node::getValue() const
	{
		return get_Const_tarval(node);
	}

	bool Node::isConst() const
	{
		return is_Const(node);
	}

	void Node::setTarval(Tarval tarval)
	{
		set_irn_link(node, (void*) (ir_tarval*) tarval);
	}

	Node Node::getChild(unsigned int i) const
	{
		if (i < getChildCount())
			return Node(get_irn_n(node, i));
		else
			throw "Tried to access child out of bounds";
	}

	void Node::setChild(unsigned int i, Node child)
	{
		set_irn_n(node, i, child);
	}

	unsigned int Node::getChildCount() const
	{
		return get_irn_arity(node);
	}

	bool Node::hasChildren() const
	{
		return get_irn_arity(node) > 0;
	}

	vec<Node> Node::getChildren() const
	{
		auto children = vec<Node>();

		for (unsigned int i = 0; i < getChildCount(); ++i)
			children.push_back(getChild(i));

		return std::move(children);
	}

	void Node::replaceWith(ir_node* node, bool copyTarval)
	{
		if (copyTarval)
			set_irn_link(node, (void*) (ir_tarval*)getTarval());

		exchange(this->node, node);
	}

	ir_mode* Node::getMode() const
	{
		return get_irn_mode(node);
	}

	bool Node::isNumericOrBool() const
	{
		auto mode = get_irn_mode(node);
		return mode == mode_Is || mode == mode_Bu || mode == mode_Lu;
	}

	bool Node::isNumeric() const
	{
		auto mode = get_irn_mode(node);
		return mode == mode_Is || mode == mode_Lu;
	}

	long Node::getNodeNumber() const
	{
		return get_irn_node_nr(node);
	}

	unsigned Node::getOpcode() const
	{
		return get_irn_opcode(node);
	}

	vec<std::pair<Node, unsigned int>> Node::getOuts() const
	{
		vec<std::pair<Node, unsigned int>> outs;

		for (ir_edge_t const* oe = get_irn_out_edge_first(node); oe; oe = get_irn_out_edge_next(node, oe, EDGE_KIND_NORMAL))
			outs.emplace_back(Node(get_edge_src_irn(oe)), get_edge_src_pos(oe));

		return outs;
	}
}


//ir_tarval* newTarval = new_tarval_from_long(val, mode_Is);


