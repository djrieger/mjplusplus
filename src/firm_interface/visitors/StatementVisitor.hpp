#ifndef STATEMENT_VISITOR_HPP
#define STATEMENT_VISITOR_HPP

#include "FirmVisitor.hpp"
#include "MemberVisitor.hpp"
#include "../../ast/IfStatement.hpp"
#include "../../ast/WhileStatement.hpp"
#include "../../ast/ReturnStatement.hpp"
#include "../../ast/Block.hpp"
#include "../../ast/ExpressionStatement.hpp"
#include "../../ast/LVDStatement.hpp"

namespace firm
{
	namespace visitor
	{
		class StatementVisitor : public FirmVisitor
		{
			protected:

				/**
				 * The calling MemberVisitor
				 */
				MemberVisitor& memberVisitor;

				/**
				 * Create, mature and set as current a firm block for a then or else statement
				 * @param thenOrElseStmt	The then or else statement of an ast::IfStatement
				 * @param precedingProjection	The Firm projection node to use as predecessor for the then/else block
				 * @param exitBlock	The exitBlock to use as successor for the Jump node
				 */
				void visitThenOrElse(ir_node* thenOrElseBlock, shptr<const ast::stmt::Statement> thenOrElseStmt, ir_node* exitBlock);
			public:

				/**
				 * Constrcuts a StatementVisitor with the given MemberVisitor.
				 * Sets the owner to the MemberVisitor's owner.
				 * @param memberVisitor the given MemberVisitor
				 */
				StatementVisitor(MemberVisitor& memberVisitor);

				/**
				 * Evaluates the condition of the visited IfStatement using a BoolExpressionVisitor
				 * and then evaluates the BlockStatement of the then and/or else-blocks.
				 * Afterwards it matures and sets the current block accordingly.
				 * @param ifStatement the visited IfStatement
				 * @see BoolExpressionVisitor
				 */
				virtual void visit(shptr<const ast::stmt::IfStatement> ifStatement);

				/**
				 * Evaluates the condition of the visited WhileStatement using a BoolExpressionVisitor
				 * and then evaluates the WhileStatement's BlockStatment.
				 * Afterwards it matures and sets the current block accordingly.
				 * To prevent issues with infinite loops, the while-condition block is kept alive.
				 * @param whileStmt the visited WhileStatement
				 * @see BoolExpressionVisitor
				 */
				virtual void visit(shptr<const ast::stmt::WhileStatement> whileStmt);

				/**
				 * Creates a Return node using the visited ReturnStatement by evaluating its
				 * expression using an ExpressionVisitor.
				 * This Return node is used as "end node" of the current function graph.
				 * @param returnStmt the visited ReturnStatement
				 * @see ExpressionVisitor
				 */
				virtual void visit(shptr<const ast::stmt::ReturnStatement> returnStmt);

				/**
				 * Iterates over the statements in the visited Block.
				 * @param blockStmt the visited Block
				 */
				virtual void visit(shptr<const ast::stmt::Block> blockStmt);

				/**
				 * Evaluates the expression of the visited ExpressionStatement
				 * using an ExpressionVisitor.
				 * @param exprStmt the visited ExpressionStatement
				 * @see ExpressionVisitor
				 */
				virtual void visit(shptr<const ast::stmt::ExpressionStatement> exprStmt);

				/**
				 * Retrieves the variables position in the current context from the FirmInterface
				 * based on the visited LVDStatement's name.
				 * If the LVDStatement has an initialization, the value at the variable's position
				 * is set to the value of the initialization's expression.
				 * This is evaluated using an ExpressionVisitor.
				 * If there is no initialization, then the value is set to a null-node.
				 * @param lvdStmt the visited LVDStatement
				 * @see FirmInterface
				 * @see ExpressionVisitor
				 */
				virtual void visit(shptr<const ast::stmt::LVDStatement> lvdStmt);
		};
	}
}
#endif
