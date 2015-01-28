#include "LoadStoreOptimizer.hpp"
#include "../FirmInterface.hpp"
#include <iostream>
#include <algorithm>

namespace firm
{
	LoadStoreOptimizer::LoadStoreOptimizer(ir_graph* irg): GraphHandler(irg)
	{

	}

	void LoadStoreOptimizer::handle(Node node)
	{
		set_irn_link(node, NULL);

		if (node.hasChildren() && is_Proj(node.getChild(0)))
		{
			if (is_Load(node))
				handleLoad(node);
			else if (is_Store(node))
				handleStore(node);
		}
	}

	void LoadStoreOptimizer::handleLoad(Node node2)
	{
		Node loadProj = getLoadRes(node2);

		if ((ir_node*) loadProj == NULL)
			removeLoad(node2);

		Node node1 = node2.getChild(0).getChild(0);

		if (is_Load(node1))
			handleLoadLoad(node1, node2);
		else if (is_Store(node1))
			handleStoreLoad(node1, node2);

	}

	void LoadStoreOptimizer::handleStore(Node node2)
	{
		Node node1 = node2.getChild(0).getChild(0);

		if (is_Load(node1))
			handleLoadStore(node1, node2);
		else if (is_Store(node1))
			handleStoreStore(node1, node2);

	}

	void LoadStoreOptimizer::handleStoreLoad(Node node1, Node node2)
	{
		Node address = node2.getChild(1);

		if (address == node1.getChild(1))
		{

			for (auto& ne : node2.getOuts())
			{
				Node outChild = ne.first;

				if (outChild.getMode() == mode_M)
				{
					// Relink memory chain
					for (auto& ne2 : outChild.getOuts())
					{
						Node outChild2 = ne2.first;
						outChild2.setChild(ne2.second, node2.getChild(0));
					}
				}
				else
					replaceNode(outChild, node1.getChild(2));
			}
		}
	}

	void LoadStoreOptimizer::handleLoadLoad(Node node1, Node node2)
	{
		Node address = node2.getChild(1);

		if (address == node1.getChild(1))
		{
			//find return proj of first load
			Node loadProj = getLoadRes(node1);

			if ((ir_node*) loadProj == NULL)
				return;

			for (auto& ne : node2.getOuts())
			{
				Node outChild = ne.first;

				if (outChild.getMode() == mode_M)
				{
					// Relink memory chain
					for (auto& ne2 : outChild.getOuts())
					{
						Node outChild2 = ne2.first;
						outChild2.setChild(ne2.second, node2.getChild(0));
					}
				}
				else
					replaceNode(outChild, loadProj);
			}
		}
	}

	void LoadStoreOptimizer::handleStoreStore(Node node1, Node node2)
	{
		Node address = node2.getChild(1);
		Node prevMemNode = node1.getChild(0);

		if (address == node1.getChild(1) && prevMemNode.getMode() == mode_M)
		{
			// Relink memory chain
			node2.setChild(0, prevMemNode);
		}
	}

	void LoadStoreOptimizer::handleLoadStore(Node node1, Node node2)
	{
		Node address = node2.getChild(1);
		Node value = node2.getChild(2);

		if (address == node1.getChild(1) && value.hasChildren() && value.getChild(0) == node1)
		{
			for (auto& ne : node2.getOuts())
			{
				Node outChild = ne.first;

				if (outChild.getMode() == mode_M)
				{
					// Relink memory chain
					for (auto& ne2 : outChild.getOuts())
					{
						Node outChild2 = ne2.first;
						outChild2.setChild(ne2.second, node2.getChild(0));
					}
				}
			}
		}
	}

	void LoadStoreOptimizer::removeLoad(Node node)
	{
		for (auto& ne : node.getOuts())
		{
			Node outChild = ne.first;

			if (outChild.getMode() == mode_M)
			{
				// Relink memory chain
				for (auto& ne2 : outChild.getOuts())
				{
					Node outChild2 = ne2.first;
					outChild2.setChild(ne2.second, node.getChild(0));
				}
			}
		}
	}


	Node LoadStoreOptimizer::getLoadRes(Node node)
	{
		Node loadProj(NULL);

		for (auto& ne : node.getOuts())
		{
			Node outChild = ne.first;

			if (outChild.getMode() != mode_M)
			{
				loadProj = outChild;
				break;
			}
		}

		return loadProj;
	}

	void LoadStoreOptimizer::cleanUp(Node)
	{
	}
}
