#ifndef GERENCIADOR_ARMAZENAMENTO_H
#define GERENCIADOR_ARMAZENAMENTO_H

#include "config.h"
#include "Arduino.h"
#include "gerenciador_sensores.h"
#include "gerenciador_time.h"

// BIBLIOTECAS LittleFS - DIFERENTES PLATAFORMAS

#ifdef AMBIENTE_WOKWI
// No Wokwi, simulação do sistema de arquivos
#include <FS.h>
#pragma message "🔧 Wokwi: Usando FS simulado"
#else
// No ESP32 físico - LittleFS real
#include "LittleFS.h"
#pragma message "🔧 ESP32 Físico: Usando LittleFS"
#endif

// ESTRUTURA PARA UM REGISTRO COMPLETO

struct RegistroDados
{
    DadosTempo tempo;
    DadosSensores sensores;
    uint32_t checksum; // para verificar integridade
};

// CLASSE DO GERENCIADOR DE ARMAZENAMENTO LittleFS

class GerenciadorArmazenamento
{
private:
    bool sistema_arquivos_inicializado;
    const char *nome_arquivo = "/dados_log.csv";
    const char *nome_arquivo_pendente = "/dados_pendente.csv";

    // calcula checksum simples para verificar integridade

    uint32_t calcularChecksum(const RegistroDados &registro)
    {
        uint32_t checksum = 0;

        // Usa os dados principais para calcular checksum
        checksum += (uint32_t)registro.tempo.epoch;
        checksum += (uint32_t)(registro.sensores.temperatura * 100);
        checksum += (uint32_t)(registro.sensores.luminosidade * 100);
        checksum += registro.sensores.timestamp_leitura;

        return checksum;
    }

    // formata um registro para salvar em CSV

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

    // inicializa o LittleFS (ou simulação no Wokwi)

    bool iniciar()
    {
        Serial.println("\ninicializando LittleFS...");

#ifdef AMBIENTE_WOKWI
        // No Wokwi, simulação do sistema de arquivos
        Serial.println("wokwi: Sistema de arquivos simulado (LittleFS)");
        sistema_arquivos_inicializado = true;

#else
        // No ESP32 físico - LittleFS real
        if (!LittleFS.begin(true))
        { // true = formatar se falhar
            Serial.println("falha ao montar LittleFS");
            sistema_arquivos_inicializado = false;
            return false;
        }

        Serial.println("LittleFS montado com sucesso");

        // mostra informações do sistema de arquivos
        Serial.print("   espaço total: ");
        Serial.print(LittleFS.totalBytes() / 1024.0);
        Serial.println(" KB");

        Serial.print("   espaço usado: ");
        Serial.print(LittleFS.usedBytes() / 1024.0);
        Serial.println(" KB");

        sistema_arquivos_inicializado = true;
#endif

        // cria o cabeçalho do arquivo se for a primeira vez
        criarCabecalho();

        return true;
    }

    // salva um registro de dados no arquivo LittleFS

    bool salvarRegistro(const DadosTempo &tempo, const DadosSensores &sensores)
    {
        if (!sistema_arquivos_inicializado)
        {
            Serial.println("LittleFS não inicializado");
            return false;
        }

        Serial.println("\nsalvando registro no LittleFS...");

        // cria estrutura completa
        RegistroDados registro;
        registro.tempo = tempo;
        registro.sensores = sensores;
        registro.checksum = calcularChecksum(registro);

        // formata para CSV
        String linha_csv = formatarRegistroCSV(registro);

#ifdef AMBIENTE_WOKWI
        // no Wokwi, simula salvamento
        Serial.println("registro CSV (LittleFS simulado):");
        Serial.println("   " + linha_csv);
        Serial.println("wokwi: registro salvo no LittleFS simulado");
        return true;

#else
        // no físico, salva no LittleFS real
        File arquivo = LittleFS.open(nome_arquivo, "a"); // "a" = append (adicionar ao final)

        if (!arquivo)
        {
            Serial.println("falha ao abrir arquivo para escrita");
            return false;
        }

        // se o arquivo estiver vazio, adiciona cabeçalho
        if (arquivo.size() == 0)
        {
            arquivo.println("timestamp,data_hora,temperatura,luminosidade,temp_valida,lux_valida,checksum");
        }

        // escreve a linha
        arquivo.println(linha_csv);
        arquivo.close();

        Serial.println("registro salvo no LittleFS:");
        Serial.println("   " + linha_csv);
        Serial.println("dados armazenados com sucesso!");

        return true;
#endif
    }

    // cria/verifica o cabeçalho do arquivo

    void criarCabecalho()
    {
        Serial.println("\ncabeçalho LittleFS:");
        Serial.println("   timestamp,data_hora,temperatura,luminosidade,temp_valida,lux_valida,checksum");

#ifndef AMBIENTE_WOKWI

        // no físico, verifica se precisa criar cabeçalho
        if (!LittleFS.exists(nome_arquivo))
        {
            File arquivo = LittleFS.open(nome_arquivo, "w");
            if (arquivo)
            {
                arquivo.println("timestamp,data_hora,temperatura,luminosidade,temp_valida,lux_valida,checksum");
                arquivo.close();
                Serial.println("cabeçalho criado no arquivo LittleFS");
            }
        }
#endif
    }

    // verifica se um registro está íntegro

    bool verificarIntegridade(const RegistroDados &registro)
    {
        uint32_t checksum_calculado = calcularChecksum(registro);
        bool integro = (checksum_calculado == registro.checksum);

        Serial.print("verificação de integridade: ");
        Serial.println(integro ? "OK" : "CORROMPIDO");

        return integro;
    }

    // lista arquivos disponíveis (para debug)

    void listarArquivos()
    {
        Serial.println("\n[i] arquivos no LittleFS:");

#ifndef AMBIENTE_WOKWI
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
#else
        Serial.println("   [simulação wokwi - arquivos virtuais]");
        Serial.println("   /dados_log.csv");
        Serial.println("   /dados_pendente.csv");
#endif
    }
};

#endif