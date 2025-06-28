//TX P2P
//COM 4: LORA CONECTADO X ATRAS  (CABLE NEGRO)      RECEPTOR
//COM 6: LORA LATERAL DERECHO     (CABLE AZUL)      TRANSMISOR

// Heltec LoRa V2 pinout típico (puede variar)
#define LORA_SCK     5
#define LORA_MISO   19
#define LORA_MOSI   27
#define LORA_SS     18
#define LORA_RST    14
#define LORA_DIO0   26
//Pines definidos arriba son Modificacion

#include <SPI.h>
#include <LoRa.h>

int counter = 0;

void setup() {
  Serial.begin(9600);

  while (!Serial); //Espera a que se abra el Monitor Serial en la pc antes de continuar con la ejecución del programa.

   // Configura pines para Heltec
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0); // Modificacion

  Serial.println("LoRa Sender");

  if (!LoRa.begin(915E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
}

void loop() {
  Serial.print("Sending packet: ");
  Serial.println(counter);

  // send packet
  LoRa.beginPacket();
  LoRa.print("hello ");
  LoRa.print(counter);
  LoRa.endPacket();

  counter++;

  delay(10000);
}