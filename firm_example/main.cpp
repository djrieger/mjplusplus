#include <libfirm/firm.h>
#include <libfirm/irdump.h>
#include <libfirm/irverify.h>

int main()
{
	ir_init();
	printf("Initialized libFirm Version: %d.%d\n",
	       ir_get_version_major(),
	       ir_get_version_minor());
	ir_finish();
	return 0;
}
