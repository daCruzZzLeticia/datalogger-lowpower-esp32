#ifndef GERENCIADOR_TIME_H
#define GERENCIADOR_TIME_H

#include "config.h"
#include "Arduino.h"
#include <WiFi.h>

// ESTRUTURA PARA DADOS DE TEMPO

struct DadosTempo
{
    unsigned long epoch;               // timestamp unix
    unsigned long millis_sincronizado; // millis() quando foi sincronizado
    bool sincronizado;                 // se tempo está sincronizado com NTP
    char data_hora[20];                // string no formato "2024-01-15 14:30:25"
};

// CLASSE DO GERENCIADOR DE TEMPO

class GerenciadorTempo
{
private:
    // configurações NTP
    const char *ntp_server = "pool.ntp.org";
    const long gmt_offset_sec = -3 * 3600; // GMT-3 (Brasília)
    const int daylight_offset_sec = 0;

    bool tempo_inicializado;
    unsigned long ultima_sincronizacao;
    unsigned long epoch_fallback; // se NTP falhar

    // MÉTODOS PRIVADOS

    // sincronizar com servidor NTP
    bool sincronizarNTP()
    {
        Serial.println("tentando sincronizar com NTP...");

#ifdef AMBIENTE_WOKWI
        // no Wokwi, simula uma sincronização
        Serial.println("wokwi: simulando sincronização NTP");
        configTime(gmt_offset_sec, daylight_offset_sec, ntp_server);

        // simula rede  (DEIXAR?)
        delay(1000);

        // no wokwi, sempre sincroniza
        Serial.println("sincronização NTP simulada com sucesso");
        return true;

#else
        // ESP32 físico - sincronização real
        configTime(gmt_offset_sec, daylight_offset_sec, ntp_server);

        // aguarda a sincronização, máx 10 segundos
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

public:
    // MÉTODOS PÚBLICOS

    GerenciadorTempo()
    {
        tempo_inicializado = false;
        ultima_sincronizacao = 0;
        epoch_fallback = 0;
    }

    /**
     * inicializa o sistema de tempo
     * tenta NTP primeiro, depois usa fallback
     */

    void iniciar()
    {
        Serial.println("\ninicializando gerenciador de tempo...");

        // tenta sincronizar com NTP
        bool ntp_sucesso = sincronizarNTP();

        if (ntp_sucesso)
        {
            tempo_inicializado = true;
            ultima_sincronizacao = millis();

            // obtém o timestamp atual para usar como fallback
            time_t agora;
            time(&agora);
            epoch_fallback = agora;

            Serial.println("gerenciador de tempo inicializado (NTP)");
        }
        else
        {
            // se NTP falhou, usa tempo fallback
            tempo_inicializado = true;
            epoch_fallback = 1609459200;

            Serial.println("[!] gerenciador de tempo usando modo FALLBACK");
            Serial.println("        (sem internet - usando RTC interno)");
        }

        imprimirTempoAtual();
    }

    /**
     * Obtém o timestamp atual
     * Se NTP disponível, retorna tempo real
     * Se não, estima baseado no último sync + millis()
     */

    DadosTempo obterTempo()
    {
        DadosTempo tempo;

        if (!tempo_inicializado)
        {
            Serial.println("tempo não inicializado: chamando iniciar()...");
            iniciar();
        }

        tempo.sincronizado = true; // por enquanto

#ifdef AMBIENTE_WOKWI
        // o wokwi, sempre tem tempo sincronizado
        time_t agora;
        time(&agora);
        tempo.epoch = agora;
        tempo.millis_sincronizado = millis();

#else
        // no físico, tenta obter tempo real
        struct tm timeinfo;
        if (getLocalTime(&timeinfo))
        {
            // tempo NTP disponível
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

    // imprime o tempo atual no Serial (para debug)
    void imprimirTempoAtual()
    {
        DadosTempo tempo = obterTempo();

        Serial.println("\n[i] informações de tempo");
        Serial.print("  timestamp: ");
        Serial.println(tempo.epoch);
        Serial.print("  data/hora: ");
        Serial.println(tempo.data_hora);
        Serial.print("  sincronizado: ");
        Serial.println(tempo.sincronizado ? "SIM (NTP)" : "NÃO (RTC fallback)");

        if (!tempo.sincronizado)
        {
            Serial.println("usando estimativa baseada no RTC interno");
        }
    }
};

#endif