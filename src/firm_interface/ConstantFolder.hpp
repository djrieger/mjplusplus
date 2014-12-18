#ifndef CONSTANT_FOLDER
#define CONSTANT_FOLDER

#include <libfirm/firm.h>
#include <set>
#include <queue>
#include <iostream>
#include "../globals.hpp"
#include "GraphOptimizer.hpp"
#include "Node.hpp"

namespace firm
{
	class ConstantFolder : public GraphOptimizer
	{
		protected:
			void optimizePhi(Node node);
			void updateTarvalForArithmeticNode(Node node);
			void updateTarvalAndExchangeMemory(ir_node* oldNode, ir_node* newNode);

			void handlePhi(ir_node* node);
			void handleMinus(ir_node* node);
			void handleMul(ir_node* node);
			void handleDivAndMod(ir_node* node);
			void handleProj(ir_node* node);
			void handleCmp(ir_node* node);
			void handleConv(Node node);

			bool tarvalIsZero(Tarval tarval);
			void processChildren(Node node, std::function<void (Node leftChild, Node rightChild)> fun);
			bool replaceGeneric(Node node);
			void replaceAdd(Node node);
			void replaceSub(Node node);
			void replaceMul(Node node);
			void replaceMinus(Node node);
			void replaceConv(Node node);
		public:
			virtual void handle(Node);
			virtual void cleanUp(Node node);
			ConstantFolder(ir_graph* irg);
	};
}
#endif
