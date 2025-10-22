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

class GerenciadorTempo
{
private:
    // configurações NTP
    const char *ntp_server = "pool.ntp.org";
    const long gmt_offset_sec = -3 * 3600; // GMT-3 (Brasília)
    const int daylight_offset_sec = 0;

    bool tempo_inicializado;
    unsigned long ultima_sincronizacao;
    unsigned long epoch_fallback; // usado se NTP falha
};

#endif