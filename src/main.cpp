#include "Arduino.h"

#include "config.h"
#include "gerenciador_sensores.h"

// cria o gerenciador de sensores
GerenciadorSensores gerenciadorSensores;

// estado de deep sleep simulado
bool esta_dormindo = false;
unsigned long tempo_inicio_sono = 0;

void setup()
{
  Serial.begin(115200);
  delay(1000);

  Serial.println("-----------------------------------------");
  Serial.println("Data Logger Inicializado");

  // configura o pino do botão para wake-up
  pinMode(PINO_BOTAO, INPUT_PULLUP);
  Serial.println("Botão configurado no pino: " + String(PINO_BOTAO));

  // inicialização do gerenciador de sensores
  gerenciadorSensores.iniciar(); // olhar sensores estão disponíveis

  gerenciadorSensores.imprimirStatus(); // mostra o status dos sensores

  Serial.println("\nSetup configurado");

  Serial.println("Modo Deep Sleep Simulado (30 segundos)");
  Serial.println("-----------------------------------------");
}

void loop()
{
}