#include "uid.h"
#include "commons.h"

uid newuid(void)
{
    return (uid)time(NULL);
}
