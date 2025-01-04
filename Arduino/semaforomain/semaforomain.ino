/*
Trabalho Realizado por: 
Ana Sá nº303 ERSC IPVC
Eduardo Junqueira nº30241 ERSC IPVC
*/
/*
Código main inicial. Tudo aqui depois existe códigos externos de testes!
Registo de dados:
Tempo de cada estado do semáforo (vermelho, amarelo, verde).
Contagem do número de toques no sensor.
Tempo total em que o sensor está ativo.
Estruturação para simular um processo de tráfego humano:
Ajustar as transições de estados para refletir o comportamento esperado.
Garantir que os dados registados possam ser enviados para a API mais tarde.
*/
#include "esp_timer.h" // Biblioteca nativa para temporizadores do ESP32
#include <WiFi.h>      // Biblioteca para ter ligação ao Wi-Fi
#include <HTTPClient.h>

// Definir pinos atuador:
#define LedRed 23     // LED vermelho no semáforo no pino 23 do ESP32
#define LedYellow 22  // LED amarelo no semáforo no pino 22 do ESP32
#define LedGreen 21   // LED verde no semáforo no pino 21 do ESP32
// Definir pinos sensor:
#define sensorTouch 19 // Sensor de toque no pino 19 do ESP32

/** Definir estados do semáforo **/
enum State {
  OFF,
  RED,
  YELLOW,
  GREEN
};

// Variáveis globais
bool touchActivated = false;
State trafficLightState = OFF;
unsigned long stateStartTime = 0; // Tempo de início do estado atual
unsigned long totalTouchTime = 0; // Tempo total de toque no sensor
unsigned int touchCount = 0;      // Número de toques no sensor

// Credenciais Wi-Fi
const char* ssid = "SEU_SSID";
const char* password = "SUA_PASSWORD";

// URL do servidor
const char* serverURL = "http://172.16.192.157:3000/trafficlight";

// Função para conectar ao Wi-Fi
void connectToWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Conectando ao Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nWi-Fi conectado!");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP());
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
  // Atualizar o tempo de início do estado
  stateStartTime = millis();
}

// Função para verificar o sensor de toque
void checkTouchSensor() {
  static bool wasTouched = false;
  unsigned long touchStartTime = 0;

  if (digitalRead(sensorTouch) == HIGH) {
    if (!wasTouched) {
      wasTouched = true;
      touchStartTime = millis();
      touchCount++; // Incrementar contagem de toques
      Serial.println("Sensor ativado!");
    }
  } else {
    if (wasTouched) {
      wasTouched = false;
      totalTouchTime += millis() - touchStartTime; // Calcular tempo total de toque
      Serial.println("Sensor desativado!");
    }
  }
}

// Função para enviar dados ao servidor
void sendTrafficData() {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverURL);
    http.addHeader("Content-Type", "application/json");

    // Criar payload JSON
    String jsonPayload = "{\"state\":\"";
    if (trafficLightState == RED) jsonPayload += "RED\",";
    else if (trafficLightState == GREEN) jsonPayload += "GREEN\",";
    else if (trafficLightState == YELLOW) jsonPayload += "YELLOW\",";
    else jsonPayload += "OFF\",";

    jsonPayload += "\"touchCount\":" + String(touchCount) + ",";
    jsonPayload += "\"totalTouchTime\":" + String(totalTouchTime) + ",";
    jsonPayload += "\"stateTime\":" + String(millis() - stateStartTime) + "}";

    // Enviar dados
    int httpResponseCode = http.POST(jsonPayload);
    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Resposta do servidor: " + response);
    } else {
      Serial.println("Erro ao enviar dados: " + String(httpResponseCode));
    }

    http.end();
  } else {
    Serial.println("Wi-Fi desconectado. Não foi possível enviar dados.");
  }
}

// Setup inicial
void setup() {
  Serial.begin(115200);
  Serial.println("Setup iniciado!");

  // Configurar pinos
  pinMode(LedRed, OUTPUT);
  pinMode(LedYellow, OUTPUT);
  pinMode(LedGreen, OUTPUT);
  pinMode(sensorTouch, INPUT);

  connectToWiFi();
  controlTrafficLight(OFF);
}

// Loop principal
void loop() {
  checkTouchSensor();

  // Simular transições de semáforo
  if (millis() - stateStartTime > 5000) { // Tempo para mudar o estado (5 segundos)
    if (trafficLightState == RED) trafficLightState = GREEN;
    else if (trafficLightState == GREEN) trafficLightState = YELLOW;
    else if (trafficLightState == YELLOW) trafficLightState = RED;

    controlTrafficLight(trafficLightState);
  }

  sendTrafficData();
  delay(1000); // Enviar dados a cada 1 segundo
}
