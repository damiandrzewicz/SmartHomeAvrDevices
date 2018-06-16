/*
 * settings.h
 *
 *  Created on: Jun 15, 2018
 *      Author: damian
 */

#ifndef SETTINGS_H_
#define SETTINGS_H_

#define DEVICE_ADDRESS "x0001"
#define BLIND_QUANTITY 2

/*
 *Device Types
 *0 - Nrf Connector
 *1 - WindowDriver
 *2 - Light Driver
 */
#define DEVICE_TYPE 1

/*
 * MODE 0 - standalone slave
 * MODE 1 - master
 * MODE 2 - slave
 */
#define MODE 1

#if MODE == 1
#define UART_INPUT_MSG_TYPE "stm"
#define UART_OUTPUT_MSG_TYPE "mts"
#elif MODE == 2
#define UART_INPUT_MSG_TYPE "mts"
#define UART_OUTPUT_MSG_TYPE "stm"
#endif




#endif /* SETTINGS_H_ */
