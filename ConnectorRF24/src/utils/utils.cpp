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


