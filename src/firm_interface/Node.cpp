#include "Node.hpp"

namespace firm {
	Node::Node(ir_node* node): node(node) {

	}

	Tarval Node::getTarval() const {
		ir_tarval* tarval = (ir_tarval*)get_irn_link(node);
		if (!tarval)
			return NULL;
		else
			return Tarval(tarval);
	}

	void Node::setTarval(Tarval tarval) 
	{
		set_irn_link(node, (void*) (ir_tarval*) tarval);
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

	void Node::replaceWith(ir_node* node, bool copyTarval)
	{
		if (copyTarval)
			set_irn_link(node, (void*) (ir_tarval*)getTarval());
		exchange(this->node, node);
	}
}


//ir_tarval* newTarval = new_tarval_from_long(val, mode_Is);
			

