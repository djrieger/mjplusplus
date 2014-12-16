#include "Node.hpp"

namespace firm {
	Node::Node(ir_node* node): node(node) {

	}

	ir_tarval* Node::getTarval() const {
		return (ir_tarval*)get_irn_link(node);
	}

	void Node::setTarval(ir_tarval* tarval) 
	{
		set_irn_link(node, (void*) tarval);
	}

	void Node::setTarvalToBad() 
	{
		setTarval(tarval_bad);
	}

	void Node::setTarvalToUnknown() 
	{
		setTarval(tarval_unknown);
	}

	Node Node::getChild(unsigned int i) const {
		if (i < getChildCount())
			return Node(get_irn_n(node, i));
		else
			throw "Tried to access child out of bounds";
	}

	unsigned int Node::getChildCount() const 
	{
		return get_irn_arity(node);
	}

	bool Node::hasChildren() const {
		return get_irn_arity(node) > 0;
	}

	shptr<vec<Node>> Node::getChildren() const {
		auto children = std::make_shared<vec<Node>>();
		for (unsigned int i = 0; i < getChildCount(); ++i)
			children->push_back(getChild(i));
		return children;
	}

	shptr<vec<std::pair<Node, unsigned int>>> Node::getOuts() const
	{
		auto outs = std::make_shared<vec<std::pair<Node, unsigned int>>>();

		for (ir_edge_t const* oe = get_irn_out_edge_first(node); oe; oe = get_irn_out_edge_next(node, oe, EDGE_KIND_NORMAL))
			outs->emplace_back(Node(get_edge_src_irn(oe)), get_edge_src_pos(oe));

		return outs;
	}	
}


//ir_tarval* newTarval = new_tarval_from_long(val, mode_Is);
			

