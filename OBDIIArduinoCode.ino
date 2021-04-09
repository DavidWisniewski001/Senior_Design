#include <SPI.h>

#define CAN_2515
// #define CAN_2518FD

// For Arduino MCP2515 Hat:
// the cs pin of the version after v1.1 is default to D9
// v0.9b and v1.0 is default D10
const int SPI_CS_PIN = 9;
const int CAN_INT_PIN = 2;

#ifdef CAN_2518FD
#include "mcp2518fd_can.h"
mcp2518fd CAN(SPI_CS_PIN); // Set CS pin
#endif

#ifdef CAN_2515
#include "mcp2515_can.h"
mcp2515_can CAN(SPI_CS_PIN); // Set CS pin
#endif
#define CAN_ID_PID          0x7DF

unsigned char PID_INPUT;
unsigned char getPid    = 0;

/* This is going to be where we store what data we are looking at for calculations and sending data
 *  1 = Engine RPM
 *  2 = Vehicle Speed
 */
unsigned short DataPoint = 0;

void sendPid(unsigned char __pid) {
    unsigned char tmp[8] = {0x02, 0x01, __pid, 0, 0, 0, 0, 0};
    SERIAL_PORT_MONITOR.print("SEND PID: 0x");
    SERIAL_PORT_MONITOR.println(__pid, HEX);
    CAN.sendMsgBuf(CAN_ID_PID, 0, 8, tmp);

    switch(__pid){
        //C -> 12 this is going ot be for engine RPM
        case 12:
            DataPoint=1;
            break;
            
        case 13:
             DataPoint=2;
             break;

        default:
            break;
    }
}

void setup() {
    SERIAL_PORT_MONITOR.begin(115200);
    while(!Serial){};

    while (CAN_OK != CAN.begin(CAN_500KBPS)) {             // init can bus : baudrate = 500k
        SERIAL_PORT_MONITOR.println("CAN init fail, retry...");
        delay(50);
    }
    SERIAL_PORT_MONITOR.println("CAN init ok!");
}


void loop() {
    delay(3000);

    //Vehicle Speed in kmph
    sendPid(13);
    delay(10);
    taskCanRecv();
    //Engine Speed in rpm (256A + B) / 4
    sendPid(12);
    delay(10);    
    taskCanRecv();
        
}

void taskCanRecv() {
    unsigned char len = 0;
    unsigned char buf[8];

    if (CAN_MSGAVAIL == CAN.checkReceive()) {                // check if get data
        CAN.readMsgBuf(&len, buf);    // read data,  len: data length, buf: data buf
        for (int i = 0; i < len; i++) { // print the data
            SERIAL_PORT_MONITOR.print(buf[i], HEX);
            SERIAL_PORT_MONITOR.print("\t");
            
        }
        SERIAL_PORT_MONITOR.println();

        switch(DataPoint){
          case 1:
            break;
        }
    }
}
 //END FILE
