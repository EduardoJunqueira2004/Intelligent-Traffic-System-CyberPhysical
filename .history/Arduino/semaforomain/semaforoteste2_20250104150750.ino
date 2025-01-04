//2 modelo de Teste
#include <WiFi.h>
#include <WiFiClient.h>

// Definir pinos do semáforo
#define LedRed 23
#define LedYellow 22
#define LedGreen 21
#define sensorTouch 19

// Configuração da rede Wi-Fi
const char* ssid = "SEU_SSID";
const char* password = "SUA_PASSWORD";

// Configuração do servidor TCP
const char* serverIP = "192.168.1.100"; // IP do servidor (API)
const int serverPort = 8090;

// Estados do semáforo
enum State {
  OFF,
  RED,
  YELLOW,
  GREEN
};
State trafficLightState = RED; // Estado inicial do semáforo

// Função para conectar ao Wi-Fi
void connectToWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Conectando ao Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi conectado!");
}

// Função para controlar o semáforo
void controlTrafficLight(State state) {
  digitalWrite(LedRed, LOW);
  digitalWrite(LedYellow, LOW);
  digitalWrite(LedGreen, LOW);

  switch (state) {
    case RED:
      digitalWrite(LedRed, HIGH);
      Serial.println("Semáforo: Vermelho");
      break;
    case YELLOW:
      digitalWrite(LedYellow, HIGH);
      Serial.println("Semáforo: Amarelo");
      break;
    case GREEN:
      digitalWrite(LedGreen, HIGH);
      Serial.println("Semáforo: Verde");
      break;
    case OFF:
      Serial.println("Semáforo: OFF");
      break;
  }
}

// Função para enviar o estado do semáforo ao servidor
void sendTrafficState(WiFiClient& client) {
  if (client.connected()) {
    String msg = "ID=10&STATE=";
    if (trafficLightState == RED) msg += "RED";
    else if (trafficLightState == YELLOW) msg += "YELLOW";
    else if (trafficLightState == GREEN) msg += "GREEN";
    else msg += "OFF";

    client.print(msg);
    Serial.println("Estado enviado: " + msg);
  } else {
    Serial.println("Não foi possível enviar o estado. Sem conexão.");
  }
}

// Função para receber comandos do servidor
void receiveCommand(WiFiClient& client) {
  if (client.connected() && client.available()) {
    String command = client.readStringUntil('\n');
    Serial.println("Comando recebido: " + command);

    if (command == "RED") trafficLightState = RED;
    else if (command == "YELLOW") trafficLightState = YELLOW;
    else if (command == "GREEN") trafficLightState = GREEN;
    else if (command == "OFF") trafficLightState = OFF;

    controlTrafficLight(trafficLightState);
  }
}

// Setup inicial
void setup() {
  Serial.begin(115200);
  pinMode(LedRed, OUTPUT);
  pinMode(LedYellow, OUTPUT);
  pinMode(LedGreen, OUTPUT);
  pinMode(sensorTouch, INPUT);

  connectToWiFi();
}

// Loop principal
void loop() {
  WiFiClient client;

  // Conectar ao servidor TCP
  if (!client.connect(serverIP, serverPort)) {
    Serial.println("Falha na conexão ao servidor.");
    delay(1000);
    return;
  }

  // Enviar estado do semáforo
  sendTrafficState(client);

  // Receber comandos do servidor
  receiveCommand(client);

  client.stop(); // Fechar conexão
  delay(1000);   // Intervalo de 1 segundo
}
