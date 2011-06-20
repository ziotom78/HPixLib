#include <chealpix.h>
#include <stdio.h>

int
main(void)
{
    int result = 1;
    result = result && (chealpix_nside_to_npixel(1024) == 0);
    return result != 0;
}
