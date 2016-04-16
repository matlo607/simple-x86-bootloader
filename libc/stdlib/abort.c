#include <sys/system.h>

void abort(void)
{
    panic("system abort execution");
}
