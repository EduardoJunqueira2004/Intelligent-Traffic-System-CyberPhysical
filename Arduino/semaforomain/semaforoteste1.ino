//1 Modelo de Teste!
#include "esp_timer.h" // Biblioteca nativa para temporizadores do ESP32
#include <WiFi.h>      // Biblioteca para conexão Wi-Fi
#include <HTTPClient.h> // Biblioteca para comunicação HTTP

// Definir pinos do semáforo
#define LedRed 23     // LED vermelho no pino 23
#define LedYellow 22  // LED amarelo no pino 22
#define LedGreen 21   // LED verde no pino 21
// Definir pino do sensor de toque
#define sensorTouch 19 // Sensor de toque no pino 19

/** Definição de estados possíveis do semáforo **/
enum State {
  OFF,    // Semáforo desligado
  RED,    // Semáforo no vermelho
  YELLOW, // Semáforo no amarelo
  GREEN   // Semáforo no verde
};

// Configurações de tempo para cada estado do semáforo (em ms)
const unsigned long redDuration = 5000;   // Duração do vermelho
const unsigned long yellowDuration = 2000; // Duração do amarelo
const unsigned long greenDuration = 4000; // Duração do verde

// Variáveis globais
State trafficLightState = RED;         // Estado inicial do semáforo
unsigned long totalTouchTime = 0;      // Tempo total em que o sensor foi tocado
unsigned int touchCount = 0;           // Número total de toques no sensor

// Variáveis para timers
esp_timer_handle_t trafficTimer;  // Timer para alternar estados do semáforo
esp_timer_handle_t sendDataTimer; // Timer para envio de dados ao servidor

// Credenciais Wi-Fi
const char* ssid = "SEU_SSID";         // Nome da rede Wi-Fi
const char* password = "SUA_PASSWORD"; // Senha da rede Wi-Fi

// URL do servidor
const char* serverURL = "http://172.16.192.157:3000/trafficlight";

// Função de callback para alternar o estado do semáforo
void changeTrafficLight(void* arg) {
  switch (trafficLightState) {
    case RED:
      // Mudar para verde
      trafficLightState = GREEN;
      digitalWrite(LedRed, LOW);
      digitalWrite(LedGreen, HIGH);
      Serial.println("Semáforo: Verde");
      esp_timer_start_once(trafficTimer, greenDuration * 1000); // Configurar próximo estado
      break;

    case GREEN:
      // Mudar para amarelo
      trafficLightState = YELLOW;
      digitalWrite(LedGreen, LOW);
      digitalWrite(LedYellow, HIGH);
      Serial.println("Semáforo: Amarelo");
      esp_timer_start_once(trafficTimer, yellowDuration * 1000); // Configurar próximo estado
      break;

    case YELLOW:
      // Mudar para vermelho
      trafficLightState = RED;
      digitalWrite(LedYellow, LOW);
      digitalWrite(LedRed, HIGH);
      Serial.println("Semáforo: Vermelho");
      esp_timer_start_once(trafficTimer, redDuration * 1000); // Configurar próximo estado
      break;

    case OFF:
      // Semáforo desligado
      digitalWrite(LedRed, LOW);
      digitalWrite(LedYellow, LOW);
      digitalWrite(LedGreen, LOW);
      Serial.println("Semáforo: OFF");
      break;
  }
}

// Função de callback para enviar dados ao servidor
void sendTrafficData(void* arg) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverURL); // Conectar ao servidor
    http.addHeader("Content-Type", "application/json"); // Definir cabeçalho JSON

    // Criar o payload JSON
    String jsonPayload = "{\"state\":\"";
    if (trafficLightState == RED) jsonPayload += "RED\",";
    else if (trafficLightState == GREEN) jsonPayload += "GREEN\",";
    else if (trafficLightState == YELLOW) jsonPayload += "YELLOW\",";
    else jsonPayload += "OFF\",";

    jsonPayload += "\"touchCount\":" + String(touchCount) + ",";
    jsonPayload += "\"totalTouchTime\":" + String(totalTouchTime) + "}";

    // Enviar dados para o servidor
    int httpResponseCode = http.POST(jsonPayload);
    if (httpResponseCode > 0) {
      Serial.println("Dados enviados: " + jsonPayload);
    } else {
      Serial.println("Erro ao enviar dados: " + String(httpResponseCode));
    }

    http.end(); // Encerrar conexão HTTP
  } else {
    Serial.println("Wi-Fi desconectado. Não foi possível enviar dados.");
  }
}

// Função para conectar ao Wi-Fi
void connectToWiFi() {
  WiFi.begin(ssid, password);
  Serial.print("Conectando ao Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000); // Esperar conexão
    Serial.print(".");
  }
  Serial.println("\nWi-Fi conectado!");
  Serial.print("Endereço IP: ");
  Serial.println(WiFi.localIP()); // Exibir endereço IP
}

// Função para monitorizar o sensor de toque
void checkTouchSensor() {
  static bool wasTouched = false;        // Estado anterior do sensor
  static unsigned long touchStartTime = 0; // Tempo de início do toque

  if (digitalRead(sensorTouch) == HIGH) {
    if (!wasTouched) {
      // Detetar novo toque
      wasTouched = true;
      touchStartTime = millis(); // Registar tempo inicial do toque
      touchCount++; // Incrementar contagem de toques
      Serial.println("Sensor ativado!");
    }
  } else {
    if (wasTouched) {
      // Finalizar toque
      wasTouched = false;
      totalTouchTime += millis() - touchStartTime; // Atualizar tempo total de toque
      Serial.println("Sensor desativado!");
    }
  }
}

// Setup inicial
void setup() {
  Serial.begin(115200); // Iniciar comunicação serial
  Serial.println("Setup iniciado!");

  // Configurar pinos como entrada ou saída
  pinMode(LedRed, OUTPUT);
  pinMode(LedYellow, OUTPUT);
  pinMode(LedGreen, OUTPUT);
  pinMode(sensorTouch, INPUT);

  // Conectar ao Wi-Fi
  connectToWiFi();

  // Configurar e iniciar timers
  esp_timer_create_args_t trafficTimerArgs = {
    .callback = &changeTrafficLight,
    .name = "TrafficTimer"
  };
  esp_timer_create(&trafficTimerArgs, &trafficTimer);
  esp_timer_start_once(trafficTimer, redDuration * 1000); // Iniciar no vermelho

  esp_timer_create_args_t sendDataTimerArgs = {
    .callback = &sendTrafficData,
    .name = "SendDataTimer"
  };
  esp_timer_create(&sendDataTimerArgs, &sendDataTimer);
  esp_timer_start_periodic(sendDataTimer, 5000 * 1000); // Enviar dados a cada 5 segundos
}

// Loop principal
void loop() {
  checkTouchSensor(); // Monitorizar o sensor de toque

  // Reconectar ao Wi-Fi, se necessário
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi desconectado. Tentando reconectar...");
    connectToWiFi();
  }

  // Aqui pode-se adicionar outras verificações ou lógica no futuro
}
