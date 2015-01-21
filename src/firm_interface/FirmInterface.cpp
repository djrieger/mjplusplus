#include <iostream>
#include <fstream>

#include <cstring>
#include <stdio.h>
#include <cstdlib>

#include "FirmInterface.hpp"

#include "../lexer/token.hpp"
#include "../ast/LVDStatement.hpp"
#include "../ast/Block.hpp"
#include "visitors/ProgramVisitor.hpp"
#include "visitors/MemberVisitor.hpp"
#include "visitors/ExpressionVisitor.hpp"

#include "CodeGen.hpp"
#include "Optimizer.hpp"

namespace firm
{
	FirmInterface::FirmInterface()
	{
		ir_init();
		all_optimizations_off();
		printf("Initialized libFirm Version: %d.%d\n", ir_get_version_major(), ir_get_version_minor());
		be_parse_arg("isa=amd64");
		ir_mode* modeP = new_reference_mode("P64", irma_twos_complement, 64, 64);
		set_modeP(modeP);

		lea = new_ir_op(get_next_ir_opcodes(1), "Lea", op_pin_state_floats, irop_flag_none, oparity_variable, 0, 0);
	}

	void FirmInterface::setInput(std::string const& in)
	{
		in_name = in;
	}

	void FirmInterface::setOutput(std::string const& out)
	{
		out_name = out;
	}

	void FirmInterface::setFirmGraphOutput(bool output)
	{
		dumpFirmGraph = output;
	}

	void FirmInterface::convert(shptr<ast::Program> program)
	{
		visitor::ProgramVisitor v;

		try
		{
			program->accept(v);
		}
		catch (char const* e)
		{
			std::cerr << e << std::endl;
			throw;
		}
	}

	void FirmInterface::buildWithFirm()
	{
		std::cout << "Building with FIRM" << std::endl;
		FILE* o = fopen(out_name.c_str(), "w");

		try
		{
			be_main(o, in_name.c_str());
		}
		catch (...)
		{
			std::cerr << "Something went wrong" << std::endl;
			throw;
		}

		fprintf(o, "\t.file \"print.c\"\n\
\t.section .rodata.str1.1,\"aMS\",@progbits,1\n\
.LC0:\n\
\t.string \"%%d\\n\"\n\
\t.section .text.unlikely,\"ax\",@progbits\n\
.LCOLDB1:\n\
\t.text\n\
.LHOTB1:\n\
\t.p2align 4,,15\n\
\t.globl _COut_Mprintln\n\
\t.type _COut_Mprintln, @function\n\
_COut_Mprintln:\n\
.LFB11:\n\
\t.cfi_startproc\n\
\tsubq $8, %%rsp\n\
\t.cfi_def_cfa_offset 16\n\
\tmovl $.LC0, %%edi\n\
\tcall printf\n\
\taddq $8, %%rsp\n\
\t.cfi_def_cfa_offset 8\n\
\tret\n\
\t.cfi_endproc\n\
.LFE11:\n\
\t.size _COut_Mprintln, .-_COut_Mprintln\n\
\t.section .text.unlikely\n\
.LCOLDE1:\n\
\t.text\n\
.LHOTE1:\n\
\t.ident \"GCC: (Debian 4.9.1-19) 4.9.1\"\n\
\t.section .note.GNU-stack,\"\",@progbits\n\
"
		       );
		fclose(o);
	}

	void FirmInterface::build()
	{
		std::cout << "Building" << std::endl;
		FILE* o = fopen(out_name.c_str(), "w");

		try
		{
			CodeGen::assemble(o);
		}
		catch (...)
		{
			std::cerr << "Something went wrong" << std::endl;
			throw;
		}

#ifndef __APPLE__
		fprintf(o, "\t.section .rodata.str1.1,\"aMS\",@progbits,1\n");
#endif
		fprintf(o, ".LC0:\n\t.string \"%%d\\n\"\n");
		fclose(o);
	}

	void FirmInterface::outputFirmGraph(ir_graph* irg, std::string suffix)
	{
		if (dumpFirmGraph)
			dump_ir_graph(irg, suffix.c_str());
	}

	void FirmInterface::optimize(ir_graph* irg)
	{
		outputFirmGraph(irg, "orig");
		Optimizer opt(irg);
		opt.setOptimizationFlag(opt.DEFAULT);
		opt.run();
		outputFirmGraph(irg, "optimized");
	}

	ir_entity* FirmInterface::createMethodEntity(ir_type* owner, shptr<ast::MethodDeclaration const> methodDeclaration)
	{
		std::string mangledMethodName = methodDeclaration->mangle();

		unsigned int paramsCount = methodDeclaration->getParameters()->size();
		bool hasReturnType = !methodDeclaration->getReturnType()->isVoid();
		ir_type* methodType = new_type_method(paramsCount + 1, hasReturnType);

		// this pointer as first parameter
		// TODO: owner must be firm pointer type
		auto ast_class_type = std::make_shared<ast::Type>(methodDeclaration->getDeclaration()->getIdent());
		ir_type* class_ref_type = getType(ast_class_type);
		set_method_param_type(methodType, 0, class_ref_type);

		int i = 1;

		for (auto& param : *methodDeclaration->getParameters())
		{
			auto type = FirmInterface::getInstance().getType(param->getType());
			set_method_param_type(methodType, i, type);
			i++;
		}

		if (hasReturnType)
		{
			auto type = FirmInterface::getInstance().getType(methodDeclaration->getReturnType());
			set_method_res_type(methodType, 0, type);
		}

		ir_entity* ent = new_entity(owner, new_id_from_str(mangledMethodName.c_str()), methodType);
		return ent;
	}

	std::tuple<ir_node*, ir_type*> FirmInterface::createNodeForMethodCall(ir_node* caller,
	        shptr<ast::Arguments const> arguments,
	        shptr<ast::MethodDeclaration const> methodDeclaration)
	{
		auto classIdent = methodDeclaration->getDeclaration()->getIdent();
		auto classAstType = std::make_shared<ast::Type>(classIdent);
		ir_type* class_type = FirmInterface::getInstance().getType(classAstType);

		auto method_name = methodDeclaration->mangle();

		ir_type* owner = get_pointer_points_to_type(class_type);
		ir_entity* method_ent = getMethodEntity(owner, method_name);

		int argc = arguments->getArgumentsSize() + 1;

		ir_node** in = (ir_node**) calloc(argc, sizeof(ir_node*));
		int in_counter = 0;

		visitor::ExpressionVisitor exprVisitor;


		in[in_counter++] = caller;

		for (shptr<ast::Expression> argumentExpr : * (arguments->getArgumentExpressions()))
		{
			argumentExpr->accept(exprVisitor);
			in[in_counter++] = exprVisitor.getResultNode();
		}

		// create the call
		ir_node* store = get_store();
		ir_node* callee = new_Address(method_ent);

		ir_node* call_node = new_Call(store, callee, argc, in, get_entity_type(method_ent));
		free(in);

		// update the current store
		ir_node* new_store = new_Proj(call_node, get_modeM(), pn_Call_M);
		set_store(new_store);

		// get the result
		bool hasReturnType = !methodDeclaration->getReturnType()->isVoid();

		if (hasReturnType)
		{
			ir_type* resultType = getType(methodDeclaration->getReturnType());
			ir_mode* result_mode = getMode(methodDeclaration->getReturnType());
			ir_node* tuple = new_Proj(call_node, get_modeT(), pn_Call_T_result);
			ir_node* resultNode = new_Proj(tuple, result_mode, 0);
			return std::tuple<ir_node*, ir_type*>(resultNode, resultType);
		}

		return std::tuple<ir_node*, ir_type*>(NULL, NULL);
	}

	std::tuple<ir_node*, ir_type*> FirmInterface::createNodeForMethodCall(shptr<ast::pe::MethodInvocation const> expr)
	{
		int this_pos = 0;
		ir_node* caller = get_value(this_pos, mode_P);
		auto arguments = expr->getArguments();

		return createNodeForMethodCall(caller, arguments, expr->getDeclaration());
	}

	std::tuple<ir_node*, ir_type*> FirmInterface::createNodeForMethodCall(ir_node* caller, shptr<ast::po::MethodInvocation const> expr)
	{
		auto arguments = expr->getArguments();
		return createNodeForMethodCall(caller, arguments, expr->getDeclaration());
	}

	ir_node* FirmInterface::createNodeForCallocCall(ir_node* count, unsigned int size)
	{
		static ir_type* proc_calloc = NULL;
		static ir_entity* callocMethodEntity = NULL;

		if (!proc_calloc)
		{
			// calloc
			proc_calloc = new_type_method(2, 1);
			set_method_param_type(proc_calloc, 0, new_type_primitive(get_reference_mode_unsigned_eq(mode_P)));
			set_method_param_type(proc_calloc, 1, new_type_primitive(get_reference_mode_unsigned_eq(mode_P)));
			set_method_res_type(proc_calloc, 0, new_type_primitive(mode_P));
			ir_type* globalOwner = get_glob_type();
			callocMethodEntity = new_entity(globalOwner, new_id_from_str("calloc"), proc_calloc);
		}

		// call calloc
		ir_node* args[] =
		{
			new_Conv(count, get_reference_mode_unsigned_eq(mode_P)),
			new_Conv(FirmInterface::getInstance().createNodeForIntegerConstant(size), get_reference_mode_unsigned_eq(mode_P))
		};
		//ir_graph* irg = get_current_ir_graph();
		ir_node* store = get_store();
		ir_node* callee = new_Address(callocMethodEntity);
		ir_node* call_node = new_Call(store, callee, 2, args, proc_calloc);

		// update the current store
		ir_node* new_store = new_Proj(call_node, get_modeM(), pn_Call_M);
		set_store(new_store);

		// get the result
		ir_node* tuple = new_Proj(call_node, get_modeT(), pn_Call_T_result);
		ir_node* result = new_Proj(tuple, getReferenceMode(), 0);
		return result;
	}

	ir_node* FirmInterface::createNodeForIntegerConstant(int64_t x)
	{
		return new_Const_long(getIntegerMode(), x);
	}

	ir_node* FirmInterface::createNodeForBooleanConstant(bool x)
	{
		return x ? new_Const_long(getBooleanMode(), true) : new_Const_long(getBooleanMode(), false);
	}

	ir_node* FirmInterface::createModOperation(ir_node* left, ir_node* right)
	{
		ir_node* memory = get_store();
		ir_node* res = new_Mod(memory, left, right, getIntegerMode(), op_pin_state_pinned);
		set_store(new_Proj(res, mode_M, pn_Mod_M));
		return new_Proj(res, getIntegerMode(), pn_Mod_res);
	}

	ir_node* FirmInterface::createDivOperation(ir_node* left, ir_node* right)
	{
		ir_node* memory = get_store();
		ir_node* res = new_DivRL(memory, left, right, getIntegerMode(), op_pin_state_pinned); //op_pin_state_floats??
		set_store(new_Proj(res, mode_M, pn_Div_M));
		return new_Proj(res, getIntegerMode(), pn_Div_res);
	}

	ir_node* FirmInterface::createNullPointerNode()
	{
		return new_Const_long(mode_P, 0);
	}

	ir_op* FirmInterface::getLeaOp() const
	{
		return lea;
	}

	FirmInterface::~FirmInterface()
	{
		ir_finish();
		std::cout << "Destroyed FirmInterface instance" << std::endl;
	}

	ir_mode* FirmInterface::getIntegerMode()
	{
		return mode_Is;
	}

	ir_mode* FirmInterface::getBooleanMode()
	{
		return mode_Bu; /* TODO*/
	}

	ir_mode* FirmInterface::getReferenceMode()
	{
		return mode_P;
	}

	//probably to do: getMode(type) -> mode
	ir_mode* FirmInterface::getMode(shptr<ast::Type> ast_type)
	{
		if (ast_type->isAnyInteger())
			return getIntegerMode();
		else if (ast_type->isBool())
			return getBooleanMode();
		else
			return getReferenceMode();

	}

	ir_type* FirmInterface::getType(shptr<ast::Type> ast_type)
	{
		auto it = types.find(ast_type);

		if (it != types.end())
			return it->second;

		ir_type* r;

		if (ast_type->isBool() || ast_type->isInteger())
		{
			// add new primitive
			r = new_type_primitive(getMode(ast_type));

		}
		else if (ast_type->isArray())
		{
			// add new array + recursion for lesser dimensions
			r = new_type_pointer(getType(ast_type->de_array()));
		}
		else
		{
			ir_type* int_type = getType(std::make_shared<ast::Type>(ast::Type::Primitive_type::INT));
			r = new_type_pointer(int_type);
		}

		types[ast_type] = r;
		return r;
	}

	void FirmInterface::addClassType(shptr<ast::Ident> class_ident, ir_type* class_type)
	{
		auto ast_type = std::make_shared<ast::Type>(class_ident);
		auto it = types.find(ast_type);

		if (it != types.end())
			set_pointer_points_to_type(it->second, class_type);
		else
			types[ast_type] = new_type_pointer(class_type);
	}

	void FirmInterface::addMethod(ir_type* class_type, std::string method_name, ir_entity* ent)
	{
		classMethodEntities[ {class_type, method_name}] = ent;
	}

	ir_entity* FirmInterface::getMethodEntity(ir_type* class_type, std::string method_name)
	{
		return classMethodEntities[ {class_type, method_name}];
	}

	void FirmInterface::addField(ir_type* class_type, std::string method_name, ir_entity* ent)
	{
		classFieldEntities[ {class_type, method_name}] = ent;
	}

	ir_entity* FirmInterface::getFieldEntity(ir_type* class_type, std::string field_name)
	{
		return classFieldEntities[ {class_type, field_name}];
	}

	std::string FirmInterface::replace_dollar(std::string name)
	{
		std::string s = name;

		for (size_t i = 0; i < s.size(); ++i)
		{
			if (s[i] == '$')
				s = s.substr(0, i) + "_C" + s.substr(i + 1, s.size() - i - 1);
		}

		return s;
	}

	shptr<std::map<std::string, int>> FirmInterface::getVarMap()
	{
		return var2pos;
	}

	void FirmInterface::setVarMap(shptr<std::map<std::string, int>> newVar2pos)
	{
		var2pos = newVar2pos;
	}

	std::queue<ir_node*> FirmInterface::getWorklist(ir_graph* irg)
	{
		typedef void (*ir_func)(ir_node*, void*);
		//auto pWorklist = new std::queue<ir_node*>();
		std::queue<ir_node*> pWorklist;
		ir_func addToWorklist = [](ir_node * node, void* env)
		{
			auto pWorklist = (std::queue<ir_node*>*)env;
			set_irn_link(node, (void*)tarval_unknown);
			pWorklist->push(node);
		};

		// post ordering
		assure_irg_properties(irg, IR_GRAPH_PROPERTY_CONSISTENT_DOMINANCE);
		irg_walk_blkwise_dom_top_down(irg, NULL, addToWorklist, (void*)&pWorklist);
		return std::move(pWorklist);
	}

	std::vector<std::pair<ir_node*, unsigned int> > FirmInterface::getOuts(ir_node const* n)
	{
		std::vector<std::pair<ir_node*, unsigned int>> outs;

		for (ir_edge_t const* oe = get_irn_out_edge_first(n); oe; oe = get_irn_out_edge_next(n, oe, EDGE_KIND_NORMAL))
			outs.emplace_back(get_edge_src_irn(oe), get_edge_src_pos(oe));

		return outs;
	}
}

ir_node* new_r_Lea(ir_node* block, ir_node* offset, ir_node* base, ir_node* x, ir_node* scale, ir_mode* mode)
{
	ir_node* in[4] = {offset, base, x, scale};
	return new_ir_node(NULL, get_irn_irg(block), block, firm::FirmInterface::getInstance().getLeaOp(), mode, x ? scale ? 4 : 3 : 2, in);
}

ir_node* new_Lea(ir_node* offset, ir_node* base, ir_node* x, ir_node* scale, ir_mode* mode)
{
	return new_r_Lea(get_cur_block(), offset, base, x, scale, mode);
}

int is_Lea(ir_node const* node)
{
	return get_irn_op(node) == firm::FirmInterface::getInstance().getLeaOp();
}
