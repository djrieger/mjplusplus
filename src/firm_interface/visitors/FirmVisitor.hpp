#ifndef FIRM_VISITOR_HPP
#define FIRM_VISITOR_HPP

#include <libfirm/firm.h>
#include "../FirmInterface.hpp"
#include "../../ast/ASTVisitor.hpp"

namespace firm
{
	namespace visitor
	{

		class FirmVisitor : public ast::ASTVisitor
		{
			protected:

				/**
				 * The owner of this visitor
				 */
				ir_type* owner;
			public:
				FirmVisitor();

				/**
				 * Sets the owner to the given one. Should be a pointer to the type.
				 * @param owner the given owner
				 */
				virtual void setOwner(ir_type* owner);

				/**
				 * Returns the owner.
				 * @return owner
				 */
				virtual ir_type* getOwner() const;
		};
	}
}

#endif //FIRM_VISITOR_HPP
