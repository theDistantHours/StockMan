#include "uid.h"
#include "commons.h"

uid newuid(void)
{
    static uid last;
    uid res = time(NULL) * 1000 + clock() % 1000;
    if (res == last) {
        res++;
        last = res;
        return res;
    }
    else {
        last = res;
        return res;
    }
}
