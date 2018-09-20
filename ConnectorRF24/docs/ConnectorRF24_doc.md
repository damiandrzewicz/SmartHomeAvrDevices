# ConnectorRF24 documentation

blabla..

## Overview



## Work modes

## Communication

### Exchange data between RF24l01 connectors.

![ConnectorRF24_communication_between](pics/ConnectorRF24_communication_between.png)

#### Data format

Data frame exchanged between two connectors has specific format. All parts of requests and responses are divided by "#" sign. 

Main data frame is following:

```json
#{operation}#{direction}#{data}#
```

Where:

- {operation} - name of operation which will be executed,
- {direction} - two possibilities: 
  - "req" - request, 
  - "res" - response,
- {data} - content of message.

Main response data frame is following:

```jaon
#{operation}#{direction}#{data}#
```

In case of error device sends response:

```
#ERR#{mainErrorNumber}#{minorErrorNumber}#
```



Here are following operations:

- PassDataByAir- data are sent in special format frame as request to another device. This request is used for pass data between devices connected to nRF connectors.

  ```json
  @PDBA@req@ThisIsExampleOfContent@
  ```

  If requests is processed without any errors then response is sent back:

  ```json
  @PDBA@res@ok@ThisIsExampleOfResponse@
  ```


### Exchange data between RF24l01 connector and external devices

#### UART

blabla



##### asdasd

##### asdasd



