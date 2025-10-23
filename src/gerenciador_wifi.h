#ifndef GERENCIADOR_WIFI_H
#define GERENCIADOR_WIFI_H

#include "config.h"
#include "Arduino.h"
#include <WiFi.h>

class GerenciadorWiFi
{
private:
    bool wifi_conectado;
    unsigned long ultima_tentativa;

public:
    GerenciadorWiFi()
    {
        wifi_conectado = false;
        ultima_tentativa = 0;
    }
    // tenta conectar ao WiFi (simples)

    // wifi_manager.h - ATUALIZAR O MÉTODO conectar()
    bool conectar()
    {
        Serial.println("\ntentando conectar wifi...");

#ifdef AMBIENTE_WOKWI
        // wokwi: simulacao
        Serial.println("wokwi: simulando conexao wifi");
        delay(1000);
        wifi_conectado = true;
        Serial.println("wifi simulado conectado");
        return true;
#else
        // esp32 fisico: conexao real com credenciais do config.h
        WiFi.begin(WIFI_SSID, WIFI_SENHA);

        Serial.print("conectando a rede: ");
        Serial.print(WIFI_SSID);
        Serial.print(" ...");

        for (int i = 0; i < 15; i++)
        {
            if (WiFi.status() == WL_CONNECTED)
            {
                wifi_conectado = true;
                Serial.println("\nwifi conectado!");
                Serial.print("ip: ");
                Serial.println(WiFi.localIP());
                return true;
            }
            delay(1000);
            Serial.print(".");
        }

        Serial.println("\nfalha ao conectar wifi");
        wifi_conectado = false;
        return false;
#endif
    }

    /**
     * Verifica se está conectado
     */
    bool estaConectado()
    {
#ifdef AMBIENTE_WOKWI
        return wifi_conectado; // No Wokwi retorna o estado simulado
#else
        return WiFi.status() == WL_CONNECTED; // No físico verifica real
#endif
    }

    /**
     * Faz upload SIMULADO dos dados
     */
    bool enviarDados(const String &dados_csv)
    {
        if (!estaConectado())
        {
            Serial.println("❌ Sem WiFi para enviar dados");
            return false;
        }

        Serial.println("\n📤 Enviando dados para servidor...");

#ifdef AMBIENTE_WOKWI
        // No Wokwi: Simula upload
        Serial.println("🔮 WOKWI: Simulando upload HTTP");
        Serial.println("📄 Dados que seriam enviados:");
        Serial.println(dados_csv);
        delay(500);
        Serial.println("✅ Upload simulado com sucesso!");
        return true;
#else
        // No físico: Upload real (implementaremos depois)
        Serial.println("📡 ESP32: Preparado para upload real");
        // Aqui virá o código HTTP real depois
        return true;
#endif
    }
};

#endif