
/*
Trabalho Realizado por: 
Ana Sá nº303 ERSC IPVC
Eduardo Junqueira nº30241 ERSC IPVC
*/

/*
Teste de luz do processador e do sensor de toque: Para verificar se o ESP32 está a funcionar corretamente e se o sensor de toque deteta interações.
Teste do semáforo: Simulação de um sistema de semáforo real utilizando os LEDs conforme os presets fornecidos.

*/
// Definir pinos sensor e LEDs de diagnóstico:
#define sensorTouch 19 // Sensor de toque no pino 19 do ESP32
#define ledProcessor 2 // LED integrado no ESP32 para diagnóstico (D2)

// Setup inicial
void setup() {
  // Configurar pinos como entrada ou saída
  pinMode(sensorTouch, INPUT);
  pinMode(ledProcessor, OUTPUT);

  // Iniciar comunicação serial para monitorização
  Serial.begin(115200);
  Serial.println("Teste do ESP32 e sensor de toque iniciado.");
}

// Loop principal
void loop() {
  // Acender e apagar o LED integrado para testar o processador
  digitalWrite(ledProcessor, HIGH);
  delay(500); // LED aceso por 500 ms
  digitalWrite(ledProcessor, LOW);
  delay(500); // LED apagado por 500 ms

  // Ler o estado do sensor de toque
  int touchState = digitalRead(sensorTouch);

  // Mostrar o estado do sensor na porta serial
  if (touchState == HIGH) {
    Serial.println("Sensor de toque: Ativo");
  } else {
    Serial.println("Sensor de toque: Inativo");
  }

  delay(100); // Aguardar antes da próxima leitura
}
