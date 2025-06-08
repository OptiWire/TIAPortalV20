#ifdef ESP8266
 #include <ESP8266WiFi.h>
#else //ESP32
 #include <WiFi.h>
#endif
#include "ModbusIP_ESP8266.h"
#include "ModbusRTU.h"

#define ID_ESCLAVO 1
#define DEBUG true  // Set to true for debugging output

// Create Modbus instances
ModbusRTU mb1;  // RTU instance for communication with VFD
ModbusIP mb2;   // TCP instance for remote control

// Variables for register operations
uint16_t resp = 0;
uint16_t resp2 = 0;
uint16_t resp3 = 0;
int direccion = 0;
int direccion2 = 0;
int valor = 0;
int comando = 0;  // 0: write, 1: read
int lectura = 0;
int lectura2 = 0;
int lectura3 = 0;

void setup() {
  Serial.begin(115200);
  
  // Connect to WiFi
  WiFi.begin("SSDI", "pass");
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
 
  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  
  // Initialize Modbus RTU serial communication
  Serial2.begin(19200, SERIAL_8N1, 16, 17);  // Using pins 16 (RX) and 17 (TX)
  mb1.begin(&Serial2, 5);  // Specify RE_DE control pin for RS485 direction control
  mb1.master();
  
  // Initialize Modbus TCP server
  mb2.server();
  // Register definitions for TCP interface
  mb2.addHreg(0);  // Register for target address (offset) // addrs 1
  mb2.addHreg(1);  // Register for value to write // addrs 2
  mb2.addHreg(2);  // Command register (0: write, 1: read) // value
  mb2.addHreg(3);  // First read value storage // command
  mb2.addHreg(4);  // Second read value storage // storage 1
  mb2.addHreg(5);  // Second address to read from // storage 2
  mb2.addHreg(6);  // Second address to read from // storage 3
}

void loop() {
  //mb2.readHreg(remote, TO_REG, &res);  // Initiate Read Coil from Modbus Slave);
    //mb1.writeHreg(remote, 1, &res);
  //mb1.task();
  direccion= mb2.Hreg(0); //Registro 0 es la dirección o offset del registro a escribir
  direccion2= mb2.Hreg(1);
  valor=mb2.Hreg(2);//1 es el valor a apuntar a ese registro
  comando=mb2.Hreg(3);//Lectura de comando para variador
  //mb1.Hreg(20,res);
  if (!mb1.slave()) {
    if(comando==0&&direccion<4){//Si dirección esta en el rango de 1 a 3 la operación de escritura y lectura es valida
      mb1.writeHreg(ID_ESCLAVO,direccion,valor);
    }else{
//      mb1.readHreg(ID_ESCLAVO,direccion,&resp,2); //Lectura de 1 holding register en la dirección o offset indicados
//      mb2.Hreg(3,resp);
//      mb2.Hreg(4,resp);
//      lectura=mb2.Hreg(3);
//      lectura2=mb2.Hreg(4);

        if (direccion==6){
          mb1.readHreg(ID_ESCLAVO,direccion,&resp,1); //Lectura de 1 holding register en la dirección o offset indicados
          mb2.Hreg(4,resp);
          lectura=mb2.Hreg(4);
          }
        if(direccion2>6){
          mb1.readHreg(ID_ESCLAVO,direccion2,&resp2,1); //Lectura de 1 holding register en la dirección o offset indicados
          mb2.Hreg(5,resp2);
          lectura2=mb2.Hreg(5);
          }
          if(comando==1&&direccion==0){
          mb1.readHreg(ID_ESCLAVO,direccion,&resp3,1); //Lectura de 1 holding register en la dirección o offset indicados
          mb2.Hreg(6,resp3);
          lectura3=mb2.Hreg(6);
          }
      }
   }
  mb1.task();
  mb2.task();
  delay(50);
  Serial.print("valor: ");
  Serial.print(valor);
  Serial.print(" comando: ");
  Serial.print(comando);
  Serial.print(" direccion: ");
  Serial.print(direccion);
  Serial.print(" direccion2: ");
  Serial.print(direccion2);
  Serial.print(" lectura: ");
  Serial.println(lectura);
  Serial.print(" lectura2: ");
  Serial.println(lectura2);
  Serial.print(" lectura3: ");
  Serial.println(lectura3);
}
