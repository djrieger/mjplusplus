#include "FirmInterface.hpp"
#include <cstring>

FirmInterface::FirmInterface()
{
	ir_init();
	printf("Initialized libFirm Version: %d.%d\n", ir_get_version_major(), ir_get_version_minor());
}

void FirmInterface::foo()
{
	const unsigned int paramsCount = 0;
	const unsigned int resultsCount = 0;
	const unsigned int localVarsCount = 0;
	
	ir_type* proc_main = new_type_method(paramsCount, resultsCount);
	ir_type* owner = get_glob_type();      /* the class in which this method is defined */
	ir_entity* ent = new_entity(owner, new_id_from_str("EMPTY_main"), proc_main);
	ir_graph* irg = new_ir_graph(ent, localVarsCount);
	set_current_ir_graph(irg);

	ir_node* currentMemState = get_store();
	ir_node* x = new_Return (currentMemState, 0, NULL);
	add_immBlock_pred (get_irg_end_block(irg), x);
	mature_immBlock (get_irg_end_block(irg));
	// Commented out missing function irg_vrfy(irg);
	irg_finalize_cons (irg);
	dump_ir_graph (irg, 0);

	std::cout << "Dumped ycomp graph" << std::endl;
}

FirmInterface::~FirmInterface()
{
	ir_finish();
}