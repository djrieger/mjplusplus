#ifndef WORKLIST_HPP
#define WORKLIST_HPP

#include <libfirm/firm.h>
#include <queue>
#include <functional>
#include <set>
#include <unordered_map>
#include "../globals.hpp"
#include "ConstantFolder.hpp"

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
			void processChildren(ir_node* node, std::function<void (ir_node* leftChild, ir_tarval* leftTarval, ir_node* rightChild, ir_tarval* rightTarval)> fun);

			void replaceGeneric(ir_node* node);
			void replaceAdd(ir_node* node);
			void replaceSub(ir_node* node);
		public:
			Worklist(ir_graph* functionGraph, ConstantFolder& handler);
			void run();

	};
}

#endif