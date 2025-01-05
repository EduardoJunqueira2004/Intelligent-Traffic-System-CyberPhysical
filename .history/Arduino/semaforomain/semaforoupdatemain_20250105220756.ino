//Begin.
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

  ## Trabalho realizado pelo grupo nº8 Sistemas Ciberfísicos:
  - Eduardo Junqueira nº30241;
  - Ana Sá nº30234.

  Disciplina: Sistemas Ciberfísicos | Ano Letivo 2024/2025.
*/

/*
Code Sensetive Case!
*/

//Bibliotecas:
#include "esp_timer.h" //Biblioteca para timers de hardware no ESP32;

#include <WiFi.h>//Biblioteca de conexão wifi;

#include <HTTPClient.h>//Biblioteca de comunicação HTTP;

/*
Configurações.
*/

//Rede Wi-fi configuração:
const char* ssid = "MEO-FODF40"; //Nome da rede Wi-fi;
const char* password = "cf227eb984"; // Senha da rede Wi-Fi;

//Servidor configuração:
const char* serverIP ="192.168.1.112";//IP do servidorTCP;
const int serverPort = 8090;//Porta do servidor TCP;
const char* serverURL ="http://192.168.1.112:8080/dados";//URL da API HTTP;

/*
Definição de pinos do atuador e do sensor.
*/

//Define Atuador(Semáforo):
#define LedRed 23 //LED vermelho no pino 23 do ESP32;
#define LedYellow 22 //LED amarelo no pino 22 do ESP32;
#define LedGreen 21 //LED verde no pino 21;

//Define Sensor(Sensor de Toque Touch):
#define sensorTouch 19 //Sensor de toque no pino 19 do ESP32;

/*
Estados
*/

//Diferentes estados possíveis do Semáforo:
enum State{
  OFF, //semáforo desligado;
  RED, //Semáforo no vermelho;
  YELLOW, //Semáforo no amarelo;
  GREEN //Semáforo no verde;
};

//Estado Inicial do Loop:
State trafficLightState = OFF; //O estado inicial do semáforo é desligado;

//Variáveis sensor:
unsigned long totalTouchTime = 0; //Tempo total de toque no sensor;
unsigned int touchCount = 0; //Número de toques no sensor;

//Timer no ESP32:
esp_timer_handle_t trafficTimer; //Timer para alternância automática do semáforo;
esp_timer_handle_t sendDataTimer; //Timer para envio de dados via HTTP;

//Client:
WiFiClient client; //cliente TCP para comunicação com o servidor;

/*
Funções.
*/

//Função para conectar ao Wi-Fi:
void connectToWiFi(){
  WiFi.begin(ssid, password);//Iniciar a conexão com a rede Wi-FI;
  Serial.print("A conectar ao Wi-Fi");//Mensagem no Serial de conexão ao Wi-Fi;
  while (WiFi.status() != WL_CONNECTED){
    delay(5000);//Espera 5 segundos enquanto o estado do WiFi está não conectado;
    Serial.print(".");//tempo de espera no Serial; .....
  }
  Serial.println("\nWi-Fi conectado!");//Mensagem no Serial Conectado com sucesso;
}

//Função para conectar ao Servidor TCP:
void connectToServer(){
  if(!client.connected()){ //Verifica se o Cliente não está conectado;
  Serial.println("Conectando ao Servidor TCP...");
  if(client.connect(serverIP, serverPort)){// Conectar ao Servidor;
    Serial.println("Conexão ao servidor TCP estabelecida!");//Mensagem no Serial de conexão ao Servidor com sucesso;
  } else{
    Serial.println("Falha na conexão ao servidor TCP");//Se não Mensagem no Serial de conexão ao Servidor sem sucesso; 
  }
  }
}

//Função para alternar estados do semáforo:
void changeTrafficLight(void* arg){
  switch(trafficLightState){

    case RED:         //Do vermelho para o verde;
    trafficLightState = GREEN;//Estado GREEN;
    digitalWrite(LedRed, LOW); //Apagar vermelho;
    digitalWrite(LedGreen, HIGH);//Acender verde;
    Serial.println("Semáforo: Verde");//Mensagem Serial de Semáforo na cor Verde;
    esp_timer_start_once(trafficTimer, 4000 * 1000); //configurar o próximo estado em 4 segundos;
    break;

    case GREEN:       //Do verde para o amarelo;
    trafficLightState = YELLOW;//Estado YELLOW;
    digitalWrite(LedGreen, LOW);//Apagar o verde;
    digitalWrite(LedYellow, HIGH);//Acender o amarelo;
    Serial.println("Semáforo: Amarelo");//Mensagem Serial de Semáforo na cor Amarelo;
    esp_timer_start_once(trafficTimer, 2000 * 1000);//configurar próximo estado em 2s;
    break;

    case YELLOW:      //Do amarelo para o vermelho;
    trafficLightState = RED;//Estado RED;
    digitalWrite(LedYellow, LOW);//Apagar o amarelo;
    digitalWrite(LedRed, HIGH);//Acender o vermelho;
    Serial.println("Semáforo: Vermelho");//Mensagem Serial  de Semáforo na cor Vermelho;
    esp_timer_start_once(trafficTimer, 5000 * 1000);//Configurar próximo estado em 5 segundos;
    break;

    case OFF:       //Semáforo Desligado;
    digitalWrite(LedRed, LOW);//Apagar vermelho;
    digitalWrite(LedYellow, LOW);//Apagar amarelo;
    digitalWrite(LedGreen, LOW);//Apagar Verde;
    Serial.println("Semáforo: Desligado");//mensagem Serial de Semáforo Desligado;
    break;
  }
}

//Função para enviar dados do semáforo via HTTP:
void sendTrafficData(void* arg){
  if(WiFi.status() == WL_CONNECTED){  //Verificar se está conectado ao Wi-Fi;
  HTTPClient http;
  http.begin(serverURL);  //Iniciar conexão HTTP com a API;
  http.addHeader("Content-Type","application/json");//Definir cabeçalho JSON;

  //Criar payload JSON com os dados do semáforo:
  String jsonPayload = "{\"state\":\"";//Endpoit State;
  if (trafficLightState == RED) jsonPayload += "RED\"";//Se o estado for RED vai ao Endpoit RED;
  else if (trafficLightState == GREEN) jsonPayload += "GREEN\"";//Se o estado for GREEN vai ao Endpoit GREEN;
  else if (trafficLightState == YELLOW) jsonPayload += "YELLOW\"";//Se o estado for YELLOW vai ao Endpoit YELLOW;
  else jsonPayload += "OFF\"";
    jsonPayload += ",\"touchCount\":" + String(touchCount);//Endpoint touchCount;
    jsonPayload += ",\"totalTouchTime\":" + String(totalTouchTime) + "}";//Endpoint totalTouchTime;

    // Enviar dados para a API
    int httpResponseCode = http.POST(jsonPayload);//variável de serviço HTTP POST;
    if(httpResponseCode > 0){//Se existir code de response ele envia;
      Serial.println("Dados enviados:" + jsonPayload);//Mensagem Serial de dados enviados do Payload para a API;
    }else{
      Serial.println("Erro ao enviar dados:" + String(httpResponseCode));//Se não mensagem  no Serial de dados não enviados + o erro da página;
    }

    http.end();       //Encerrar a conexão HTTP;
  } else {
    Serial.println("Wi-Fi desconectado. Não foi possível enviar dados.");//Se o Wi-Fi estiver desconectado no Serial de dados envia que não foi possível;
  }
}

//Função para verificar o sensor de toque:
void checkTouchSensor(){
  static bool wasTouched = false; //Estado anterior do sensor;
    if(digitalRead(sensorTouch) == HIGH){//Verificar se o sensor foi tocado;
    if(!wasTouched){
      wasTouched = true; //Atualizar estado para "tocado";
      touchCount++;//Incrementa o número de toques;
      Serial.println("Sensor ativado!");//Mensagem no Serial de sensor ativado com sucesso;
      trafficLightState = GREEN; //Altera para verde sempre que for tocado;
      changeTrafficLight(nullptr);
    }

    }else{
      if(wasTouched){ //Se o sensor não está mais a ser "tocado";
        wasTouched = false;
        Serial.println("Sensor desativado!");//Mensagem Serial Sensor não está mais a ser tocado;
      }
    }
}

/*
  Setup Inicial.
*/

void setup(){
  Serial.begin(115200); //inicia a comunicação com 115200 de Serial Monitor;
  //definir os pinos do sensor e atuador:
  Serial.println("In Setup function!"); //Mensagem de Serial de início da função Setup;
  pinMode(LedRed, OUTPUT);//Definir o pino correspondente ao LedRed como saída;
  pinMode(LedYellow, OUTPUT);//Definir o pino correspondente ao LedYellow como saída;
  pinMode(LedGreen, OUTPUT);//Definir o pino correspondente ao LedGreen como saída;
  pinMode(sensorTouch, INPUT);//Definir o pino correspondente ao sensorTouch como entrada;

  connectToWiFi();//Chamar a função de conexão ao Wi-Fi;

  /*
  Configurar os Timers.
  */

  //Cria argumentos:
  esp_timer_create_args_t trafficTimerArgs = {.callback = &changeTrafficLight, .name = "TrafficTimer"};
  esp_timer_create(&trafficTimerArgs, &trafficTimer);
  esp_timer_start_once(trafficTimer, 5000 * 1000);// inicia este tempo no estado inicial!

  //Envia argumentos:
  esp_timer_create_args_t sendDataTimerArgs={.callback = &sendTrafficData, .name="SendDataTimer"};
  esp_timer_create(&sendDataTimerArgs, &sendDataTimer);
  esp_timer_start_periodic(sendDataTimer, 5000 * 1000);//Enviar dados a cada 5 segundos;

}

/*
  Loop de controle Principal
*/

void loop(){
  checkTouchSensor();       //Chama a função de verificação constante de Toque no sensor;

  //Reconectar ao Wi-Fi, se necessário:
  if(WiFi.status() != WL_CONNECTED){
    Serial.println("Wi-Fi desconectado. Tentando reconectar...");//Mensagem Serial de reconexão ao Wi-Fi;
    connectToWiFi();      //Reconectar ao Wi-Fi;

  }
}
