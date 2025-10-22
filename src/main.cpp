#include "config.h"
#include "gerenciador_sensores.h"
#include "Arduino.h"

// cria o gerenciador de sensores
GerenciadorSensores gerenciadorSensores;

// estado de deep sleep simulado
bool esta_dormindo = false;
unsigned long tempo_inicio_sono = 0;

void setup()
{
  Serial.begin(115200);
  delay(1000);
  Serial.println("------------------------------------------------");
  Serial.println("sistema data logger - temperatura e luminosidade");
  Serial.println("------------------------------------------------");

  Serial.println("[modo]: configuração do sistema");

  // configura o pino do botão para wake-up
  pinMode(PINO_BOTAO, INPUT_PULLUP);
  Serial.println("\nbotão GPIO configurado no pino: " + String(PINO_BOTAO));

  // inicialização do gerenciador de sensores
  gerenciadorSensores.iniciar(); // olha sensores disponíveis

  gerenciadorSensores.imprimirStatus(); // mostra o status dos sensores

  Serial.println("\nconfigurações concluídas!");
  Serial.println("------------------------------------------------");
}

void loop()
{
  // ESTADO: ACORDADO - SIMULAÇÃO DO DEEP SLEEP
  if (!esta_dormindo)
  {
    Serial.println("[modo]: leitura de sensores");

    // PASSO 1: ler os sensores
    Serial.println("\nbuscanco sensores...");
    DadosSensores dados = gerenciadorSensores.lerSensores();

    // PASSO 2: mostrar dados no console serial
    Serial.println("\n[i] dados obtidos:");
    Serial.print("  timestamp: ");
    Serial.println(dados.timestamp_leitura);

    if (dados.temperatura_valida)
    {
      Serial.print("  temperatura: ");
      Serial.print(dados.temperatura);
      Serial.println(" °C");
    }
    else
    {
      Serial.println("  temperatura: indisponível");
    }

    if (dados.luminosidade_valida)
    {
      Serial.print("  luminosidade: ");
      Serial.print(dados.luminosidade);
      Serial.println(" lux");
    }
    else
    {
      Serial.println("  luminosidade: indisponível");
    }
    //**********************************************************
    // PASSO 3: TO DOOOOOO (permanência de dados + checksun)
    //**********************************************************

    Serial.println("\nleitura finalizada");
    Serial.println("------------------------------------------------");

    // PASSO 4: entrar em modo deep sleep simulado
    Serial.println("[modo]: preparação para baixo consumo");
    Serial.println("\ndefinindo timer deep sleep para " + String(TEMPO_DEEP_SLEEP_DEMO / 1000) + " segundos");
    esta_dormindo = true;
    tempo_inicio_sono = millis();
    Serial.println("clico de sono pode ser interrompido pressionando");
    Serial.println("o botão GPIO conectado ao pino " + String(PINO_BOTAO));
    Serial.println("------------------------------------------------");
    Serial.println("[status]: deep sleep zZz");
  }

  // ESTADO: DORMINDO - SIMULAÇÃO DO DEEP SLEEP

  else
  {
    // PASSO 1: verificar se tempo de sonou terminou
    if (millis() - tempo_inicio_sono >= TEMPO_DEEP_SLEEP_DEMO)
    {
      esta_dormindo = false;
      Serial.println("[status]: wake-up programado");
      Serial.println("------------------------------------------------");
    }

    // PASSO 2: verificar se botão foi pressionado
    if (digitalRead(PINO_BOTAO) == LOW)
    {
      esta_dormindo = false;
      Serial.println("[status]: wake-up manual");
      Serial.println("------------------------------------------------");
      delay(300); // debounce simples

      // esperar botão ser solto
      while (digitalRead(PINO_BOTAO) == LOW)
      {
        delay(50);
      }

      // reinicia o contador do sono
      tempo_inicio_sono = millis();
    }
  }
  delay(200);
}