// upload_manager.h
#ifndef UPLOAD_MANAGER_H
#define UPLOAD_MANAGER_H

#include "config.h"
#include "Arduino.h"
#include <HTTPClient.h>
#include <WiFi.h>
#include "gerenciador_armazenamento.h" // 👈 ADICIONAR ESTE INCLUDE

class GerenciadorUpload
{
private:
    const char *servidor_url;
    bool upload_habilitado;
    const int max_tentativas = 3;            // 👈 MOVER PARA AQUI
    const int delay_entre_tentativas = 2000; // 👈 MOVER PARA AQUI

public:
    GerenciadorUpload() : servidor_url(SERVIDOR_URL)
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
    bool enviarDadosPendentes(GerenciadorArmazenamento &armazenamento)
    { // 👈 MÉTODO QUE ESTAVA FALTANDO
        Serial.println("verificando dados pendentes para upload...");

        // primeiro verifica se existem dados
        if (!armazenamento.existemDadosPendentes())
        {
            Serial.println("nenhum dado pendente encontrado");
            return true;
        }

        Serial.println("dados pendentes encontrados, lendo do arquivo...");

        // lê dados reais do arquivo
        String dados_reais = armazenamento.lerDadosParaUpload();

        if (dados_reais.length() == 0)
        {
            Serial.println("erro: nao foi possivel ler dados do arquivo");
            return false;
        }

        Serial.println("enviando " + String(dados_reais.length()) + " caracteres de dados");

        // envia dados
        bool sucesso = enviarDados(dados_reais);

        if (sucesso)
        {
            Serial.println("upload bem-sucedido - marcando como enviado");
            armazenamento.marcarComoEnviado(); // 👈 MARCA COMO ENVIADO
        }
        else
        {
            Serial.println("falha no upload - dados mantidos para retentativa");
        }

        return sucesso;
    }

    /**
     * envia dados com sistema de retentativas
     */
    bool enviarComRetentativas(GerenciadorArmazenamento &armazenamento)
    {
        Serial.println("iniciando envio com retentativas...");

        for (int tentativa = 1; tentativa <= max_tentativas; tentativa++)
        {
            Serial.println("tentativa " + String(tentativa) + " de " + String(max_tentativas));

            bool sucesso = enviarDadosPendentes(armazenamento);

            if (sucesso)
            {
                Serial.println("upload bem-sucedido na tentativa " + String(tentativa));
                return true;
            }

            // se não foi a última tentativa, espera e tenta novamente
            if (tentativa < max_tentativas)
            {
                Serial.println("aguardando " + String(delay_entre_tentativas / 1000) + " segundos para retentativa...");
                delay(delay_entre_tentativas);
            }
        }

        Serial.println("todas as " + String(max_tentativas) + " tentativas falharam");
        Serial.println("dados mantidos para proximo ciclo com wifi");
        return false;
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