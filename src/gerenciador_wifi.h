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
    /*
     * construtor - inicializa o gerenciador wifi
     */
    GerenciadorWiFi()
    {
        wifi_conectado = false;
        ultima_tentativa = 0;
    }

    /*
     * tenta conectar ao wifi nos dois ambientes
     * no wokwi: usa rede real "Wokwi-GUEST"
     * no fisico: usa credenciais do config.h
     */
    bool conectar()
    {
        Serial.println("\nconectando ao wifi...");

#ifdef AMBIENTE_WOKWI
        // wokwi: conexao real com rede simulada do wokwi
        Serial.println("wokwi: usando rede Wokwi-GUEST");
        WiFi.begin("Wokwi-GUEST", "", 6); // canal 6 para conexao rapida

        Serial.print("conectando");
        for (int i = 0; i < 15; i++)
        {
            if (WiFi.status() == WL_CONNECTED)
            {
                wifi_conectado = true;
                Serial.println("\nwokwi: wifi conectado!");
                Serial.print("endereco ip: ");
                Serial.println(WiFi.localIP());
                return true;
            }
            delay(500);
            Serial.print(".");
        }

        Serial.println("\n[!] wokwi: falha ao conectar wifi");
        wifi_conectado = false;
        return false;

#else
        // esp32 fisico: conexao real com credenciais do config.h
        Serial.print("conectando a rede: ");
        Serial.print(WIFI_SSID);
        Serial.print(" ...");

        WiFi.begin(WIFI_SSID, WIFI_SENHA);

        for (int i = 0; i < 20; i++) // mais tentativas no fisico
        {
            if (WiFi.status() == WL_CONNECTED)
            {
                wifi_conectado = true;
                Serial.println("\nwifi conectado!");
                Serial.print("endereco ip: ");
                Serial.println(WiFi.localIP());
                return true;
            }
            delay(1000);
            Serial.print(".");
        }

        Serial.println("\n[!] falha ao conectar wifi");
        wifi_conectado = false;
        return false;
#endif
    }

    /*
     * verifica se esta conectado ao wifi
     * em ambos ambientes verifica o status real da conexao
     */
    bool estaConectado()
    {
#ifdef AMBIENTE_WOKWI
        // wokwi: verifica status real da conexao
        return WiFi.status() == WL_CONNECTED;
#else
        // fisico: verifica status real da conexao
        return WiFi.status() == WL_CONNECTED;
#endif
    }

    /*
     * envia dados via http (apenas verificacao de conexao)
     * o upload real e feito pelo gerenciador_upload.h
     */
    bool verificarConexaoUpload()
    {
        if (!estaConectado())
        {
            Serial.println("[!] sem conexao wifi para enviar dados");
            return false;
        }

        Serial.println("\nverificando conexao para upload...");

#ifdef AMBIENTE_WIFI
        // wokwi: simula verificacao de conexao
        Serial.println("wokwi: conexao wifi verificada - pronto para upload");
        Serial.println("servidor: " + String(SERVIDOR_URL));
        return true;
#else
        // fisico: verifica conexao real
        Serial.println("conexao wifi verificada - pronto para upload");
        Serial.println("servidor: " + String(SERVIDOR_URL));
        return true;
#endif
    }

    /*
     * retorna o endereco ip atual (para debug)
     */
    String obterIP()
    {
        if (estaConectado())
        {
            return WiFi.localIP().toString();
        }
        return "desconectado";
    }

    /*
     * retorna a forca do sinal wifi em dBm
     */
    int obterForcaSinal()
    {
        if (estaConectado())
        {
            return WiFi.RSSI();
        }
        return 0;
    }
};

#endif