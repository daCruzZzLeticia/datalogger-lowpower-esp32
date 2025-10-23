#include "config.h"
#include "gerenciador_armazenamento.h"
#include "gerenciador_sensores.h"
#include "gerenciador_sleep.h"
#include "gerenciador_time.h"
#include "gerenciador_upload.h"
#include "gerenciador_wifi.h"
#include "Arduino.h"

// cria o gerenciador de sensores
GerenciadorArmazenamento gerenciadorArmazenamento;
GerenciadorSensores gerenciadorSensores;
GerenciadorSleep gerenciadorSleep;
GerenciadorTempo gerenciadorTempo;
GerenciadorWiFi gerenciadorWiFi;
GerenciadorUpload gerenciadorUpload;

// estado de deep sleep simulado
bool esta_dormindo = false;
unsigned long tempo_inicio_sono = 0;

void setup()
{
  Serial.begin(115200);
  delay(2000);

  Serial.println("\n🔍 VERIFICANDO AMBIENTE:");
  Serial.print("AMBIENTE_WOKWI: ");
  Serial.println(AMBIENTE_WOKWI ? "SIM" : "NÃO");
  Serial.print("AMBIENTE_FISICO: ");
  Serial.println(AMBIENTE_FISICO ? "SIM" : "NÃO");

#ifdef __WOKWI__
  Serial.println("__WOKWI__ definido: SIM");
#else
  Serial.println("__WOKWI__ definido: NÃO");
#endif

  delay(5000);

  Serial.println("------------------------------------------------");
  Serial.println("sistema data logger - temperatura e luminosidade");
  Serial.println("------------------------------------------------");

  Serial.println("[modo]: configuração do sistema");

  // configura o pino do botão para wake-up
  pinMode(PINO_BOTAO, INPUT_PULLUP);
  Serial.println("\nbotão GPIO configurado no pino: " + String(PINO_BOTAO));

  // passo 1. inicialização do gerenciador de sensores
  gerenciadorSensores.iniciar(); // olha sensores disponíveis

  // passo 2. inicializa sistema de tempo NTP + RTC fallback
  gerenciadorTempo.iniciar();

  // passo 3. inicializa LittleFS (sistema de arquivos)
  if (!gerenciadorArmazenamento.iniciar())
  {
    Serial.println("ERRO: LittleFS não pôde ser inicializado");
  }
  // tenta conectar wi-fi
  gerenciadorWiFi.conectar();

  // mostra status completo do sistema
  Serial.println("\n📊 STATUS DO SISTEMA:");
  gerenciadorSensores.imprimirStatus();
  gerenciadorTempo.imprimirTempoAtual();
  gerenciadorArmazenamento.listarArquivos();

  Serial.println("\nconfigurações concluídas!");
  Serial.println("------------------------------------------------");
}

void loop()
{
  // ESTADO: ACORDADO - SIMULAÇÃO DO DEEP SLEEP
  if (!esta_dormindo)
  {
    Serial.println("[modo]: leitura de sensores");

    // passo 1: obter timestamp atual
    Serial.println("obtendo timestamp...");
    DadosTempo dados_tempo = gerenciadorTempo.obterTempo();

    // passo 2: ler os sensores
    Serial.println("\nbuscanco sensores...");
    DadosSensores dados_sensores = gerenciadorSensores.lerSensores();

    // passo 3: mostrar dados no console serial
    Serial.println("\n[i] dados obtidos:");

    Serial.print("  timestamp: ");
    Serial.print(dados_tempo.epoch);
    Serial.print(" (");
    Serial.print(dados_tempo.data_hora);
    Serial.println(")");

    if (dados_sensores.temperatura_valida)
    {
      Serial.print("  temperatura: ");
      Serial.print(dados_sensores.temperatura);
      Serial.println(" °C");
    }
    else
    {
      Serial.println("  temperatura: indisponível");
    }

    if (dados_sensores.luminosidade_valida)
    {
      Serial.print("  luminosidade: ");
      Serial.print(dados_sensores.luminosidade);
      Serial.println(" lux");
    }
    else
    {
      Serial.println("  luminosidade: indisponível");
    }

    // passo 4: permanecer de dados + checksun
    Serial.println("\nsalvando dados no LittleFS...");
    bool salvou = gerenciadorArmazenamento.salvarRegistro(dados_tempo, dados_sensores);

    if (salvou)
    {
      Serial.println("dados salvos com sucesso!");
    }
    else
    {
      Serial.println("falha ao salvar dados!");
    }

    if (gerenciadorWiFi.estaConectado())
    {
      Serial.println("wifi disponivel - tentando enviar dados pendentes");
      gerenciadorUpload.enviarDadosPendentes();
    }
    else
    {
      Serial.println("sem wifi - dados mantidos localmente");
    }

    gerenciadorSleep.entrarDeepSleep();
  }

  delay(200);
}