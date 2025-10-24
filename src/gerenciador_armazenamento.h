#ifndef GERENCIADOR_ARMAZENAMENTO_H
#define GERENCIADOR_ARMAZENAMENTO_H

#include "config.h"
#include "Arduino.h"
#include "gerenciador_sensores.h"
#include "gerenciador_time.h"

// BIBLIOTECAS LittleFS

#ifdef AMBIENTE_WOKWI
// wokwi: simulacao
#include <FS.h>
#else
// esp32 fisico: LittleFS real
#include "LittleFS.h"
#endif

// ESTRUTURA PARA REGISTRO COMPLETO

struct RegistroDados
{
    DadosTempo tempo;
    DadosSensores sensores;
    uint32_t checksum;
};

// CLASSE GERENCIADOR ARMAZENAMENTO

class GerenciadorArmazenamento
{
private:
    bool sistema_arquivos_inicializado;
    const char *nome_arquivo = "/dados_log.csv";

    uint32_t calcularChecksum(const RegistroDados &registro)
    {
        uint32_t checksum = 0;
        checksum += (uint32_t)registro.tempo.epoch;
        checksum += (uint32_t)(registro.sensores.temperatura * 100);
        checksum += (uint32_t)(registro.sensores.luminosidade * 100);
        checksum += registro.sensores.timestamp_leitura;
        return checksum;
    }

    String formatarRegistroCSV(const RegistroDados &registro)
    {
        String linha = "";
        linha += String(registro.tempo.epoch) + ",";
        linha += String(registro.tempo.data_hora) + ",";
        linha += String(registro.sensores.temperatura, 2) + ",";
        linha += String(registro.sensores.luminosidade, 2) + ",";
        linha += String(registro.sensores.temperatura_valida ? "1" : "0") + ",";
        linha += String(registro.sensores.luminosidade_valida ? "1" : "0") + ",";
        linha += String(registro.checksum);
        return linha;
    }

public:
    GerenciadorArmazenamento()
    {
        sistema_arquivos_inicializado = false;
    }

    // METODOS EXISTENTES (mantidos iguais)

    bool iniciar()
    {
        Serial.println("\ninicializando LittleFS...");

#ifdef AMBIENTE_WOKWI
        Serial.println("wokwi: sistema de arquivos simulado");
        sistema_arquivos_inicializado = true;
#else
        if (!LittleFS.begin(true))
        {
            Serial.println("falha ao montar LittleFS");
            return false;
        }
        Serial.println("LittleFS montado com sucesso");
        sistema_arquivos_inicializado = true;
#endif

        criarCabecalho();
        return true;
    }

    bool salvarRegistro(const DadosTempo &tempo, const DadosSensores &sensores)
    {
        if (!sistema_arquivos_inicializado)
        {
            Serial.println("LittleFS nao inicializado");
            return false;
        }

        Serial.println("\nsalvando registro...");

        RegistroDados registro;
        registro.tempo = tempo;
        registro.sensores = sensores;
        registro.checksum = calcularChecksum(registro);

        String linha_csv = formatarRegistroCSV(registro);

#ifdef AMBIENTE_WOKWI
        Serial.println("registro CSV (simulado):");
        Serial.println("   " + linha_csv);
        return true;
#else
        File arquivo = LittleFS.open(nome_arquivo, "a");
        if (!arquivo)
        {
            Serial.println("falha ao abrir arquivo");
            return false;
        }
        if (arquivo.size() == 0)
        {
            arquivo.println("timestamp,data_hora,temperatura,luminosidade,temp_valida,lux_valida,checksum");
        }
        arquivo.println(linha_csv);
        arquivo.close();
        Serial.println("dados salvos no LittleFS");
        return true;
#endif
    }

    void criarCabecalho()
    {
#ifndef AMBIENTE_WOKWI
        if (!LittleFS.exists(nome_arquivo))
        {
            File arquivo = LittleFS.open(nome_arquivo, "w");
            if (arquivo)
            {
                arquivo.println("timestamp,data_hora,temperatura,luminosidade,temp_valida,lux_valida,checksum");
                arquivo.close();
                Serial.println("cabecalho criado no arquivo");
            }
        }
#endif
    }

    bool verificarIntegridade(const RegistroDados &registro)
    {
        uint32_t checksum_calculado = calcularChecksum(registro);
        bool integro = (checksum_calculado == registro.checksum);
        Serial.print("verificacao de integridade: ");
        Serial.println(integro ? "OK" : "CORROMPIDO");
        return integro;
    }

    void listarArquivos()
    {
        Serial.println("\narquivos no LittleFS:");
#ifdef AMBIENTE_WOKWI
        Serial.println("   [simulacao wokwi]");
        Serial.println("   /dados_log.csv");
#else
        File root = LittleFS.open("/");
        File arquivo = root.openNextFile();
        while (arquivo)
        {
            Serial.print("   ");
            Serial.print(arquivo.name());
            Serial.print(" (");
            Serial.print(arquivo.size());
            Serial.println(" bytes)");
            arquivo = root.openNextFile();
        }
#endif
    }

    // METODOS NOVOS - LEITURA E CONTROLE DE UPLOAD

    /**
     * verifica se existem dados pendentes para upload
     */
    bool existemDadosPendentes()
    {
#ifdef AMBIENTE_WOKWI
        // wokwi: sempre retorna true para teste
        return true;
#else
        if (!sistema_arquivos_inicializado)
            return false;

        File arquivo = LittleFS.open(nome_arquivo, "r");
        if (!arquivo)
            return false;

        // pula cabecalho
        arquivo.readStringUntil('\n');

        // verifica se tem dados
        bool tem_dados = arquivo.available() > 0;
        arquivo.close();

        return tem_dados;
#endif
    }

    /**
     * le dados do arquivo para enviar no upload
     */
    String lerDadosParaUpload()
    {
        Serial.println("lendo dados do arquivo para upload...");

#ifdef AMBIENTE_WOKWI
        // wokwi: dados de exemplo
        String dados = "1705339825,2024-01-15 10:30:25,22.50,550.00,1,1,123456789";
        Serial.println("dados para upload (simulados): " + dados);
        return dados;
#else
        if (!sistema_arquivos_inicializado)
        {
            Serial.println("erro: LittleFS nao inicializado");
            return "";
        }

        File arquivo = LittleFS.open(nome_arquivo, "r");
        if (!arquivo)
        {
            Serial.println("erro: nao foi possivel abrir arquivo");
            return "";
        }

        String dados = "";

        // pula cabecalho
        arquivo.readStringUntil('\n');

        // le todas as linhas de dados
        while (arquivo.available())
        {
            String linha = arquivo.readStringUntil('\n');
            linha.trim();
            if (linha.length() > 0)
            {
                if (dados.length() > 0)
                {
                    dados += ";"; // separador entre registros
                }
                dados += linha;
            }
        }
        arquivo.close();

        Serial.println("dados lidos: " + String(dados.length()) + " caracteres");
        return dados;
#endif
    }

    /**
     * marca dados como enviados (limpa o arquivo)
     */
    bool marcarComoEnviado()
    {
        Serial.println("marcando dados como enviados...");

#ifdef AMBIENTE_WOKWI
        Serial.println("dados marcados como enviados (simulacao)");
        return true;
#else
        if (!sistema_arquivos_inicializado)
        {
            Serial.println("erro: LittleFS nao inicializado");
            return false;
        }

        File arquivo = LittleFS.open(nome_arquivo, "w");
        if (!arquivo)
        {
            Serial.println("erro: nao foi possivel limpar arquivo");
            return false;
        }

        // recria apenas o cabecalho
        arquivo.println("timestamp,data_hora,temperatura,luminosidade,temp_valida,lux_valida,checksum");
        arquivo.close();

        Serial.println("arquivo limpo - dados marcados como enviados");
        return true;
#endif
    }
};

#endif