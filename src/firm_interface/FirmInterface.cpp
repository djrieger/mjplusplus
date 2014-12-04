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

#ifdef __APPLE__
	std::string printFunAsm = "	.section	__TEXT,__text,regular,pure_instructions \n \
								.globl	_COut_Mprintln \n \
								.align	4, 0x90 \n \
							_COut_Mprintln:                         ## @COut_Mprintln \n \
								.cfi_startproc \n \
							## BB#0: \n \
								pushq	%rbp \n \
							Ltmp2: \n \
								.cfi_def_cfa_offset 16 \n \
							Ltmp3: \n \
								.cfi_offset %rbp, -16 \n \
								movq	%rsp, %rbp \n \
							Ltmp4: \n \
								.cfi_def_cfa_register %rbp \n \
								subq	$16, %rsp \n \
								leaq	L_.str(%rip), %rax \n \
								movl	%edi, -4(%rbp) \n \
								movl	-4(%rbp), %esi \n \
								movq	%rax, %rdi \n \
								movb	$0, %al \n \
								callq	_printf \n \
								movl	$0, %esi \n \
								movl	%eax, -8(%rbp)          ## 4-byte Spill \n \
								movl	%esi, %eax \n \
								addq	$16, %rsp \n \
								popq	%rbp \n \
								retq \n \
								.cfi_endproc \n \
							 \n \
								.section	__TEXT,__cstring,cstring_literals \n \
							L_.str:                                 ## @.str \n \
								.asciz	\"%d\\n\" \n \
							.subsections_via_symbols \n \
							";
#else
	std::string printFunAsm = "	.file	\"print.c\" \n \
								.section	.rodata \n \
							.LC0: \n \
								.string	\"%d\\n\" \n \
								.text \n \
								.globl	_COut_Mprintln \n \
								.type	_COut_Mprintln, @function \n \
							_COut_Mprintln: \n \
							.LFB0: \n \
								.cfi_startproc \n \
								pushq	%rbp \n \
								.cfi_def_cfa_offset 16 \n \
								.cfi_offset 6, -16 \n \
								movq	%rsp, %rbp \n \
								.cfi_def_cfa_register 6 \n \
								subq	$16, %rsp \n \
								movl	%edi, -4(%rbp) \n \
								movl	-4(%rbp), %eax \n \
								movl	%eax, %esi \n \
								movl	$.LC0, %edi \n \
								movl	$0, %eax \n \
								call	printf \n \
								movl	$0, %eax \n \
								leave \n \
								.cfi_def_cfa 7, 8 \n \
								ret \n \
								.cfi_endproc \n \
								";
#endif

	std::ofstream output(out_name, std::ios::app);
	output << printFunAsm;
	output.close();

#ifdef __APPLE__
	std::system(std::string("sed -i '' 's/.*\\.size.*$//' " + out_name).c_str());
	std::system(std::string("sed -i '' 's/.*\\.type.*$//' " + out_name).c_str());
#endif
}

ir_node* FirmInterface::createNodeForMethodCall(ir_node* caller,
        ir_type* class_type,
        std::string const& method_name,
        shptr<ast::Arguments const> arguments)
{

	ir_entity* method_ent = getMethodEntity(class_type, method_name);
	int argc = arguments->getArgumentsSize() + 1;

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
	ir_node* call_node = new_Call(store, callee, argc, in, get_entity_type(method_ent));

	// update the current store
	ir_node* new_store = new_Proj(call_node, get_modeM(), pn_Call_M);
	set_store(new_store);

	// get the result
	ir_node* tuple = new_Proj(call_node, get_modeT(), pn_Call_T_result);
	ir_node* result = new_Proj(tuple, getIntegerMode(), 0);

	free(in); // necessary?
	return result;
}

ir_node* FirmInterface::createNodeForMethodCall(shptr<ast::pe::MethodInvocation const> expr)
{
	int this_pos = 0;
	ir_node* caller = get_value(this_pos, mode_P);
	ir_type* class_type = get_irn_type_attr(caller);

	auto method_name = expr->getIdentifier();
	auto arguments = expr->getArguments();

	return createNodeForMethodCall(caller, class_type, method_name, arguments);
}

ir_node* FirmInterface::createNodeForMethodCall(shptr<ast::MethodInvocation const> expr)
{
	auto methodDecl = expr->getDeclaration();
	ir_node* caller = NULL; //TODO: get caller
	ir_type* class_type = NULL; //TODO: get caller class type

	auto method_name = expr->getIdentifier();
	auto arguments = expr->getArguments();

	return createNodeForMethodCall(caller, class_type, method_name, arguments);
}

ir_node* FirmInterface::createNodeForIntegerConstant(int64_t x)
{
	return new_Const_long(getIntegerMode(), x);
}

ir_node* FirmInterface::createNodeForBooleanConstant(bool x)
{
	return new_Const(x ? get_tarval_b_true() : get_tarval_b_false());
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

void FirmInterface::foo()
{
	const unsigned int paramsCount = 0;
	const unsigned int resultsCount = 0;
	const unsigned int localVarsCount = 0;

#ifndef __APPLE__
	std::string mainMethodName = "main";
#else
	std::string mainMethodName = "_main";
#endif

	// main
	ir_type* proc_main = new_type_method(paramsCount, resultsCount);
	ir_type* globalOwner = get_glob_type();      /* the class in which this method is defined */
	ir_entity* mainMethodEntity = new_entity(globalOwner, new_id_from_str(mainMethodName.c_str()), proc_main);
	ir_graph* irg = new_ir_graph(mainMethodEntity, localVarsCount);
	set_current_ir_graph(irg);

	// println
	ir_type* proc_print = new_type_method(1, 0);
	set_method_param_type(proc_print, 0, new_type_primitive(mode_Is));
	ir_entity* printMethodEntity = new_entity(globalOwner, new_id_from_str("_COut_Mprintln"), proc_print);
	//new_ir_graph(printMethodEntity, 1);

	// call println
	ir_node* arg = new_Const_long(mode_Is, 42);
	ir_node* store = get_irg_no_mem(irg); // get_store();
	ir_node* callee = new_Address(printMethodEntity);
	ir_node* call_node = new_Call(store, callee, 1, &arg, proc_print);

	// update the current store
	ir_node* new_store = new_Proj(call_node, get_modeM(), pn_Call_M);
	set_store(new_store);

	ir_node* currentMemState = get_store();
	ir_node* x = new_Return (currentMemState, 0, NULL);
	add_immBlock_pred (get_irg_end_block(irg), x);

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

ir_entity* FirmInterface::getFieldEntity(ir_type* class_type, std::string method_name)
{
	return classFieldEntities[ {class_type, method_name}];
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
