#ifndef GERENCIAODR_WIFI_H
#define GERENCIAODR_WIFI_H

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

    bool conectar()
    {
        Serial.println("\ntentando conectar WiFi...");

#ifdef AMBIENTE_WOKWI
        // no wokwi: simula conexão
        Serial.println("WOKWI: simulando conexão WiFi");
        delay(1000);
        wifi_conectado = true;
        Serial.println("WiFi simulado conectado");
        return true;
#else
        // no físico: conexão real
        WiFi.begin("SUA_REDE", "SUA_SENHA"); // Você coloca suas credenciais depois

        Serial.print("⏳ Conectando");
        for (int i = 0; i < 10; i++)
        {
            if (WiFi.status() == WL_CONNECTED)
            {
                wifi_conectado = true;
                Serial.println("\n✅ WiFi conectado!");
                Serial.print("📶 IP: ");
                Serial.println(WiFi.localIP());
                return true;
            }
            delay(1000);
            Serial.print(".");
        }

        Serial.println("\n❌ Falha ao conectar WiFi");
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