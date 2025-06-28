//libreria lora
//#include "LoRaWan_APP.h"
#include "Arduino.h"
//libreria gps
#include <HardwareSerial.h>
#include <TinyGPS++.h>
//libreria acelerometro
#include <Wire.h>
#include <MPU9250_asukiaaa.h> // Librería para el MPU9250

//Obs: LoRa solo manda bytes, por lo que haremos una conversión entre los valores float de los sensores a bytes.


//Variables acelerometro (PIN gpio 21 para SDA y GPIO 22 para SCL)
MPU9250_asukiaaa mySensor;
float aX, aY, aZ, aSqrt;


//Variables Bateria
#define BatteryLevel 33  // Pin ADC del Heltec (GPIO33)

float lectura_adc;
float volt_adc;
float volt_bateria;
// Resistencias del divisor
const float R1 = 50000.0; // 50kΩ
const float R2 = 33000.0; // 33kΩ


//Variables GPS
HardwareSerial gpsSerial(2);
TinyGPSPlus gps;


/*
//Definimos funcion para convertir float a bytes
void floatToBytes(float val, uint8_t* bytes_array){
  union{
    float f;
    uint8_t b[4];
  } data;   //el float y arraybytes comparten la direccion de memoria
  data.f = val;
  memcpy(bytes_array, data.b, 4);
}
  //al hacer data.f = val;, se llenan automáticamente los 4 bytes de data.b con la representación binaria de ese float.
*/



void setup() {
  Serial.begin(115200);
  delay(2000);
  //Configura acelerometro
  Wire.begin(21,22); // Usa GPIO21 como SDA y GPIO22 como SCL
  mySensor.setWire(&Wire);
  mySensor.beginAccel();

  //configura pin bateria
  pinMode(BatteryLevel, INPUT);

  //Inicia el GPS
  gpsSerial.begin(9600, SERIAL_8N1, 16, 17);
  Serial.println("Esperando fix de GPS...");
  
  // Inicializa el hardware
  //Serial.println("Inicializando LoRaWAN con ABP...");
  //Mcu.begin(HELTEC_BOARD, SLOW_CLK_TPYE);

 

}

void loop() {

  //read Acelerometter
  mySensor.accelUpdate();
  aX = mySensor.accelX();
  aY = mySensor.accelY();
  aZ = mySensor.accelZ();
  aSqrt = mySensor.accelSqrt();

  //read Battery
  lectura_adc = analogRead(BatteryLevel);              // valor de 0 a 4095
  volt_adc = lectura_adc / 4095.0 * 3.3;               // voltaje en el pin
  volt_bateria = volt_adc * ((R1 + R2) / R2);          // voltaje real
  

  // read GPS

  while (gpsSerial.available()) {
    gps.encode(gpsSerial.read());
  }

  if (gps.location.isValid()) {
    Serial.print("¡FIX OBTENIDO! Latitud: ");
    Serial.print(gps.location.lat(), 6);
    Serial.print(" | Longitud: ");
    Serial.println(gps.location.lng(), 6);
  } else {
    Serial.println("Sin fix todavía...");
  }

    // Imprime los datos por serial
  Serial.print("Acelerómetro [g]: ");
  Serial.print("X = "); Serial.print(aX, 2);
  Serial.print(", Y = "); Serial.print(aY, 2);
  Serial.print(", Z = "); Serial.println(aZ, 2);
  Serial.print(", Aceleración neta = "); Serial.println(aSqrt, 2);
  Serial.print(volt_bateria, 3);




  delay(5000);




  //ENVIO DE DATOS

  

  /*
  //Definimos el array de bytes y lo llenamos con nuestra funcion floatToBytes
  uint8_t myData[24];  // Ejemplo: dato simple en el payload
  floatToBytes(gps.location.lat(),   myData + 0);
  floatToBytes(gps.location.lng(),   myData + 4);
  floatToBytes(aX,                   myData + 8);
  floatToBytes(aY,                   myData + 12);
  floatToBytes(aZ,                   myData + 16);
  floatToBytes(volt_bateria,        myData + 20);
  */

}
