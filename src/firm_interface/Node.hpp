#ifndef FIRM_NODE_HPP
#define FIRM_NODE_HPP

#include <libfirm/firm.h>
#include "../globals.hpp"
#include "Tarval.hpp"

namespace firm
{
	class Node
	{
		private:
			ir_node* node;
		public:
			Node() {}
			Node(ir_node* node);
			operator ir_node* ()
			{
				return node;
			}
			operator bool ()
			{
				return node != NULL;
			}
			bool operator==(Node const& otherNode) const
			{
				return node == otherNode.node;
			}
			bool operator==(ir_node* otherNode) const
			{
				return node == otherNode;
			}
			bool operator!=(Node const& otherNode) const
			{
				return !operator==(otherNode);
			}
			bool operator!=(ir_node* otherNode) const
			{
				return !operator==(otherNode);
			}
			Tarval getTarval() const;
			Tarval getValue() const;
			bool isConst() const;
			void setTarval(Tarval tarval);
			void setTarvalToBad();
			void setTarvalToUnknown();
			vec<std::pair<Node, unsigned int>> getOuts() const;
			Node getChild(unsigned int i) const;
			void setChild(unsigned int i, Node child);
			unsigned int getChildCount() const;
			bool hasChildren() const;
			vec<Node> getChildren() const;
			void replaceWith(ir_node* node, bool copyTarval = false);
			ir_mode* getMode() const;
			bool isNumericOrBool() const;
			bool isNumeric() const;
			long getNodeNumber() const;
			unsigned getOpcode() const;
	};
}

#endif
