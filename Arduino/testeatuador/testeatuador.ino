/*
Trabalho Realizado por: 
Ana Sá nº303 ERSC IPVC
Eduardo Junqueira nº30241 ERSC IPVC
*/

/*
Teste do atuador Semáforo.
Teste do Semáforo com Ciclo Realista
Este código implementa o comportamento de um semáforo real, alternando entre os estados vermelho, amarelo e verde. Ele utiliza os presets fornecidos para os LEDs do mesmo.

*/

// Definir pinos atuador:
#define LedRed 23     // LED vermelho no semáforo no pino 23 do ESP32
#define LedYellow 22  // LED amarelo no semáforo no pino 22 do ESP32
#define LedGreen 21   // LED verde no semáforo no pino 21 do ESP32

// Definir estados do semáforo com enum
enum State {
  OFF,    // Todos os LEDs desligados
  RED,    // LED vermelho ligado
  YELLOW, // LED amarelo ligado
  GREEN,  // LED verde ligado
};

// Variável para armazenar o estado atual do semáforo
State currentState = RED;

// Setup inicial
void setup() {
  // Configurar os pinos como saída
  pinMode(LedRed, OUTPUT);
  pinMode(LedYellow, OUTPUT);
  pinMode(LedGreen, OUTPUT);

  // Iniciar comunicação serial
  Serial.begin(115200);
  Serial.println("Teste do semáforo iniciado.");
}

// Loop principal
void loop() {
  // Controlar o semáforo baseado no estado atual
  switch (currentState) {
    case RED:
      digitalWrite(LedRed, HIGH);
      digitalWrite(LedYellow, LOW);
      digitalWrite(LedGreen, LOW);
      Serial.println("Semáforo: Vermelho");
      delay(5000); // Vermelho por 5 segundos
      currentState = GREEN;
      break;

    case GREEN:
      digitalWrite(LedRed, LOW);
      digitalWrite(LedYellow, LOW);
      digitalWrite(LedGreen, HIGH);
      Serial.println("Semáforo: Verde");
      delay(4000); // Verde por 4 segundos
      currentState = YELLOW;
      break;

    case YELLOW:
      digitalWrite(LedRed, LOW);
      digitalWrite(LedYellow, HIGH);
      digitalWrite(LedGreen, LOW);
      Serial.println("Semáforo: Amarelo");
      delay(2000); // Amarelo por 2 segundos
      currentState = RED;
      break;

    case OFF:
      digitalWrite(LedRed, LOW);
      digitalWrite(LedYellow, LOW);
      digitalWrite(LedGreen, LOW);
      Serial.println("Semáforo: OFF");
      delay(1000);
      break;
  }
}
