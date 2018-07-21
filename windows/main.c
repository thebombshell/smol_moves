
#include "../log.h"
#include "../smol.h"

smol* g_smol;

int main() {
	
	SYS("entering application...");
	
	SYS("allocating smol context");
	
	g_smol = alloc_smol();
	
	SYS("entering smol context loop");
	
	smol_run(g_smol);
	
	SYS("freeing smol context");
	
	free_smol(g_smol);
	
	SYS("...exiting application");
	
	return 1;
}