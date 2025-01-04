//Modelo Final:
/*
  Mini-Projeto Source-code (C) sobre: Sistema Inteligente de Monitorização e Análise de Tráfego Urbano.
  
  ## Objetivo:
  - Conceber e desenvolver um protótipo funcional;
  - Testar e integrar os respetivos device drivers na solução definida previamente;
  - Depurar e corrigir possíveis erros identificados;
  - Realizar testes finais e analisar os resultados obtidos.

  ## Principais Declarações:
  - Utilização de timers de hardware para controlo eficiente do semáforo;
  - Comunicação via Wi-Fi com envio de dados a um servidor HTTP;
  - Registo de interação do utilizador com um sensor de toque;
  - Alternância automática de estados do semáforo baseada em tempos definidos.

  ## Principais funções necessárias:
  -Timers de Hardware: Para gerir os tempos do semáforo sem usar millis.
  -Interrupções: Para ações rápidas e assíncronas, como o toque no sensor.
  -Sockets TCP: Para comunicação direta e eficiente com o servidor Python.
  -HTTP: Para integração com a API e envio de dados estruturados, como o estado do semáforo.

  ## Trabalho realizado pelo grupo nº8:
  - Eduardo Junqueira nº30241;
  - Ana Sá nº30234.

  Disciplina: Sistemas Ciberfísicos | Ano Letivo 2024/2025
*/


#include "esp_timer.h" // Biblioteca para timers de hardware no ESP32
#include <WiFi.h>      // Biblioteca para conexão Wi-Fi
#include <HTTPClient.h> // Biblioteca para comunicação HTTP

// Configuração da rede Wi-Fi
const char* ssid = "SEU_SSID";         // Nome da rede Wi-Fi
const char* password = "SUA_PASSWORD"; // Senha da rede Wi-Fi

// Configuração do servidor
const char* serverIP = "192.168.1.100"; // IP do servidor TCP
const int serverPort = 8090;            // Porta do servidor TCP
const char* serverURL = "http://192.168.1.100:8080/api/trafficlight"; // URL da API HTTP

// Definição de pinos do semáforo e do sensor
#define LedRed 23     // LED vermelho no pino 23
#define LedYellow 22  // LED amarelo no pino 22
#define LedGreen 21   // LED verde no pino 21
#define sensorTouch 19 // Sensor de toque no pino 19

// Estados possíveis do semáforo
enum State {
  OFF,    // Semáforo desligado
  RED,    // Semáforo no vermelho
  YELLOW, // Semáforo no amarelo
  GREEN   // Semáforo no verde
};

State trafficLightState = RED;          // Estado inicial do semáforo
unsigned long totalTouchTime = 0;       // Tempo total de toque no sensor
unsigned int touchCount = 0;            // Número de toques no sensor

esp_timer_handle_t trafficTimer;        // Timer para alternância automática do semáforo
esp_timer_handle_t sendDataTimer;       // Timer para envio de dados via HTTP

WiFiClient client;                      // Cliente TCP para comunicação com o servidor

// Função para conectar ao Wi-Fi
void connectToWiFi() {
  WiFi.begin(ssid, password);           // Iniciar conexão com a rede Wi-Fi
  Serial.print("Conectando ao Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);                        // Esperar 1 segundo enquanto tenta conectar
    Serial.print(".");
  }
  Serial.println("\nWi-Fi conectado!"); // Confirmar conexão
}

// Função para conectar ao servidor TCP
void connectToServer() {
  if (!client.connected()) {            // Verificar se o cliente não está conectado
    Serial.println("Conectando ao servidor TCP...");
    if (client.connect(serverIP, serverPort)) { // Tentar conectar ao servidor
      Serial.println("Conexão ao servidor TCP estabelecida!");
    } else {
      Serial.println("Falha na conexão ao servidor TCP.");
    }
  }
}

// Função para alternar estados do semáforo automaticamente
void changeTrafficLight(void* arg) {
  switch (trafficLightState) {
    case RED:                           // Do vermelho para o verde
      trafficLightState = GREEN;
      digitalWrite(LedRed, LOW);        // Apagar vermelho
      digitalWrite(LedGreen, HIGH);     // Acender verde
      Serial.println("Semáforo: Verde");
      esp_timer_start_once(trafficTimer, 4000 * 1000); // Configurar próximo estado em 4s
      break;

    case GREEN:                         // Do verde para o amarelo
      trafficLightState = YELLOW;
      digitalWrite(LedGreen, LOW);      // Apagar verde
      digitalWrite(LedYellow, HIGH);    // Acender amarelo
      Serial.println("Semáforo: Amarelo");
      esp_timer_start_once(trafficTimer, 2000 * 1000); // Configurar próximo estado em 2s
      break;

    case YELLOW:                        // Do amarelo para o vermelho
      trafficLightState = RED;
      digitalWrite(LedYellow, LOW);     // Apagar amarelo
      digitalWrite(LedRed, HIGH);       // Acender vermelho
      Serial.println("Semáforo: Vermelho");
      esp_timer_start_once(trafficTimer, 5000 * 1000); // Configurar próximo estado em 5s
      break;

    case OFF:                           // Semáforo desligado
      digitalWrite(LedRed, LOW);
      digitalWrite(LedYellow, LOW);
      digitalWrite(LedGreen, LOW);
      Serial.println("Semáforo: OFF");
      break;
  }
}

// Função para enviar dados do semáforo via HTTP
void sendTrafficData(void* arg) {
  if (WiFi.status() == WL_CONNECTED) {  // Verificar se está conectado ao Wi-Fi
    HTTPClient http;
    http.begin(serverURL);              // Iniciar conexão HTTP com a API
    http.addHeader("Content-Type", "application/json"); // Definir cabeçalho JSON

    // Criar payload JSON com os dados do semáforo
    String jsonPayload = "{\"state\":\"";
    if (trafficLightState == RED) jsonPayload += "RED\"";
    else if (trafficLightState == GREEN) jsonPayload += "GREEN\"";
    else if (trafficLightState == YELLOW) jsonPayload += "YELLOW\"";
    else jsonPayload += "OFF\"";

    jsonPayload += ",\"touchCount\":" + String(touchCount);
    jsonPayload += ",\"totalTouchTime\":" + String(totalTouchTime) + "}";

    // Enviar dados para a API
    int httpResponseCode = http.POST(jsonPayload);
    if (httpResponseCode > 0) {
      Serial.println("Dados enviados: " + jsonPayload);
    } else {
      Serial.println("Erro ao enviar dados: " + String(httpResponseCode));
    }

    http.end();                         // Encerrar conexão HTTP
  } else {
    Serial.println("Wi-Fi desconectado. Não foi possível enviar dados.");
  }
}

// Função para monitorizar o sensor de toque
void checkTouchSensor() {
  static bool wasTouched = false;       // Estado anterior do sensor

  if (digitalRead(sensorTouch) == HIGH) { // Verificar se o sensor foi tocado
    if (!wasTouched) {
      wasTouched = true;                // Atualizar estado para "tocado"
      touchCount++;                     // Incrementar número de toques
      Serial.println("Sensor ativado!");
      trafficLightState = GREEN;        // Exemplo: Alterar estado para verde
      controlTrafficLight(trafficLightState);
    }
  } else {
    if (wasTouched) {                   // Se o sensor não está mais tocado
      wasTouched = false;
      Serial.println("Sensor desativado!");
    }
  }
}

// Setup inicial
void setup() {
  Serial.begin(115200);                 // Inicializar comunicação serial
  pinMode(LedRed, OUTPUT);
  pinMode(LedYellow, OUTPUT);
  pinMode(LedGreen, OUTPUT);
  pinMode(sensorTouch, INPUT);

  connectToWiFi();                      // Conectar ao Wi-Fi

  // Configurar timers
  esp_timer_create_args_t trafficTimerArgs = {.callback = &changeTrafficLight, .name = "TrafficTimer"};
  esp_timer_create(&trafficTimerArgs, &trafficTimer);
  esp_timer_start_once(trafficTimer, 5000 * 1000); // Iniciar no vermelho

  esp_timer_create_args_t sendDataTimerArgs = {.callback = &sendTrafficData, .name = "SendDataTimer"};
  esp_timer_create(&sendDataTimerArgs, &sendDataTimer);
  esp_timer_start_periodic(sendDataTimer, 5000 * 1000); // Enviar dados a cada 5 segundos
}

// Loop principal
void loop() {
  checkTouchSensor();                   // Monitorizar sensor de toque

  // Reconectar ao Wi-Fi, se necessário
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wi-Fi desconectado. Tentando reconectar...");
    connectToWiFi();                    // Reconectar ao Wi-Fi
  }
}
