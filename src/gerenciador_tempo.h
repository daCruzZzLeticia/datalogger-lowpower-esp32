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

    // sincroniza com servidor NTP
    bool sincronizarNTP()
    {
        Serial.println("tentando sincronizar com NTP...");

#ifdef AMBIENTE_WOKWI

        // no Wokwi, simula uma sincronização
        Serial.println("wokwi: simulando sincronização NTP");
        configTime(gmt_offset_sec, daylight_offset_sec, ntp_server);

        // no Wokwi, sempre sincroniza
        Serial.println("sincronização NTP simulada com sucesso");
        return true;

#else
        // No ESP32 físico - sincronização real
        configTime(gmt_offset_sec, daylight_offset_sec, ntp_server);

        // Aguarda a sincronização (máximo 10 segundos)
        Serial.print("aguardando sincronização NTP");
        for (int i = 0; i < 20; i++)
        {
            delay(500);
            Serial.print(".");

            struct tm timeinfo;
            if (getLocalTime(&timeinfo))
            {
                Serial.println("\nsincronização NTP realizada com sucesso!");
                return true;
            }
        }

        Serial.println("\nfalha na sincronização NTP");
        return false;
#endif
    }
};

#endif