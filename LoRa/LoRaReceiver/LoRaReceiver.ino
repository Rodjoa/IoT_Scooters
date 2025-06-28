//RX P2P
//COM 4: LORA CONECTADO X ATRAS
//COM 6: LORA LATERAL DERECHO (CABLE AZUL)

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

void setup() {
  Serial.begin(9600); //he ajustado este parametro entre el monitor serial 115200 y este valor (probado combinaciones)
  //Serial.begin(115200);
  while (!Serial);//Espera a que se abra el Monitor Serial en la pc antes de continuar con la ejecución del programa.
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0); // Modificacion

  Serial.println("LoRa Receiver");

  if (!LoRa.begin(915E6)) {
    Serial.println("Starting LoRa failed!");
    while (1);
  }
}

void loop() {
  // try to parse packet
  Serial.println("ENTRA AL LOOP '"); //debuggin
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    // received a packet
    Serial.print("Received packet '");

    // read packet
    while (LoRa.available()) {
      Serial.print((char)LoRa.read());
    }

    // print RSSI of packet
    Serial.print("' with RSSI ");
    Serial.println(LoRa.packetRssi());
  }
  Serial.println("SALE DEL LOOP '"); //debuggin
  delay(1000); //GENERE UN DEFASE POR ESO NO SE SINCRONIZABAN
}