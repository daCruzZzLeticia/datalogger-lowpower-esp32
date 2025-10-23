// upload_manager.h
#ifndef UPLOAD_MANAGER_H
#define UPLOAD_MANAGER_H

#include "config.h"
#include "Arduino.h"
#include <HTTPClient.h>
#include <WiFi.h>

class GerenciadorUpload
{
private:
    const char *servidor_url = "http://seuservidor.com/api/dados"; // ex. SUBSTITUIR DPS
    bool upload_habilitado;

public:
    GerenciadorUpload()
    {
        upload_habilitado = true;
    }

    /**
     * envia dados para o servidor via http post
     */

    bool enviarDados(const String &dados_csv)
    {
        if (!upload_habilitado)
        {
            Serial.println("upload desabilitado");
            return false;
        }

#ifdef AMBIENTE_WOKWI
        // wokwi: simulacao de upload
        Serial.println("enviando dados (simulacao wokwi)...");
        Serial.println("dados que seriam enviados:");
        Serial.println(dados_csv);
        delay(500);
        Serial.println("upload simulado com sucesso");
        return true;

#else
        // esp32 fisico: upload real
        if (WiFi.status() != WL_CONNECTED)
        {
            Serial.println("sem conexao wifi para upload");
            return false;
        }

        Serial.println("preparando upload http...");

        HTTPClient http;
        http.begin(servidor_url);
        http.addHeader("Content-Type", "application/json");

        // prepara dados no formato json
        String json_dados = "{\"dados\": \"" + dados_csv + "\"}";

        Serial.println("enviando para: " + String(servidor_url));
        int http_code = http.POST(json_dados);

        if (http_code > 0)
        {
            Serial.println("codigo http: " + String(http_code));
            if (http_code == HTTP_CODE_OK)
            {
                String resposta = http.getString();
                Serial.println("resposta do servidor: " + resposta);
                http.end();
                Serial.println("upload realizado com sucesso");
                return true;
            }
        }
        else
        {
            Serial.println("erro no upload: " + String(http_code));
        }

        http.end();
        return false;

#endif
    }

    /**
     * verifica se ha dados pendentes e envia em lote
     */

    bool enviarDadosPendentes()
    {
        Serial.println("verificando dados pendentes...");

        // dados simulados
        String dados_exemplo = "timestamp,data_hora,temperatura,luminosidade;1705339825,2024-01-15 10:30:25,22.5,550.0";

        return enviarDados(dados_exemplo);
    }

    /**
     * habilita/desabilita upload
     */
    void setUploadHabilitado(bool habilitado)
    {
        upload_habilitado = habilitado;
        Serial.println("upload " + String(habilitado ? "habilitado" : "desabilitado"));
    }
};

#endif