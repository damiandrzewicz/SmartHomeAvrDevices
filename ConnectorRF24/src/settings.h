/*
 * settings.h
 *
 *  Created on: Aug 28, 2018
 *      Author: damian
 */

#ifndef SRC_SETTINGS_H_
#define SRC_SETTINGS_H_

/*
 * 0 - Terminal mode
 * 1 - Connector mode
 * 2 - Standalone device mode
 */
#define NRF_MODE 0

#define DEVICE_NAME "ConnectorRF24"

const char *const DeviceStaticSettings[] PROGMEM = {
		DEVICE_NAME
};


#endif /* SRC_SETTINGS_H_ */
