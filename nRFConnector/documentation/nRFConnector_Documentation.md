# nRF Connector Documentation

### Overview

​	nRF connector is a portable circuit board (PCB) based on ATmega8 microcontroller. The main advantage and reason for using this connector is a unified way to communicate with nRF24l01 wireless transmitter/receiver. Data exchange between connector and external devices is based on RS232 (UART) interface. That makes nRF24l01 wireless module easy to usage and adapt for further requirements.  

​	Radio transmitter is connected to the ATMega8 microcontroller by SPI interface. Processor is clocked at 8MHz frequency by internal oscillator. Connector board is prepared for voltage measuring by embedded ADC. 

### Communication interface

All slave devices has theirs own addresses. That means master connector must change it's address to this same which slave device has. Following picture shows example of communication:

![changeAddress](/home/damian/Documents/SmartHomeSystem/workspace/SmartHomeAvrDevicesWorkspace/nRFConnector/documentation/images/changeAddress.png)

1. To send data to device with address *x0001* nRF Connector must change it's address to *x0001*,
2. To send data to device with address *x0002* nRF Connector must change it's address to *x0002*,
3. To send data to device with address *x0003* nRF Connector must change it's address to *x0003*,

Following request should be send to the nRF Connector:

- **set received address**:

  ```
  @request@setReceiverAddress@[addressValue]@\r
  ```

  where: ***addressValue*** is device address to which connection should be opened.

- **send data**:

  ```
  @request@sendData@[message]@\r
  ```

  where: ***message*** is data to send.

All messages should be "\r" terminated to indicate end of the string. This is necessary because UART protocol can divide whole string to smart packages. The figure below describes message flow between connectors and theirs related devices. It is necessary to mention that connector can be master device by itself.

### nRF Connector implementation

​	During receiving data by UART interface particular callback function is called. Deceived data are parsed, verified and the buffer is set with received string. Particular flag is triggered for indicate to process received data in event loop. After sent data to the device then connector is waiting for the response. If timeout occurred then error message is sent to master device also by UART protocol. Received data 

Following sequence diagram shows process of UART callback:

![uartCallback](/home/damian/Documents/SmartHomeSystem/workspace/SmartHomeAvrDevicesWorkspace/nRFConnector/documentation/images/uartCallback.png)

​	When request is processed then flag *requestInBufferReady* is set to true. Then event loop can process received data. Following sequence diagram shows messages execution for setting transmitter address or sending data to another receiver:

![eventLoop1](/home/damian/Documents/SmartHomeSystem/workspace/SmartHomeAvrDevicesWorkspace/nRFConnector/documentation/images/eventLoop1.png)

​	During processing UART callback flag *requestinBufferReady* is set to true if all data has been processed without any errors. This trigger event loop for processing received data. Aboves diagram describes data process for this part of event. 

​	First part of this event is checking if received data should be processed as setting connector address or sending data to another receiver. In first case method *processSetReceiverAddress* is called and flag *readyForProcessResponse* is selected. In second case operation for sending data is executed. Flag *waitingForRadioResponse* is triggered and timer is started. In case of timeout error message is sent to master device. Value for timeout cam be changed in settings section of nRF Connector.