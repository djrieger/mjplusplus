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
				/**
				 * the result node
				 */
				ir_node* resultNode;

				/**
				 * the result type
				 */
				ir_type* resultType;

				/**
				 * store flag
				 */
				bool doStore;

				/**
				 * the current variable's number
				 */
				int varNum;

			public:

				/**
				 * Constructs a VisitorWithResultNode with default values (NULL, NULL, false, -1).
				 */
				VisitorWithResultNode();

				/**
				 * Returns the current result node.
				 * @return resultNode
				 */
				ir_node* getResultNode() const;

				/**
				 * Returns the current result type.
				 * @return resultType
				 */
				ir_type* getResultType() const;

				/**
				 * Sets the doStore flag with the given bool value.
				 * @param s the given bool value
				 */
				void setDoStore(bool s);

				/**
				 * Returns the current number of a variable.
				 * @return varNum
				 */
				int getVarNum() const;
		};
	}
}

#endif
