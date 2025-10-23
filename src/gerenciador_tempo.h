#ifndef TIME_MANAGER_H
#define TIME_MANAGER_H

#include "config.h"
#include "Arduino.h"
#include <WiFi.h>

// ESTRUTURA PARA DADOS DE TEMPO

struct DadosTempo
{
    unsigned long epoch;               // timestamp unix
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
        // no ESP32 físico - sincronização real
        configTime(gmt_offset_sec, daylight_offset_sec, ntp_server);

        // aguarda a sincronização (máximo 10 segundos)
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

    // converte epoch para string legível
    void epochParaString(unsigned long epoch, char *buffer, size_t tamanho)
    {
        time_t rawtime = epoch;
        struct tm *timeinfo = localtime(&rawtime);

        strftime(buffer, tamanho, "%Y-%m-%d %H:%M:%S", timeinfo);
    }

    // MÉTODOS PÚBLICOS

public:
    DadosTempo obterTempo()
    {
        DadosTempo tempo;

        if (!tempo_inicializado)
        {
            Serial.println("  tempo não inicializado: chamando iniciar()...");
            // iniciar();
        }

        tempo.sincronizado = true;

#ifdef AMBIENTE_WOKWI
        // no Wokwi, sempre tem tempo sincronizado
        time_t agora;
        time(&agora);
        tempo.epoch = agora;
        tempo.millis_sincronizado = millis();

#else
        // No físico, tenta obter tempo real
        struct tm timeinfo;
        if (getLocalTime(&timeinfo))
        {
            // Tempo NTP disponível
            time_t agora;
            time(&agora);
            tempo.epoch = agora;
            tempo.millis_sincronizado = millis();
            tempo.sincronizado = true;

            // atualiza fallback
            epoch_fallback = agora;
            ultima_sincronizacao = millis();
        }
        else
        {
            // NTP não disponível - estima baseado no último sync
            tempo.epoch = epoch_fallback + (millis() - ultima_sincronizacao) / 1000;
            tempo.millis_sincronizado = millis();
            tempo.sincronizado = false;
        }
#endif

        // converte para string legível
        epochParaString(tempo.epoch, tempo.data_hora, sizeof(tempo.data_hora));

        return tempo;
    }
};

#endif