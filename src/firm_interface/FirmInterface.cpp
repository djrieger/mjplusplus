#include <iostream>
#include <fstream>

#include <cstring>
#include <stdio.h>
#include <cstdlib>

#include "FirmInterface.hpp"

#include "../lexer/token.hpp"
#include "../ast/LVDStatement.hpp"
#include "../ast/Block.hpp"
#include "../visitors/ProgramVisitor.hpp"
#include "../visitors/MemberVisitor.hpp"
#include "../visitors/ExpressionVisitor.hpp"


FirmInterface::FirmInterface()
{
	ir_init();
	printf("Initialized libFirm Version: %d.%d\n", ir_get_version_major(), ir_get_version_minor());
	be_parse_arg("isa=amd64");
	ir_mode* modeP = new_reference_mode("P64", irma_twos_complement, 64, 64);
	set_modeP(modeP);
}

void FirmInterface::setInput(std::string const& in)
{
	in_name = in;
}

void FirmInterface::setOutput(std::string const& out)
{
	out_name = out;
}

void FirmInterface::convert(shptr<ast::Program> program)
{
	std::cout << "converting Program" << std::endl;
	//foo();

	ProgramVisitor v;

	try
	{
		program->accept(v);
		dump_all_ir_graphs("testsuffix");
	}
	catch (char const* e)
	{
		std::cerr << e << std::endl;
		throw;
	}

	build();

}

void FirmInterface::build()
{
	std::cout << "Building" << std::endl;
	lower_highlevel();

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

	fclose(o);

	std::string printFunAsm = "        .file   \"print.c\" \n\
			.section        .rodata.str1.1,\"aMS\",@progbits,1 \n\
	.LC0: \n\
			.string \"%d\\n\" \n\
			.section        .text.unlikely,\"ax\",@progbits \n\
	.LCOLDB1: \n\
			.text \n\
	.LHOTB1: \n\
			.p2align 4,,15 \n\
			.globl  _COut_Mprintln \n\
			.type   _COut_Mprintln, @function \n\
	_COut_Mprintln: \n\
	.LFB11: \n\
			.cfi_startproc \n\
			subq    $8, %rsp \n\
			.cfi_def_cfa_offset 16 \n\
			movl    $.LC0, %edi \n\
			xorl    %eax, %eax \n\
			call    printf \n\
			xorl    %eax, %eax \n\
			addq    $8, %rsp \n\
			.cfi_def_cfa_offset 8 \n\
			ret \n\
			.cfi_endproc \n\
	.LFE11: \n\
			.size   _COut_Mprintln, .-_COut_Mprintln \n\
			.section        .text.unlikely \n\
	.LCOLDE1: \n\
			.text \n\
	.LHOTE1: \n\
			.ident  \"GCC: (Debian 4.9.1-19) 4.9.1\" \n\
			.section        .note.GNU-stack,\"\",@progbits \n\
";

	std::ofstream output(out_name, std::ios::app);
	output << printFunAsm;
	output.close();
}

ir_entity* FirmInterface::createMethodEntity(ir_type* owner, shptr<ast::MethodDeclaration const> methodDeclaration)
{
	std::string mangledMethodName = methodDeclaration->mangle();

	unsigned int paramsCount = methodDeclaration->getParameters()->size();
	bool hasReturnType = !methodDeclaration->getReturnType()->isVoid();
	ir_type* methodType = new_type_method(paramsCount + 1, hasReturnType);

	std::cout << "- ParamsCount = " << (paramsCount + 1) << std::endl;

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

ir_node* FirmInterface::createNodeForMethodCall(ir_node* caller,
        shptr<ast::Arguments const> arguments,
        shptr<ast::MethodDeclaration const> methodDeclaration)
{
	auto classIdent = methodDeclaration->getDeclaration()->getIdent();
	auto classAstType = std::make_shared<ast::Type>(classIdent);
	ir_type* class_type = FirmInterface::getInstance().getType(classAstType);

	auto method_name = methodDeclaration->mangle();

	std::cout << "- method " << method_name << std::endl;
	ir_type* owner = get_pointer_points_to_type(class_type);
	ir_entity* method_ent = getMethodEntity(owner, method_name);
	std::cout << "- method_ent=" << method_ent << std::endl;

	int argc = arguments->getArgumentsSize() + 1;
	std::cout << "- argc=" << argc << std::endl;

	ir_node** in = (ir_node**) calloc(argc, sizeof(ir_node*));
	int in_counter = 0;

	ExpressionVisitor exprVisitor;


	in[in_counter++] = caller;

	for (shptr<ast::Expression> argumentExpr : * (arguments->getArgumentExpressions()))
	{
		argumentExpr->accept(exprVisitor);
		in[in_counter++] = exprVisitor.getResultNode();
	}

	// create the call
	ir_node* store = get_store();
	ir_node* callee = new_Address(method_ent);

	std::cout << "- caller=" << caller << std::endl;

	ir_node* call_node = new_Call(store, callee, argc, in, get_entity_type(method_ent));

	// update the current store
	ir_node* new_store = new_Proj(call_node, get_modeM(), pn_Call_M);
	set_store(new_store);

	// get the result
	ir_node* tuple = new_Proj(call_node, get_modeT(), pn_Call_T_result);
	ir_node* result = NULL;

	bool hasReturnType = !methodDeclaration->getReturnType()->isVoid();

	if (hasReturnType)
	{
		ir_mode* result_mode = getMode(methodDeclaration->getReturnType());
		result = new_Proj(tuple, result_mode, 0);
	}

	free(in);
	return result;
}

ir_node* FirmInterface::createNodeForMethodCall(shptr<ast::pe::MethodInvocation const> expr)
{
	int this_pos = 0;
	ir_node* caller = get_value(this_pos, mode_P);
	auto arguments = expr->getArguments();

	return createNodeForMethodCall(caller, arguments, expr->getDeclaration());
}

ir_node* FirmInterface::createNodeForMethodCall(ir_node* caller, shptr<ast::MethodInvocation const> expr)
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
		set_method_param_type(proc_calloc, 0, new_type_primitive(mode_Is));
		set_method_param_type(proc_calloc, 1, new_type_primitive(mode_Is));
		set_method_res_type(proc_calloc, 0, new_type_primitive(mode_P));
		ir_type* globalOwner = get_glob_type();
		callocMethodEntity = new_entity(globalOwner, new_id_from_str("calloc"), proc_calloc);
	}

	// call calloc
	ir_node* args[] = {count, FirmInterface::getInstance().createNodeForIntegerConstant(size)};
	//ir_graph* irg = get_current_ir_graph();
	ir_node* store = get_store(); // get_store();
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
	return new_Mod(memory, left, right, getIntegerMode(), op_pin_state_pinned);
	// maybe set_store()
}

ir_node* FirmInterface::createDivOperation(ir_node* left, ir_node* right)
{
	ir_node* memory = get_store();
	return new_DivRL(memory, left, right, getIntegerMode(), op_pin_state_pinned); //op_pin_state_floats??
	// maybe set_store()
}


ir_node* FirmInterface::createNullPointerNode()
{
	return new_Const_long(mode_P, 0);
}

void FirmInterface::foo()
{
	const unsigned int paramsCount = 0;
	const unsigned int resultsCount = 0;
	const unsigned int localVarsCount = 1;

	std::string mainMethodName = "main";

	// main
	ir_type* proc_main = new_type_method(paramsCount, resultsCount);
	ir_type* globalOwner = get_glob_type();      /* the class in which this method is defined */
	ir_entity* mainMethodEntity = new_entity(globalOwner, new_id_from_str(mainMethodName.c_str()), proc_main);
	ir_graph* irg = new_ir_graph(mainMethodEntity, localVarsCount);
	set_current_ir_graph(irg);

	set_cur_block(get_irg_start_block(irg));

	// println
	ir_type* proc_print = new_type_method(1, 0);
	set_method_param_type(proc_print, 0, new_type_primitive(mode_Is));
	ir_entity* printMethodEntity = new_entity(globalOwner, new_id_from_str("_COut_Mprintln"), proc_print);




	// if
	ir_node* mainAddrNode = new_Address(mainMethodEntity);
	ir_node* printAddrNode = new_Address(printMethodEntity);
	ir_node* cmpNode = new_Cmp(new_Const_long(mode_Is, 7), new_Const_long(mode_Is, 4), ir_relation::ir_relation_greater);
	ir_node* condNode = new_Cond(cmpNode);

	ir_node* projTrue = new_Proj(condNode, get_modeX(), pn_Cond_true);
	ir_node* projFalse = new_Proj(condNode, get_modeX(), pn_Cond_false);

	// then
	ir_node* trueBlock = new_immBlock();
	add_immBlock_pred(trueBlock, projTrue);
	mature_immBlock(trueBlock);
	set_cur_block(trueBlock);
	set_value(0, new_Const_long(mode_Is, 9));

	ir_node* arg = new_Const_long(mode_Is, 1010101010);
	ir_node* store = get_irg_no_mem(irg);
	ir_node* callee = new_Address(printMethodEntity);
	ir_node* call_node = new_Call(store, callee, 1, &arg, proc_print);

	ir_node* new_store = new_Proj(call_node, get_modeM(), pn_Call_M);
	set_store(new_store);

	ir_node* trueJmp = new_Jmp();

	// else
	ir_node* falseBlock = new_immBlock();
	add_immBlock_pred(falseBlock, projFalse);
	mature_immBlock(falseBlock);
	set_cur_block(falseBlock);
	set_value(0, new_Const_long(mode_Is, 3));

	arg = new_Const_long(mode_Is, 4); //get_value(0, mode_Is);
	store = get_irg_no_mem(irg); // get_store();
	callee = new_Address(printMethodEntity);
	call_node = new_Call(store, callee, 1, &arg, proc_print);

	new_store = new_Proj(call_node, get_modeM(), pn_Call_M);
	set_store(new_store);

	ir_node* falseJmp = new_Jmp();

	// after if
	ir_node* exitBlock = new_immBlock();
	add_immBlock_pred(exitBlock, trueJmp);
	add_immBlock_pred(exitBlock, falseJmp);
	mature_immBlock(exitBlock);
	set_cur_block(exitBlock);



	// finalize main
	ir_node* currentMemState = get_store();
	ir_node* x = new_Return (currentMemState, 0, NULL);
	add_immBlock_pred(get_irg_end_block(irg), x);

	irg_finalize_cons (irg);
	dump_ir_graph (irg, 0);

	std::cout << "Dumped ycomp graph" << std::endl;

	build();
}

FirmInterface::~FirmInterface()
{
	// TODO: free graph, ir_type nodes etc.
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
		r = new_type_array(getType(ast_type->de_array()));
	}
	else
	{
		std::cerr << "Trying to get firm type for ";
		ast_type->toString(std::cerr, 0);
		std::cerr << " which should exist already, returning int pointer." << std::endl;
		ir_type* int_type = getType(std::make_shared<ast::Type>(ast::Type::Primitive_type::INT));
		r = new_type_pointer(int_type);
	}

	types[ast_type] = r;
	return r;
}

void FirmInterface::addClassType(shptr<ast::Ident> class_ident, ir_type* class_type)
{
	auto ast_type = std::make_shared<ast::Type>(class_ident);
	std::cerr << "Adding type for " << get_class_name(class_type) << "." << std::endl;
	auto it = types.find(ast_type);

	if (it != types.end())
	{
		std::cerr << "\taka fixing the existing one" << std::endl;
		set_pointer_points_to_type(it->second, class_type);
	}
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

ir_entity* FirmInterface::getSystemNode()
{
	static ir_entity* system_ent = NULL;

	if (!system_ent)
	{
		lexer::Token sit {lexer::Token::Token_type::TOKEN_IDENT, lexer::Token::getTableReference("$System"), { -1, 0}};
		auto s = std::make_shared<ast::Type>(std::make_shared<ast::Ident>(sit));

		ir_type* system_type = getType(s);
		system_ent = new_entity(get_glob_type(), new_id_from_str("System"), system_type);
		set_entity_initializer(system_ent, get_initializer_null());
	}

	return system_ent;
}

void FirmInterface::initSystem()
{
	ir_entity* system_ent = getSystemNode();

	lexer::Token oit {lexer::Token::Token_type::TOKEN_IDENT, lexer::Token::getTableReference("$Out"), { -1, 0}};
	auto o = std::make_shared<ast::Type>(std::make_shared<ast::Ident>(oit));
	ir_type* out_type = getType(o);

	lexer::Token sit {lexer::Token::Token_type::TOKEN_IDENT, lexer::Token::getTableReference("$System"), { -1, 0}};
	auto s = std::make_shared<ast::Type>(std::make_shared<ast::Ident>(sit));
	ir_type* system_type = getType(s);

	ir_node* one = createNodeForIntegerConstant(1);
	ir_node* out_n = createNodeForCallocCall(one, get_type_size_bytes(out_type));
	ir_node* system_n = createNodeForCallocCall(one, get_type_size_bytes(system_type));
	ir_node* system_addr = new_Address(system_ent);

	set_store(new_Proj(new_Store(get_store(), system_addr, system_n, system_type, cons_none), mode_M, pn_Store_M));

	ir_entity* out_field = getFieldEntity(get_pointer_points_to_type(system_type), "_CSystem_Fout");

	ir_mode* addr_mode = get_reference_mode_unsigned_eq(mode_P);
	ir_node* offset_node = new_Const_long(addr_mode, get_entity_offset(out_field));
	ir_node* addr = new_Conv(new_Add(new_Conv(system_n, addr_mode), offset_node, addr_mode), mode_P);

	set_store(new_Proj(new_Store(get_store(), addr, out_n, out_type, cons_none), mode_M, pn_Store_M));
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
