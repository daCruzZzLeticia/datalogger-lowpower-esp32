#ifndef TIME_MANAGER_H
#define TIME_MANAGER_H

#include "config.h"
#include "Arduino.h"
#include <WiFi.h>

// ESTRUTURA PARA DADOS DE TEMPO

struct DadosTempo
{
    unsigned long timestamp_unix;      // timestamp unix
    unsigned long millis_sincronizado; // millis() quando foi sincronizado
    bool sincronizado;                 // se o tempo está sincronizado com NTP
    char data_hora[20];                // string no formato "2024-01-15 14:30:25"
};

#endif