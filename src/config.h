#ifndef CONFIG_H
#define CONFIG_H

#include "Arduino.h"

// CONFIGURAÇÕES PARA DETECÇÃO DE AMBIENTE

// Detecta automaticamente se esta no Wokwi
#ifdef __WOKWI__
#define AMBIENTE_WOKWI true
#define AMBIENTE_FISICO false
#pragma message "🔧 Compilando para WOKWI (Simulação)"
#else
#define AMBIENTE_WOKWI false
#define AMBIENTE_FISICO true
#pragma message "🔧 Compilando para ESP32 FÍSICO"
#endif

// CONFIGURAÇÕES DE PINOS

// Pinos para sensores e botões
#define PINO_BOTAO 33        // GPIO para botão de wake-up
#define PINO_TERMISTOR 35    // GPIO para leitura do NTC
#define PINO_FOTORESISTOR 34 // GPIO para leitura do LDR

// CONFIGURAÇÕES DE TEMPO

// (Tempos em milissegundos)
#define TEMPO_AMOSTRAGEM_MS 30000    // 30 segundos entre leituras (para testes)
#define TEMPO_AMOSTRAGEM_REAL 300000 // 5 minutos (300000 ms) - versão final

// CONFIGURAÇÕES DE SENSORES

// Parâmetros dos sensores
const float BETA_TERMISTOR = 3950.0; // Coeficiente Beta do termistor NTC
const float GAMA_LDR = 0.7;          // Coeficiente Gama do LDR
const float RESISTENCIA_LDR = 33.0;  // Resistência do LDR em 10 lux

#endif