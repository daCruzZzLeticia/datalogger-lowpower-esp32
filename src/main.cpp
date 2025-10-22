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
  // entra em deep sleep simulado por 30 segundos
  if (!esta_dormindo)
  {
    Serial.println("\nNOVA LEITURA -------------");

    // PASSO 1: ler os sensores
    Serial.println("Lendo dados dos sensores...");
    DadosSensores dados = gerenciadorSensores.lerSensores();

    // PASSO 2: mostrar dados no console serial
    Serial.println("\nDADOS OBTIDOS:");
    Serial.print("Timestamp: ");
    Serial.println(dados.timestamp_leitura);

    if (dados.temperatura_valida)
    {
      Serial.print("Temperatura: ");
      Serial.print(dados.temperatura);
      Serial.println(" °C");
    }
    else
    {
      Serial.println("[!] Temperatura: Indisponível");
    }

    if (dados.luminosidade_valida)
    {
      Serial.print("Luminosidade: ");
      Serial.print(dados.luminosidade);
      Serial.println(" lux");
    }
    else
    {
      Serial.println("[!] Luminosidade: Indisponível");

      //**********************************************************
      // PASSO 3: TO DOOOOOO (permanência de dados + checksun)
      //**********************************************************

      Serial.println("Tarefas concluídas");

      // PASSO 4: entrar em modo deep sleep simulado
      Serial.println("Definindo timer de sono para" + String(TEMPO_DEEP_SLEEP_DEMO / 1000) + " segundos...");
      esta_dormindo = true;
      tempo_inicio_sono = millis();
      Serial.println("\nClido de sono pode ser interrompido pressionando o botão conectado ao pino " + String(PINO_BOTAO));
      Serial.println("\nEntrando em modo deep sleep");
    }
  }
}