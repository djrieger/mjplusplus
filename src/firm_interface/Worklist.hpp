#ifndef WORKLIST_HPP
#define WORKLIST_HPP

#include <libfirm/firm.h>
#include <queue>
#include <functional>
#include <set>
#include <unordered_map>
#include "../globals.hpp"
#include "ConstantFolder.hpp"
#include "Node.hpp"

namespace firm
{
	class Worklist
	{
		private:
			ir_graph* functionGraph;
			std::queue<ir_node*> worklist;
			ConstantFolder& handler;
			std::unordered_map<ir_node*, bool> isQueued;

			void cleanUp();
			void processChildren(Node node, std::function<void (Node leftChild, Node rightChild)> fun);

			void replaceGeneric(Node node);
			void replaceAdd(Node node);
			void replaceSub(Node node);
			void replaceMul(Node node);
			void replaceMinus(Node node);
			void replaceConv(Node node);
		public:
			Worklist(ir_graph* functionGraph, ConstantFolder& handler);
			void run();

	};
}

#endif