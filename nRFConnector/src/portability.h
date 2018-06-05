/*
 * portability.h
 *
 *  Created on: 06.03.2017
 *      Author: Damian
 */

#ifndef PORTABILITY_H_
#define PORTABILITY_H_

__extension__ typedef int __guard __attribute__((mode (__DI__)));

void operator delete( void* pf )
{
}
extern "C" void __cxa_pure_virtual();
extern "C" void _pure_virtual(void);
extern "C" void __pure_virtual(void);
extern "C" int atexit( void (*func)(void));
extern "C" int __cxa_atexit();
extern "C" int __cxa_guard_acquire(__guard *);
extern "C" void __cxa_guard_release (__guard *);
extern "C" void __cxa_guard_abort (__guard *);


void __cxa_pure_virtual() {
}

void _pure_virtual() {
}

void __pure_virtual() {
}

int atexit( void (*func)(void)) {return -1;}
int __cxa_atexit() {return -1;}
int __cxa_guard_acquire(__guard *g) {return !*(char *)(g);};
void __cxa_guard_release (__guard *g) {*(char *)g = 1;};
void __cxa_guard_abort (__guard *) {};



#endif /* PORTABILITY_H_ */
