/*
 * settings.h
 *
 *  Created on: Jun 5, 2018
 *      Author: damian
 */

#ifndef SETTINGS_H_
#define SETTINGS_H_

#define IS_MASTER 0


/*
 *Device Types
 *0 - Nrf Connector
 *1 - WindowDriver
 *2 - Light Driver
 */
#define DEVICE_TYPE 0

/*
 * MODE 0 - standalone slave
 * MODE 1 - master
 * MODE 2 - slave
 */
#define NRF_MODE 1

/*
 * MODE 1 - master
 * MODE 2 - slave
 */

#define UART_MODE 2

#if UART_MODE == 1
#define UART_INPUT_MSG_TYPE "stm"
#define UART_OUTPUT_MSG_TYPE "mts"
#elif UART_MODE == 2
#define UART_INPUT_MSG_TYPE "mts"
#define UART_OUTPUT_MSG_TYPE "stm"
#endif





#endif /* SETTINGS_H_ */
