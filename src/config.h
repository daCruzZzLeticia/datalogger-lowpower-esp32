#ifndef CONFIG_H
#define CONFIG_H

#include "config_privado.h"
#include "Arduino.h"

// CONFIGURAÇÕES PARA DETECÇÃO DE AMBIENTE

// detecta automaticamente se esta no Wokwi
#ifdef __WOKWI__
#define AMBIENTE_WOKWI true
#define AMBIENTE_FISICO false
#pragma message "🔧 Ambiente: WOKWI (Simulação)"

// método 2: verifica variáveis de ambiente do PlatformIO
#elif defined(PLATFORMIO) && defined(ENV_WOKWI)
#define AMBIENTE_WOKWI true
#define AMBIENTE_FISICO false
#pragma message "🔧 Ambiente: WOKWI via PlatformIO"

// método 3: se não é Wokwi, assume que é físico
#else
#define AMBIENTE_WOKWI false
#define AMBIENTE_FISICO true
#pragma message "🔧 Ambiente: ESP32 FÍSICO"
#endif

// CONFIGURAÇÕES DE PINOS

// pinos para sensores e botões
#define PINO_BOTAO 33        // GPIO para botão de wake-up
#define PINO_TERMISTOR 35    // GPIO para leitura do NTC
#define PINO_FOTORESISTOR 34 // GPIO para leitura do LDR

// CONFIGURAÇÕES DE TEMPO

// (tempos em milissegundos)
#define TEMPO_DEEP_SLEEP_DEMO 30000      // 30 segundos entre leituras (para testes)
#define TEMPO_DEEP_SLEEP_COMPLETO 300000 // 5 minutos (300000 ms) - versão final

// CONFIGURAÇÕES DE SENSORES

// carâmetros dos sensores
const float BETA_TERMISTOR = 3950.0; // coeficiente Beta do termistor NTC
const float GAMA_LDR = 0.7;          // coeficiente Gama do LDR
const float RESISTENCIA_LDR = 33.0;  // resistência do LDR em 10 lux

// DECISÕES DE COMPORTAMENTO

// controle para sensores reais ou mocks
#define SENSORES_REAIS true // tentar ler sensores físicos/virtuais
#define SENSORES_MOCKS true // usar dados simulados se sensores falharem

// CONFIGURAÇÕES DE SERVIDOR

// URLs do servidor
#ifdef AMBIENTE_WOKWI
const char *SERVIDOR_URL = "http://localhost:3000/api/dados"; // teste local
#else
const char *SERVIDOR_URL = "https://seuserver.com/api/dados"; // servidor real
#endif

// configurações de upload
const int MAX_TENTATIVAS_UPLOAD = 3;
const int TIMEOUT_UPLOAD_MS = 10000;

#endif