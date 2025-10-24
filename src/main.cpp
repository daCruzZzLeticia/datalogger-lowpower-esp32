#include "config.h"
#include "gerenciador_armazenamento.h"
#include "gerenciador_sensores.h"
#include "gerenciador_sleep.h"
#include "gerenciador_time.h"
#include "gerenciador_upload.h"
#include "gerenciador_wifi.h"
#include "Arduino.h"

// gerenciadores do sistema
GerenciadorArmazenamento gerenciadorArmazenamento;
GerenciadorSensores gerenciadorSensores;
GerenciadorSleep gerenciadorSleep;
GerenciadorTempo gerenciadorTempo;
GerenciadorWiFi gerenciadorWiFi;
GerenciadorUpload gerenciadorUpload;

// controle de sleep simulado
bool esta_dormindo = false;
unsigned long tempo_inicio_sono = 0;

void setup()
{
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n[data logger] inicializando sistema");
  Serial.println("==========================================");

  // configura pino do botao
  pinMode(PINO_BOTAO, INPUT_PULLUP);
  Serial.println("configurado botao no pino: " + String(PINO_BOTAO));

  // inicializa todos os sistemas
  Serial.println("\ninicializando modulos:");

  Serial.print("- sensores: ");
  gerenciadorSensores.iniciar();
  Serial.println("pronto");

  Serial.print("- tempo: ");
  gerenciadorTempo.iniciar();
  Serial.println("pronto");

  Serial.print("- armazenamento: ");
  if (gerenciadorArmazenamento.iniciar())
  {
    Serial.println("pronto");
  }
  else
  {
    Serial.println("falha");
  }

  Serial.print("- wifi: ");
  gerenciadorWiFi.conectar();
  Serial.println(gerenciadorWiFi.estaConectado() ? "conectado" : "desconectado");

  // status do sistema
  Serial.println("\nstatus do sistema:");
  gerenciadorSensores.imprimirStatus();
  gerenciadorTempo.imprimirTempoAtual();
  gerenciadorArmazenamento.listarArquivos();

  Serial.println("==========================================");
  Serial.println("[data logger] sistema pronto para operacao");
  Serial.println("==========================================");
}

void loop()
{
  // estado: acordado - fazer leituras
  if (!esta_dormindo)
  {
    Serial.println("\n[data logger] iniciando ciclo de leitura");

    // obtem timestamp atual
    Serial.println("obtendo timestamp...");
    DadosTempo dados_tempo = gerenciadorTempo.obterTempo();

    // le dados dos sensores
    Serial.println("lendo sensores...");
    DadosSensores dados_sensores = gerenciadorSensores.lerSensores();

    // exibe dados coletados
    Serial.println("\ndados coletados:");
    Serial.println("  timestamp: " + String(dados_tempo.epoch) + " (" + String(dados_tempo.data_hora) + ")");

    if (dados_sensores.temperatura_valida)
    {
      Serial.println("  temperatura: " + String(dados_sensores.temperatura, 2) + " graus celsius");
    }
    else
    {
      Serial.println("  temperatura: sensor indisponivel");
    }

    if (dados_sensores.luminosidade_valida)
    {
      Serial.println("  luminosidade: " + String(dados_sensores.luminosidade, 2) + " lux");
    }
    else
    {
      Serial.println("  luminosidade: sensor indisponivel");
    }

    // salva dados no armazenamento
    Serial.println("\nsalvando dados...");
    if (gerenciadorArmazenamento.salvarRegistro(dados_tempo, dados_sensores))
    {
      Serial.println("dados salvos com sucesso");
    }
    else
    {
      Serial.println("falha ao salvar dados");
    }

    // verifica e envia dados pendentes
    Serial.println("\nverificando conexao para upload...");
    if (gerenciadorWiFi.estaConectado())
    {
      Serial.println("wifi disponivel - iniciando upload de dados");
      gerenciadorUpload.enviarComRetentativas(gerenciadorArmazenamento);
    }
    else
    {
      Serial.println("wifi indisponivel - dados mantidos localmente");
    }

// controle de sleep
#ifdef AMBIENTE_WOKWI
    Serial.println("\n[data logger] entrando em modo sleep");
    Serial.println("tempo de sleep: " + String(TEMPO_DEEP_SLEEP_DEMO / 1000) + " segundos");
    Serial.println("aguardando timer ou acionamento do botao...");
    esta_dormindo = true;
    tempo_inicio_sono = millis();
#else
    gerenciadorSleep.entrarDeepSleep();
#endif
  }

// estado: dormindo - apenas no wokwi
#ifdef AMBIENTE_WOKWI
  if (esta_dormindo)
  {
    // verifica se tempo de sleep acabou
    if (millis() - tempo_inicio_sono >= TEMPO_DEEP_SLEEP_DEMO)
    {
      esta_dormindo = false;
      Serial.println("\n[data logger] acordado por timer");
    }

    // verifica se botao foi pressionado
    if (digitalRead(PINO_BOTAO) == LOW)
    {
      esta_dormindo = false;
      Serial.println("\n[data logger] acordado por botao");
      delay(300);
      while (digitalRead(PINO_BOTAO) == LOW)
        delay(50);
      tempo_inicio_sono = millis();
    }

    // mostra sinal de atividade enquanto dorme
    static unsigned long ultimo_pisca = 0;
    if (millis() - ultimo_pisca > 1000)
    {
      Serial.print(".");
      ultimo_pisca = millis();
    }
  }
#endif

  delay(100);
}