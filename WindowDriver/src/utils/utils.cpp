/*
 * utils.cpp
 *
 *  Created on: Aug 15, 2018
 *      Author: damian
 */

#include "utils.h"

static volatile unsigned long milliseconds;

unsigned long millis()
{
    return milliseconds;
}

void increaseMillis()
{
	milliseconds++;
}

char* lltoa(long long val, int base){

    static char buf[64] = {0};

    int i = 62;
    int sign = (val < 0);
    if(sign) val = -val;

    if(val == 0) return "0";

    for(; val && i ; --i, val /= base) {
        buf[i] = "0123456789abcdef"[val % base];
    }

    if(sign) {
        buf[i--] = '-';
    }
    return &buf[i+1];

}


