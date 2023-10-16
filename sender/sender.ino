// Código para o envio de informações para os robôs

#include <esp_now.h>
#include <esp_wifi.h>
#include <WiFi.h>

// Definindo variáveis ------------------------------------------

// MAC Adress genérico para enviar os dados no canal selecionado
uint8_t broadcast_adr[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

#define CANAL 1

// Variáveis para armazenar informação recebida
char receivedChar;

typedef struct struct_message{
  char message;
  } struct_message;

struct_message command;

//Peer do ESP-NOW
esp_now_peer_info_t peerInfo;

// Tratando informação recebida via serial -----------------------

void recvSerial(){
  if (Serial.available() > 0)
  {
    receivedChar = Serial.read();
    Serial.println("Recebido via serial: " + String(receivedChar));
  }
}

// Função de envio via ESP-NOW -----------------------------------

void sendData(){   
    esp_err_t message = esp_now_send(broadcast_adr, (uint8_t *) &command, sizeof(command));
    delay(3); // esse delay é necessário para que os dados sejam enviados corretamente
}

//Setup e loop ---------------------------------------------------------------

void setup() {

  // Inicializando comunicação serial
  Serial.begin(115200);

  // Configuração ESP-NOW
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));
  ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
  ESP_ERROR_CHECK(esp_wifi_start());
  ESP_ERROR_CHECK(esp_wifi_set_channel(CANAL, WIFI_SECOND_CHAN_NONE));
  esp_wifi_set_max_tx_power(84);


  if (esp_now_init() != ESP_OK) 
  {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  memcpy(peerInfo.peer_addr, broadcast_adr, 6);
  if (esp_now_add_peer(&peerInfo) != ESP_OK) 
  {
    Serial.println("Failed to add peer");
    return;
  }
 
}

void loop() {
  recvSerial();
  command.message = receivedChar;
  sendData();
}