/*
 * io.cpp
 *
 *  Created on: 07.03.2017
 *      Author: Damian
 */

#include "io.h"

namespace IO
{
	namespace Dir
	{
		void Set(volatile uint8_t * ddr, uint8_t pin, IO::direction dir)
		{
			if(dir == IO::direction::in)
				*ddr &= ~(1 << pin);
			else
				*ddr |= (1 << pin);
		}
	}

	namespace Port
	{
		void Set(volatile uint8_t * port, uint8_t pin, IO::state st)
		{
			if(st == IO::state::low)
				*port &= ~(1 << pin);
			else
				*port |= (1 << pin);
		}
		void Set(volatile uint8_t * port, uint8_t pin, IO::operation op, bool isPinNeg)
		{
			//*port ^= (1 << pin);
			switch(op)
			{
			case IO::operation::OR:
				if(isPinNeg)
					*port |= ~(1 << pin);
				else
					*port |= (1 << pin);
				break;
			case IO::operation::AND:
				if(isPinNeg)
					*port &= ~(1 << pin);
				else
					*port &= (1 << pin);
				break;
			case IO::operation::XOR:
				if(isPinNeg)
					*port ^= ~(1 << pin);
				else
					*port ^= (1 << pin);
				break;
			default:
				break;
			}
		}
	}

	namespace Pin
	{
		bool isSet(volatile uint8_t * pinx, uint8_t pin)
		{
			return *pinx & (1 << pin) ? 1 : 0;
		}
	}

}


