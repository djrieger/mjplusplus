#ifndef CONSTANT_FOLDER
#define CONSTANT_FOLDER

#include <libfirm/firm.h>
#include <set>
#include <queue>
#include <iostream>
#include "../../globals.hpp"
#include "../GraphHandler.hpp"
#include "../Node.hpp"

namespace firm
{
	class ConstantFolder : public GraphHandler
	{
		protected:
			void optimizePhi(Node node);
			bool updateTarvalForArithmeticNode(Node node);

			void handlePhi(ir_node* node);
			void handleMinus(ir_node* node);
			void handleMul(ir_node* node);
			void handleDivAndMod(ir_node* node);

			//bool tarvalIsZero(Tarval tarval);
			//void processChildren(Node node, std::function<void (Node leftChild, Node rightChild)> fun);
			void replaceDivMod(Node node);
			bool replaceGeneric(Node node);
			//void replaceAdd(Node node);
			//void replaceSub(Node node);
			//void replaceMul(Node node);
			//void replaceMinus(Node node);
			//void replaceConv(Node node);
		public:
			virtual void handle(Node);
			virtual void cleanUp(Node node);
			ConstantFolder(ir_graph* irg);
	};
}
#endif
