#include "ExpressionVisitor.hpp"
#include "BoolExpressionVisitor.hpp"
#include "PostfixOpsVisitor.hpp"
#include "VariableDeclVisitor.hpp"
#include <sstream>
#include <algorithm>

namespace firm
{
	namespace visitor
	{
		ExpressionVisitor::ExpressionVisitor()
		{
			;
		}

		void ExpressionVisitor::visitBinaryExpression(
		    shptr<ast::be::BinaryExpression const> binExpr,
		    std::function<ir_node* (ir_node*, ir_node*)> createResultNode)
		{
			binExpr->getLeftChild()->accept(*this);
			ir_node* left = this->resultNode;

			binExpr->getRightChild()->accept(*this);
			ir_node* right = this->resultNode;

			this->resultNode = createResultNode(left, right);
		}

		ir_node* ExpressionVisitor::createThenOrElseBlock(ir_node* block, bool boolValue, ir_node* exitBlock)
		{
			mature_immBlock(block);
			set_cur_block(block);
			ir_node* constBoolNode = FirmInterface::getInstance().createNodeForBooleanConstant(boolValue);
			add_immBlock_pred(exitBlock, new_Jmp());

			return constBoolNode;
		}

		void ExpressionVisitor::visitBoolExpression(shptr<ast::Expression const> expression)
		{
			ir_node* thenBlock = new_immBlock();
			ir_node* elseBlock = new_immBlock();
			ir_node* exitBlock = new_immBlock();

			BoolExpressionVisitor condVisitor(thenBlock, elseBlock);
			expression->accept(condVisitor);

			ir_node* bools[2] = { createThenOrElseBlock(thenBlock, true, exitBlock),
			                      createThenOrElseBlock(elseBlock, false, exitBlock)
			                    };

			mature_immBlock(exitBlock);
			set_cur_block(exitBlock);

			resultNode = new_Phi(2, bools, FirmInterface::getInstance().getBooleanMode());
			resultType = new_type_primitive(FirmInterface::getInstance().getBooleanMode());
		}

		// primary expressions
		void ExpressionVisitor::visit(shptr<ast::pe::Bool const> boolExpr)
		{
			bool value = boolExpr->getValue();
			this->resultNode = FirmInterface::getInstance().createNodeForBooleanConstant(value);
		}

		void ExpressionVisitor::visit(shptr<ast::pe::Ident const> identExpr)
		{
			// Param / Local Variable
			// Member
			// System

			auto decl = identExpr->getDeclaration();

			if (decl)
			{
				//don't get this pointer here, we might be in main
				VariableDeclVisitor vdVisitor(NULL);
				vdVisitor.setDoStore(doStore);
				decl->accept(vdVisitor);
				resultNode = vdVisitor.getResultNode();
				resultType = vdVisitor.getResultType();
				varNum = vdVisitor.getVarNum();
			}
			else
			{
				// got System
				/*
				lexer::Token sit {lexer::Token::Token_type::TOKEN_IDENT, lexer::Token::getTableReference("$System"), { -1, 0}};
				auto si = std::make_shared<ast::Ident>(sit);
				auto s = std::make_shared<ast::Type>(si);
				resultType = FirmInterface::getInstance().getType(s);

				ir_entity* system_ent = FirmInterface::getInstance().getSystemNode();
				ir_node* system_addr = new_Address(system_ent);
				ir_node* load = new_Load(get_store(), system_addr, get_type_mode(resultType), resultType, cons_none);
				set_store(new_Proj(load, mode_M, pn_Load_M));
				resultNode = new_Proj(load, mode_P, pn_Load_res);
				*/
				resultNode = NULL;
			}
		}

		void ExpressionVisitor::visit(shptr<ast::pe::Integer const> integerExpr)
		{
			int64_t x = atoll(integerExpr->getStringValue().c_str());
			this->resultNode = FirmInterface::getInstance().createNodeForIntegerConstant(x);
		}

		void ExpressionVisitor::visit(shptr<ast::pe::MethodInvocation const> methodInvocationExpr)
		{
			std::tie(resultNode, resultType) = FirmInterface::getInstance().createNodeForMethodCall(methodInvocationExpr);
		}

		void ExpressionVisitor::visit(shptr<ast::pe::NewArrayExpression const> newArrayExpr)
		{
			newArrayExpr->getSize()->accept(*this);
			ir_type* t = FirmInterface::getInstance().getType(newArrayExpr->getType()->de_array());
			resultNode = FirmInterface::getInstance().createNodeForCallocCall(resultNode, get_type_size_bytes(t));
			resultType = FirmInterface::getInstance().getType(newArrayExpr->getType());
		}

		void ExpressionVisitor::visit(shptr<ast::pe::NewObjectExpression const> newObjectExpr)
		{
			ir_node* one = FirmInterface::getInstance().createNodeForIntegerConstant(1);
			auto at = std::make_shared<ast::Type>(newObjectExpr->getIdent());
			resultType = FirmInterface::getInstance().getType(at);
			resultNode = FirmInterface::getInstance().createNodeForCallocCall(one, get_type_size_bytes(get_pointer_points_to_type(resultType)));
		}

		void ExpressionVisitor::visit(shptr<ast::pe::Object const> objectExpr)
		{
			switch (objectExpr->getObjectType())
			{
				case ast::pe::Object::NULL_OBJECT:
				{
					this->resultNode = FirmInterface::getInstance().createNullPointerNode();
					break;
				}

				case ast::pe::Object::THIS_OBJECT:
				{
					this->resultNode = get_value(0, mode_P);
					break;
				}
			}
		}

		// unary expressions
		void ExpressionVisitor::visit(shptr<ast::ue::Neg const> negExpr)
		{
			shptr<ast::Expression> child = negExpr->getChild();
			child->accept(*this);

			if (negExpr->getSize() & 1)
				this->resultNode = new_Minus(this->resultNode, FirmInterface::getInstance().getIntegerMode());
		}

		void ExpressionVisitor::visit(shptr<ast::ue::Not const> notExpr)
		{
			visitBoolExpression(notExpr);
		}

		// binary expressions
		void ExpressionVisitor::visit(shptr<ast::be::Eq const> eqExpr)
		{
			// Assignment

			doStore = true;
			eqExpr->getLeftChild()->accept(*this);
			doStore = false;

			ir_node* storeNode = resultNode;
			ir_type* storeType = resultType;
			int storeVarNum = varNum;

			eqExpr->getRightChild()->accept(*this);

			if (storeNode)
			{
				ir_node* store = new_Store(get_store(), storeNode, resultNode, storeType, cons_none);
				set_store(new_Proj(store, mode_M, pn_Store_M));
			}
			else
				set_value(storeVarNum, resultNode);
		}

		void ExpressionVisitor::visit(shptr<ast::be::AndAnd const> andAndExpr)
		{
			visitBoolExpression(andAndExpr);
		}

		void ExpressionVisitor::visit(shptr<ast::be::OrOr const> orOrExpr)
		{
			visitBoolExpression(orOrExpr);
		}

		void ExpressionVisitor::visit(shptr<ast::be::EqEq const> eqEqExpr)
		{
			visitBoolExpression(eqEqExpr);
		}

		void ExpressionVisitor::visit(shptr<ast::be::NotEq const> notEqExpr)
		{
			visitBoolExpression(notEqExpr);
		}

		void ExpressionVisitor::visit(shptr<ast::be::GreaterThan const> greaterThanExpr)
		{
			visitBoolExpression(greaterThanExpr);
		}

		void ExpressionVisitor::visit(shptr<ast::be::GreaterThanEq const> greaterThanEqExpr)
		{
			visitBoolExpression(greaterThanEqExpr);
		}

		void ExpressionVisitor::visit(shptr<ast::be::LessThan const> lessThanExpr)
		{
			visitBoolExpression(lessThanExpr);
		}

		void ExpressionVisitor::visit(shptr<ast::be::LessThanEq const> lessThanEqExpr)
		{
			visitBoolExpression(lessThanEqExpr);
		}

		void ExpressionVisitor::visit(shptr<ast::be::Plus const> plusExpr)
		{
			visitBinaryExpression(plusExpr, [] (ir_node * left, ir_node * right) -> ir_node*
			{
				return new_Add(left, right, FirmInterface::getInstance().getIntegerMode());
			});
		}

		void ExpressionVisitor::visit(shptr<ast::be::Minus const> minusExpr)
		{
			visitBinaryExpression(minusExpr, [] (ir_node * left, ir_node * right) -> ir_node*
			{
				return new_Sub(left, right, FirmInterface::getInstance().getIntegerMode());
			});
		}

		void ExpressionVisitor::visit(shptr<ast::be::Mod const> modExpr)
		{
			visitBinaryExpression(modExpr, [] (ir_node * left, ir_node * right) -> ir_node*
			{
				return FirmInterface::getInstance().createModOperation(left, right);
			});
		}

		void ExpressionVisitor::visit(shptr<ast::be::Mult const> multExpr)
		{
			visitBinaryExpression(multExpr, [] (ir_node * left, ir_node * right) -> ir_node*
			{
				return new_Mul(left, right, FirmInterface::getInstance().getIntegerMode());
			});
		}

		void ExpressionVisitor::visit(shptr<ast::be::Slash const> slashExpr)
		{
			visitBinaryExpression(slashExpr, [] (ir_node * left, ir_node * right) -> ir_node*
			{
				return FirmInterface::getInstance().createDivOperation(left, right);
			});
		}

		void ExpressionVisitor::visit(shptr<ast::be::Invalid const>)
		{
			std::cerr << "ExpressionVisitor visited Invalid: in" << __FILE__
			          << " at " << __LINE__ << std::endl;
		}

		// postfix expression
		void ExpressionVisitor::visit(shptr<ast::po::PostfixExpression const> postfixExpression)
		{
			bool old_do_store = doStore;
			// only the last PostfixOp does the store - or the PrimaryExpression if there are no PostfixOps
			doStore &= postfixExpression->getPostfixOps()->empty();

			postfixExpression->getChild()->accept(*this);
			auto pops = postfixExpression->getPostfixOps();

			if (!pops->empty())
			{
				PostfixOpsVisitor popsVisitor(*this);

				if (resultNode)
				{
					for (auto it = pops->begin(); it != pops->end() - 1; it++)
					{
						(*it)->accept(popsVisitor);
						resultNode = popsVisitor.getResultNode();
						resultType = popsVisitor.getResultType();
					}

					// last PostfixOp may store
					doStore = old_do_store;

					if (doStore)
						popsVisitor.setDoStore(true);

					pops->back()->accept(popsVisitor);
					resultNode = popsVisitor.getResultNode();
					resultType = popsVisitor.getResultType();
					varNum = popsVisitor.getVarNum();
				}
				else
				{
					/*
					* resultNode == NULL means we got our $System as Ident.
					* (Maybe adding some sort of flag for this might be useful in the future.)
					* That means this PostfixExpression can currently only be
					* 			System.out.println(EXPRESSION);
					* So we can jump to the last PostfixOp without handling the field access etc.
					* The PostfixOpVisitor will recognize our println and handle it accordingly.
					*/
					auto printlnCall = pops->back();
					printlnCall->accept(popsVisitor);

					resultNode = popsVisitor.getResultNode();
					resultType = popsVisitor.getResultType();
				}

			}
		}
	}
}
