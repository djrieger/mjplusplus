#ifndef VISITOR_WITH_RESULT_NODE_HPP
#define VISITOR_WITH_RESULT_NODE_HPP

#include "FirmVisitor.hpp"

namespace firm
{
	namespace visitor
	{
		class VisitorWithResultNode : public FirmVisitor
		{
			protected:
				ir_node* resultNode;
				ir_type* resultType;
				bool doStore;
				int varNum;

			public:
				VisitorWithResultNode();

				ir_node* getResultNode() const;
				ir_type* getResultType() const;
				void setDoStore(bool s);
				int getVarNum() const;
		};
	}
}

#endif
