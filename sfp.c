#include "sfp.h"
#include "util.h"
#include "sfp_opt.h"

FILE *logfp = NULL;
struct prog_opt sfp_opt;

int main(int argc, char* const argv[])
{
	int rc;
	rc = get_opt(argc, argv);
	if (rc) {
		return rc;
	}
	return 0;
}
