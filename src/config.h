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

#endif