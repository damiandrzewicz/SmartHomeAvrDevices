/*
 * io.h
 *
 *  Created on: 07.03.2017
 *      Author: Damian
 */

#ifndef IO_H_
#define IO_H_

#include <avr/io.h>

namespace IO
{

	struct PinData
	{
		volatile uint8_t *ddr;
		volatile uint8_t *port;
		volatile uint8_t *pinX;
		uint8_t pin;
	};
/**
	 * \brief Enumerate fields of pin's direction.
	 *
	 * This macros are using to set direction of passed port.
	 */
	enum class direction
	{
		in,		/*!< Set pin as ingoing. */
		out		/*!< Set pin as outgoing. */
	};

	/**
	 * \brief Enumerate fields of pin's state.
	 *
	 * This macros are using to set pin states as pull-up and pull-down.
	 */
	enum class state
	{
		low,	/*!< Set pin's state as pulled-down. */
		high,	/*!< Set pin's state as pulled-up. */
	};

	/**
	 * \brief Enumerate fields of logical operations.
	 *
	 * This macros can be use for define logical operation executed on passed pin.
	 */
	enum class operation
	{
		OR,	/*!< Logical OR operation. */
		AND,	/*!< Logical AND operation */
		XOR	/*!< Logical XOR operation*/
	};


	namespace Dir
	{
		void Set(volatile uint8_t * ddr, uint8_t pin, IO::direction dir);
	}

	namespace Port
	{
		void Set(volatile uint8_t * port, uint8_t pin, IO::state st);
		void Set(volatile uint8_t * port, uint8_t pin, IO::operation op, bool isPinNeg = false);
	}

	namespace Pin
	{
		bool isSet(volatile uint8_t * port, uint8_t pin);
	}

}



#endif /* IO_H_ */
