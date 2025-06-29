#include <ThingSpeak.h>


//Lib wifi
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>  //VEREMOS SI SE USA

#include "Arduino.h"
//libreria gps
#include <HardwareSerial.h>
#include <TinyGPS++.h>
//libreria acelerometro
#include <Wire.h>
#include <MPU9250_asukiaaa.h> // Librería para el MPU9250

//Credenciales wifi
const char* ssid = "VTR-6216549";
const char* password = "n4rdVvqptnkf";
//Credenciales Thingspeak
//const char* server = "http://api.thingspeak.com/channels/2998250/bulk_update.json"; //OPCION 1  Se usa con formato de JSON PARA BULK UPDATE
const char* server = "http://api.thingspeak.com/update";                            //OPCION 2  Usa formato simple de parametros get
const char* apiKey = "X52LATNR1YTYZBJD";  // << TU Write API Key


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



//Funciones que llamaremos
//void setEspPins();
void startWifi();
void sendSensorData();


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
  
  // Inicializamos el WiFi
  //setEspPins();
  startWifi();
  Serial.println();

}

//Funcion que inicializa el WiFi
void startWifi() { 
  WiFi.mode(WIFI_STA);               
  WiFi.begin(ssid, password);        
  Serial.print("Connecting to ");
  Serial.print(ssid); Serial.println(" ...");
  
  int i = 0;
  while (WiFi.status() != WL_CONNECTED) {   
    delay(1000);
    //Serial.print(++i); Serial.print(' ');
    Serial.println(WiFi.status());
  }

  Serial.println('\n');
  Serial.println("Connection established!");  
  Serial.print("Connected to ");
  Serial.println(WiFi.SSID());              
  Serial.print("IP address:\t");
  Serial.println(WiFi.localIP());           
}




 /*OPCION 1
void sendSensorData(){
  HTTPClient http;
  Serial.println(String("URL generada: ") + server);
  http.begin(server);
  http.addHeader("Content-Type", "application/json");


 

  //Armamos el cuerpo del json
  String json = "{";
    json += "\"write_api_key\":\"" + String(apiKey) + "\",";
    json += "\"updates\":[";
    json += "{";
    //json += "\"created_at\":\"" + timestamp + "\",";              //No la estamos incluyendo por ahora
    json += "\"field1\":" + String(aY, 2) + ",";  //(le saco el ", decimal") del parentesis pq se lo corto antes
    json += "\"field2\":" + String(aZ, 2) + ",";
    json += "\"field3\":" + String(aX, 2) + ",";
    
    json += "\"field4\":" + String(aSqrt, 2) + ",";
    json += "\"field5\":" + String(gps.location.lat(), 5) + ",";
    json += "\"field6\":" + String(gps.location.lng(), 5) + ",";
    json += "\"field7\":" + String(volt_bateria, 3) + ",";
    

    //json += "\"Status\":\"" + status + "\"";                      //Mensaje sobre estado del dispositivo ("OK, Alerta, "Sensor error", "Bateria baja, etc") (No lo incluimos, tampoco esta bien declarado)
    json += "}";    // fin del primer objeto de updates
    json += "]}";   // fin del arreglo de updates y del JSON

  //Enviamos mediante POST
    int httpCode = http.POST(json);
    String payload = http.getString();  //Guardamos la respuesta del servidor en la variable payload

    Serial.print("Código HTTP: ");
    Serial.println(httpCode);
    Serial.print("Respuesta del servidor: ");
    Serial.println(payload);

    http.end();
}
  */


  //OPCION 2:

  void sendSensorData(){
    HTTPClient http;
    
    // Usa HTTPS y formato simple de parámetros GET
    String url = "http://api.thingspeak.com/update?api_key=" + String(apiKey); //Los campos de dato se van agregando a la URL (borramos la s dejando http en vez de https)
    url += "&field1=" + String(aY, 2);
    url += "&field2=" + String(aZ, 2);
    url += "&field3=" + String(aX, 2);
    // Agregare los demás campos cuando los necesite
  
    Serial.println("URL: " + url);
    
    http.begin(url);
    int httpCode = http.GET();
    
    if(httpCode > 0) {
      String payload = http.getString();
      Serial.print("Código HTTP: ");
      Serial.println(httpCode);
      Serial.print("Respuesta: ");
      Serial.println(payload); // ThingSpeak devuelve el número de entrada creado
    } else {
      Serial.print("Error en HTTP: ");
      Serial.println(http.errorToString(httpCode).c_str());
    }
    
    http.end();
}
  

void loop() {

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi desconectado. Reintentando conexión...");
    WiFi.disconnect();
    WiFi.begin(ssid, password);
    int retry = 0;
    while (WiFi.status() != WL_CONNECTED && retry < 10) {
      delay(1000);
      retry++;
    }
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("Reconexión WiFi exitosa.");
    } else {
      Serial.println("Fallo en reconectar WiFi.");
    }
}


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
  sendSensorData();

    // Imprime los datos por serial
  Serial.print("Acelerómetro [g]: ");
  Serial.print("X = "); Serial.print(aX, 2);
  Serial.print(", Y = "); Serial.print(aY, 2);
  Serial.print(", Z = "); Serial.println(aZ, 2);
  Serial.print(", Aceleración neta = "); Serial.println(aSqrt, 2);
  Serial.print(volt_bateria, 3);


  delay(16000);

}
