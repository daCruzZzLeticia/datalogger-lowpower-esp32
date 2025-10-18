#include "Arduino.h"

#define BOTAO_PIN 33      // pino conectado ao botão
#define TEMPO_SLEEP 15000 // 15 segundos (simulado)

#define NTC_PIN 35
#define LDR_PIN 34

unsigned long tempo_inicio = 0;
bool dormindo = false;

const float BETA = 3950; // coeficiente beta do termistor

const float GAMMA = 0.7; // gama do fotoresistor
const float RL10 = 33;   // rl10 do fotoresistor

void setup()
{
  Serial.begin(115200);
  delay(500);

  pinMode(BOTAO_PIN, INPUT_PULLUP);

  Serial.println("Início do programa.");
}

void loop()
{
  if (!dormindo)
  {
    // leitura termistor
    int valor_analogico_termistor = analogRead(NTC_PIN);

    // conversão para celsius
    float celsius = 1 / (log(1 / (4095. / valor_analogico_termistor - 1)) / BETA + 1.0 / 298.15) - 273.15;

    // leitura fotoresisitor
    int valor_analogico_fotoresistor = analogRead(LDR_PIN);
    float voltage = valor_analogico_fotoresistor / 4096. * 3.3;
    float resistance = 2000 * voltage / (1 - voltage / 3.3);

    // conversão para lux
    float lux = pow(RL10 * 1e3 * pow(10, GAMMA) / resistance, (1 / GAMMA));

    // apresentação dos dados para teste
    Serial.print("Temperatura: ");
    Serial.print(celsius);
    Serial.print(" °C | Luminosidade: ");
    Serial.println(lux);

    Serial.println("Entrando em modo sleep por 15 segundos...");
    tempo_inicio = millis();
    dormindo = true;
  }

  // simulação do tempo de sono
  if (dormindo && millis() - tempo_inicio >= TEMPO_SLEEP)
  {
    dormindo = false;
    Serial.println("Acordado por timer");
  }

  // botão se pressionado
  if (digitalRead(BOTAO_PIN) == LOW)
  {
    dormindo = false;
    Serial.println("Acordado por botão");
    delay(300);

    // espera soltar o botão:
    while (digitalRead(BOTAO_PIN) == LOW)
      ;
    tempo_inicio = millis(); // reinicia o sono simulado
  }
}