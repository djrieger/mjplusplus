#include "MemberVisitor.hpp"
#include "StatementVisitor.hpp"

MemberVisitor::MemberVisitor(ClassVisitor& classVisitor): classVisitor(classVisitor)
{
	setOwner(classVisitor.getOwner());
}

void MemberVisitor::visitMethodBodyAndFinalize(shptr<const ast::MethodDeclaration> methodDeclaration, ir_graph* irg)
{
	methodDeclaration->createVariablePositions();

	StatementVisitor stmtVisitor(*this);

	if (methodDeclaration->getBlock())
	{
		//methodDeclaration->getParameters();
		ir_node* start = get_irg_start(irg);
		ir_node* args = new_Proj(start, mode_T, pn_Start_T_args);
		ir_entity* m_ent = get_irg_entity(irg);
		ir_type* m_type = get_entity_type(m_ent);
		size_t param_count = get_method_n_params(m_type);

		for (size_t i = 0; i < param_count; i++)
		{
			ir_type* p_type = get_method_param_type(m_type, i);
			ir_node* arg = new_Proj(args, get_type_mode(p_type), i);
			set_value(i, arg);
		}

		methodDeclaration->getBlock()->accept(stmtVisitor);

		//TODO: only do this when void AND no explicit return.
		if (methodDeclaration->getReturnType()->isVoid()) // && hasNoReturn()
		{
			std::cout << "  void method (with no return;)" << std::endl;

			ir_node* currentMemState = get_store();
			ir_node* x = new_Return(currentMemState, 0, NULL);
			add_immBlock_pred(get_irg_end_block(irg), x);
		}
	}
	else
	{
		std::cout << "  Empty method body" << std::endl;

		ir_node* currentMemState = get_store();
		ir_node* x = new_Return(currentMemState, 0, NULL);
		add_immBlock_pred(get_irg_end_block(irg), x);
	}

	irg_finalize_cons(irg);
}

void MemberVisitor::visit(shptr<const ast::MethodDeclaration> methodDeclaration)
{
	std::cout << "  Visiting method " << methodDeclaration->mangle() << std::endl;

	std::string mangledMethodName = methodDeclaration->mangle();

	// ignore println as its implementation is generated by FirmInterface::build automatically
	if (mangledMethodName != "_Cout_Mprintln")
	{
		ir_entity* ent = FirmInterface::getInstance().getMethodEntity(owner, mangledMethodName);

		std::cout << "Revisiting MethodDeclaration: " << mangledMethodName << ", ent: " << ent << std::endl;

		//TODO: SimpleIf example includes parameters into local variable count
		//ATTENTION: methodDeclaration->countVariableDeclarations() + 1
		//			 since there is an assertion, that pos+1 < irg->n_loc
		//			 BE-FUCKING-CAUSE THEY COUNT THE PROJ AS PARAMETER!!! (maybe)
		function_graph = new_ir_graph(ent, methodDeclaration->countVariableDeclarations() + 1 + methodDeclaration->getParameters()->size());


		set_current_ir_graph(function_graph);

		visitMethodBodyAndFinalize(methodDeclaration, function_graph);
	}
}

void MemberVisitor::visit(shptr<const ast::MainMethodDeclaration> mainMethodDecl)
{

#ifndef __APPLE__
	std::string mainMethodName = "main";
#else
	std::string mainMethodName = "_main";
#endif

	std::cout << "  Visiting main method " << mainMethodName << std::endl;

	ir_type* proc_main = new_type_method(0, 0);
	ir_type* globalOwner = get_glob_type();
	ir_entity* mainMethodEntity = new_entity(globalOwner, new_id_from_str(mainMethodName.c_str()), proc_main);
	ir_graph* irg = new_ir_graph(mainMethodEntity, mainMethodDecl->countVariableDeclarations());

	set_current_ir_graph(irg);

	visitMethodBodyAndFinalize(mainMethodDecl, irg);
}

void MemberVisitor::visit(shptr<const ast::FieldDeclaration> fieldDeclaration)
{
	;
}

ir_graph* MemberVisitor::getGraph() const
{
	return function_graph;
}
