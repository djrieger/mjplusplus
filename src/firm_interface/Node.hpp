#ifndef FIRM_NODE_HPP
#define FIRM_NODE_HPP

#include <libfirm/firm.h>
#include "../globals.hpp"

namespace firm
{
	class Node {
		private:
			ir_node* node;
		public:
			Node(ir_node* node);	
			operator ir_node*() { return node; }
			ir_tarval* getTarval() const;
			void setTarval(ir_tarval* tarval);
			void setTarvalToBad();
			void setTarvalToUnknown();
			shptr<vec<std::pair<Node, unsigned int>>> getOuts() const;
			Node getChild(unsigned int i) const;
			unsigned int getChildCount() const;
			bool hasChildren() const;
			shptr<vec<Node>> getChildren() const;
	};
}

#endif