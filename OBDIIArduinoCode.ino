#include <SPI.h>
#include <SoftwareSerial.h>

#define CAN_2515

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
 *  1 = Engine RPM (256A+B)/4 
 *  2 = Vehicle Speed A
 *  3 = Engine Coolant Temp A - 40
 *  4 = Engine Oil Temperature A - 40
 *  5 = Ambient Air Temperature A - 40
 *  6 = Fuel Level (100/255)A
 *  7 = Check Engine Light
 */
unsigned short DataPoint = 0;

//setting up hc05 15 rx 14 tx
SoftwareSerial hc06(15,14);

void sendPid(unsigned char __pid) {
    unsigned char tmp[8] = {0x02, 0x01, __pid, 0, 0, 0, 0, 0};
    CAN.sendMsgBuf(CAN_ID_PID, 0, 8, tmp);

    switch(__pid){
        //1 This is what we have to take apart to come up with if the check engine light is on or off
        case 1:
            DataPoint=7;
             break;
             
        //5 this is going to be the data point for Engine Coolant temp
        case 5:
            DataPoint=3;
            break;
            
        //C -> 12 this is going to be for engine RPM
        case 12:
            DataPoint=1;
            break;
            
        //D -> 13 This is going to be for vehicle speed    
        case 13:
             DataPoint=2;
             break;

        //2F -> 47 Fuel Tank Level Input
        case 47:
            DataPoint=6;
            break;

        //46 -> 70 This is the outside air temp
        case 70:
             DataPoint =5;
             break;
             
        //5C -> 92 Engine Oil Temperature     
        case 92:
             DataPoint=4;
             break;

        default:
            break;
    }
}

void setup() {
    SERIAL_PORT_MONITOR.begin(9600);

    //begin the bluetooth connection
    hc06.begin(9600);
    
    while(!Serial){};

    while (CAN_OK != CAN.begin(CAN_500KBPS)) {             // init can bus : baudrate = 500k
        SERIAL_PORT_MONITOR.println("CAN init fail, retry...");
        delay(50);
    }
    SERIAL_PORT_MONITOR.println("CAN init ok!");
}


void loop() {
    delay(100);

    //Vehicle Speed in kmph
    sendPid(13);
    delay(10);
    taskCanRecv();
    //Engine Speed in rpm (256A + B) / 4
    sendPid(12);
    delay(10);    
    taskCanRecv();
    // Engine Coolant Temp
    sendPid(5);
    delay(10);
    taskCanRecv();
    // Engine Oil Temp
    sendPid(92);
    delay(10);
    taskCanRecv();
    //AmbientAirTemp
    sendPid(70);
    delay(10);
    taskCanRecv();
    //Fuel Tank Level Input
    sendPid(47);
    delay(10);
    taskCanRecv();
    //Checking For Check Engine light 
    sendPid(1);
    delay(10);
    taskCanRecv(); 

    SERIAL_PORT_MONITOR.println("-");
    
}

void taskCanRecv() {
    unsigned int EngineRPM = 0;
    unsigned int VehicleSpeed = 0;
    unsigned int CoolantTemp = 0;
    unsigned int OilTemp = 0;
    unsigned int AmbientTemp = 0;
    unsigned short CheckEngineLight = 0;
    
    //Need a float for 100/255
    float calc1 = 100.0/255.0;
    float FuelLevel = 0;
    
    unsigned char len = 0;
    unsigned char buf[8];

    if (CAN_MSGAVAIL == CAN.checkReceive()) {                // check if get data
        CAN.readMsgBuf(&len, buf);    // read data,  len: data length, buf: data buf
        /*
        for (int i = 2; i < len - 3; i++) { // print the data
            SERIAL_PORT_MONITOR.print(buf[i], HEX);
            SERIAL_PORT_MONITOR.print("\t");
            
        }
        SERIAL_PORT_MONITOR.println();
        */
        
        switch(DataPoint){
          //buf[3] = data point A, we will mostly only need A and B
          //Math behind the RPM calculation
          case 1:
            EngineRPM = (256*buf[3]+buf[4])/4;
            SERIAL_PORT_MONITOR.print(EngineRPM);
            SERIAL_PORT_MONITOR.print(" ");
            //hc06.write(EngineRPM);
            DataPoint = 0;
            break;
            
          case 2:
            VehicleSpeed = (0.6213711*buf[3]);
            SERIAL_PORT_MONITOR.print(VehicleSpeed);
            SERIAL_PORT_MONITOR.print(" ");
            //hc06.write(VehicleSpeed);
            DataPoint = 0;
            break;

          case 3:
            CoolantTemp = ((buf[3] - 40)*(9/5) + 32);
            SERIAL_PORT_MONITOR.print(CoolantTemp);
            SERIAL_PORT_MONITOR.print(" ");
            //hc06.write(CoolantTemp);
            DataPoint = 0;
            break;

          case 4:
            OilTemp = ((buf[3] - 40)*(9/5) + 32);
            SERIAL_PORT_MONITOR.print(OilTemp);
            SERIAL_PORT_MONITOR.print(" ");
            //hc06.write(OilTemp);
            DataPoint = 0;
            break;

         case 5:
            AmbientTemp = ((buf[3] - 40)*(9/5) + 32);
            SERIAL_PORT_MONITOR.print(AmbientTemp);
            SERIAL_PORT_MONITOR.print(" ");
            //hc06.write(AmbientTemp);
            DataPoint = 0;
            break;

         case 6:
            FuelLevel =((calc1)*buf[3]);
            SERIAL_PORT_MONITOR.print(FuelLevel);
            SERIAL_PORT_MONITOR.print(" ");
            //hc06.write(FuelLevel);
            DataPoint = 0; 
            break;

         case 7:
            if(buf[3] > 127){
            SERIAL_PORT_MONITOR.print("1 ");
            CheckEngineLight = 1;
            }else
            SERIAL_PORT_MONITOR.print("0 ");
            CheckEngineLight = 0;

            //hc06.write(CheckEngineLight);
            break;
                                   
          default:
            break;
        }
    }
}
 //END FILE
