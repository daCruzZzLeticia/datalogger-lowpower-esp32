// sleep_manager.h
#ifndef SLEEP_MANAGER_H
#define SLEEP_MANAGER_H

#include "config.h"
#include "Arduino.h"

class GerenciadorSleep
{
public:
    /**
     * configura e entra em deep sleep
     */
    void entrarDeepSleep()
    {
        Serial.println("\nentrando em deep sleep...");

#ifdef AMBIENTE_WOKWI
        // wokwi: simulacao
        Serial.println("wokwi: deep sleep simulado por " +
                       String(TEMPO_DEEP_SLEEP_DEMO / 1000) + " segundos");
        Serial.println("acordara por timer ou botao pino " + String(PINO_BOTAO));
        // no wokwi: nao faz nada - o loop principal cuida da simulacao

#else
        // esp32 fisico: deep sleep real
        Serial.println("esp32: configurando deep sleep real");

        // 1. configura wake-up por timer (5 minutos em microssegundos)
        esp_sleep_enable_timer_wakeup(TEMPO_AMOSTRAGEM_REAL * 1000);

        // 2. configura wake-up por botao
        esp_sleep_enable_ext0_wakeup((gpio_num_t)PINO_BOTAO, 0); // LOW acorda

        Serial.println("wake-up configurado:");
        Serial.println("   - timer: " + String(TEMPO_DEEP_SLEEP_DEMO / 1000) + " minutos");
        Serial.println("   - botao: pino " + String(PINO_BOTAO));

        // 3. desliga wifi/bluetooth para economizar
        WiFi.disconnect(true);
        WiFi.mode(WIFI_OFF);

        Serial.println("wifi desligado");
        Serial.println("indo dormir...");
        delay(100); // espera mensagens serem enviadas

        // 4. entra em deep sleep real
        esp_deep_sleep_start();

#endif
    }

    // chamado ao acordar - verifica motivo do wake-up

    void aoAcordar()
    {
#ifdef AMBIENTE_WOKWI
        // wokwi: ja e tratado no loop principal
        return;
#else
        // esp32 fisico: identifica motivo do wake-up
        esp_sleep_wakeup_cause_t causa = esp_sleep_get_wakeup_cause();

        Serial.println("\nacordei!");
        Serial.print("motivo: ");

        switch (causa)
        {
        case ESP_SLEEP_WAKEUP_TIMER:
            Serial.println("timer (5 minutos)");
            break;
        case ESP_SLEEP_WAKEUP_EXT0:
            Serial.println("botao (wake-up manual)");
            break;
        default:
            Serial.println("desconhecido: " + String(causa));
            break;
        }
#endif
    }
};

#endif