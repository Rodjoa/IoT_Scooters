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
#include <Polyline.h>

//Credenciales wifi (Mi casa)
const char* ssid = "VTR-6216549";
const char* password = "n4rdVvqptnkf";
//Credenciales Thingspeak
//const char* server = "http://api.thingspeak.com/channels/2998250/bulk_update.json"; //OPCION 1  Se usa con formato de JSON PARA BULK UPDATE
const char* server = "http://api.thingspeak.com/update";                            //OPCION 2  Usa formato simple de parametros get
//const char* apiKey = "X52LATNR1YTYZBJD";  // << TU Write API Key
const char* apiKey = "LVEIPL1ESA1SI07Z";    //Canal de prueba IoT_WiFi_Scooters


//Variables acelerometro (PIN gpio 21 para SDA y GPIO 22 para SCL)
MPU9250_asukiaaa mySensor;
float aX, aY, aZ, aSqrt, aceleration_converted;


//Variables Bateria
#define BatteryLevel 33  // Pin ADC del Heltec (GPIO33)

float lectura_adc;
float volt_adc;
float volt_bateria;
// Resistencias del divisor
const float R1 = 50000.0; // 50kΩ
const float R2 = 33000.0; // 33kΩ

const float V_MAX = 9.5;
const float V_MIN = 6.0;
float porcentaje_bateria; 


//Variables GPS
HardwareSerial gpsSerial(2);
TinyGPSPlus gps;

Polyline mypolyline;

//´Parametros scooter
const int Id_viaje = 1;
const int Id_scooter = 34;


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
  
  //int i = 0;
  while (WiFi.status() != WL_CONNECTED) {   
    delay(200);
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

  void sendSensorData(){  //Utiliza las variables globales
    HTTPClient http;

    //CODIFICAREMOS AQUI LAS COORDENADAS GPS A FORMATO POLYLINE, POR SIMPLICIDAD
    String gps_encoded = mypolyline.encodePolylinePoint(gps.location.lat(), gps.location.lng());
    //Conversion aceleracion: El mpu9250 mide en "gravedades = 9.81m/s^2", así que lo pasaremos a metros.
    //float aceleration_converted = aSqrt* 9.81;

    // Usa HTTP y formato simple de parámetros GET
    String url = "http://api.thingspeak.com/update?api_key=" + String(apiKey); //Los campos de dato se van agregando a la URL (borramos la s dejando http en vez de https)
    url += "&field1=" + String(Id_viaje);                     //Id Viaje cte=1
    url += "&field2=" + String(Id_scooter);                   //Id Scooter cte = 034
    url += "&field3=" + gps_encoded;                          //GPS codificado en Polyline
    url += "&field4=" + String(aceleration_converted,2);      //Aceleración neta (euclidiana)
    url += "&field5=" + String(porcentaje_bateria, 2);              //Nivel de batería (%). Se considera bateria llena a 9.5V 
    

    // Segun Yohanns deben ir estos campos: ID viaje (ctte 1), Id scooter (elegir), GPS CODIFICADO (DE?), Aceleración euclidiana y Nivel de bateria. ( 5 campos) 
  
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
      delay(200); //cambiamos 1000 por 500 en start wifi igual
      retry++;
    }
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("Reconexión WiFi exitosa.");
    } else {
      Serial.println("Fallo en reconectar WiFi.");
    }
}


  //read Acelerometter. COMENTAMOS PARA VER EL EFECTO DE TOMAR MENOS LECTURAS
  mySensor.accelUpdate();
  aX = mySensor.accelX();
  delay(20);
  aY = mySensor.accelY();
  delay(20);
  aZ = mySensor.accelZ();
  delay(20);
  aSqrt = mySensor.accelSqrt();
  aceleration_converted = aSqrt * 9.81;   //Convertimos aceleracion desde "gravedades" a la unidad m/s^2

  //read Battery
  lectura_adc = analogRead(BatteryLevel);              // valor de 0 a 4095
  volt_adc = lectura_adc / 4095.0 * 3.3;               // voltaje en el pin
  volt_bateria = volt_adc * ((R1 + R2) / R2);          // voltaje real
  //Calcularemos el porcentaje de carga ttomando como Voltaje minimo de operacion 6V
  // Cálculo del porcentaje de carga basado en rango operativo
  //float porcentaje_bateria = (volt_bateria - V_MIN) / (V_MAX - V_MIN) * 100.0;
  porcentaje_bateria = (volt_bateria - V_MIN) / (V_MAX - V_MIN) * 100.0;
  porcentaje_bateria = constrain(porcentaje_bateria, 0, 100);  // Limita a 0–100%

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
  Serial.print("Id Viaje: "); Serial.print(Id_viaje);
  Serial.print("Id Scooter: "); Serial.print(Id_scooter);
  Serial.print(", GPS encoded = "); Serial.print(mypolyline.encodePolylinePoint(gps.location.lat(), gps.location.lng()));
  Serial.print(", Aceleración [m/s^2] = "); Serial.println(aceleration_converted, 2);
  Serial.print(", volt_ADC = "); Serial.println(volt_adc,3);
  Serial.print(", volt_bateria = "); Serial.println(volt_bateria,3);
  Serial.print(",porcentaje_bateria = "); Serial.println(porcentaje_bateria);


  delay(16000);

}
