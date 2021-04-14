#include "commons.h"

static time_t virtual_time;
static time_t actual_time;
static tm *time_desc;

void initTiming() {
    time(&virtual_time);
    time_desc = gmtime(&virtual_time);
    srand((unsigned)(time(NULL)));
}
void setVirtualTime(time_t t){
    virtual_time = t;
}
time_t randomtime(time_t start, time_t end) {
    unsigned long long int  a = rand();
    unsigned long long int b = rand();
    unsigned long long int c = (a << 32) | b;
    c %= (end - start);
    return (time_t)c;
}

time_t getTime(bool isEmulated) {
    if (isEmulated) {
        return virtual_time;
    }
    else {
        time(&actual_time);
        return actual_time;
    }
}
