#include "Arduino.h"

#define PINO_BOTAO 33    // pino conectado ao botão
#define TEMPO_SONO 15000 // 15 segundos (simulado)

unsigned long tempo_inicio = 0;
bool dormindo = false;

void setup()
{
  Serial.begin(115200);
  delay(500);

  pinMode(PINO_BOTAO, INPUT_PULLUP);

  Serial.println("Início do programa.");
}

void loop()
{
  if (!dormindo)
  {
    Serial.println("Entrando em modo sleep por 15 segundos...");
    tempo_inicio = millis();
    dormindo = true;
  }

  // simulação do tempo de sono
  if (dormindo && millis() - tempo_inicio >= TEMPO_SONO)
  {
    dormindo = false;
    Serial.println("Acordado por timer");
  }

  // botão se pressionado
  if (digitalRead(PINO_BOTAO) == LOW)
  {
    dormindo = false;
    Serial.println("Acordado por botão");
    delay(300);

    while (digitalRead(PINO_BOTAO) == LOW)
      ;                      // espera soltar o botão
    tempo_inicio = millis(); // reinicia o sono simulado
  }
}