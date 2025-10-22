// sensor_manager.h
#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include "config.h"
#include "Arduino.h"

// ESTRUTURA PARA ARMAZENAR DADOS DOS SENSORES

struct DadosSensores
{
    float temperatura;          // temperatura em graus Celsius
    float luminosidade;         // luminosidade em lux
    bool temperatura_valida;    // leitura de temperatura confiável
    bool luminosidade_valida;   // leitura de luminosidade confiável
    uint32_t timestamp_leitura; // quando a leitura foi feita (millis)
};

// CLASSE PRINCIPAL DO GERENCIADOR DE SENSORES

class GerenciadorSensores
{
private:
    bool sensores_inicializados;
    bool mock_temperatura;  // true ou falso se está usando dados mock
    bool mock_luminosidade; // true ou falso se está usando dados mock
    unsigned long contador_mock;

    // LEITURA DE SENSORES FÍSICOS E SIMULADOS POR WOKWI SÃO IGUAIS

    float lerTemperatura()
    {
        Serial.println("lendo sensor de temperatura");

        int leitura_analogica = analogRead(PINO_TERMISTOR);

        // se a leitura está nos padrões para ESP32 (se há sensor conectado)
        if (leitura_analogica < 10 || leitura_analogica > 4090)
        {
            Serial.println("    sensor de temperatura não respondendo");
            return NAN;
        }

        // converte leitura analógica para temperatura (cálculo do NTC)
        float resistencia = 10000.0 / (4095.0 / leitura_analogica - 1.0);
        float temperatura_kelvin = 1.0 / (log(resistencia / 10000.0) / BETA_TERMISTOR + 1.0 / 298.15);
        float temperatura_celsius = temperatura_kelvin - 273.15;

        // verifica se o valor está dentro de limites razoáveis
        if (temperatura_celsius < -50.0 || temperatura_celsius > 100.0)
        {
            Serial.println("    temperatura fora dos limites razoáveis");
            return NAN;
        }

        Serial.print("  temperatura lida: ");
        Serial.print(temperatura_celsius);
        Serial.println(" °C");

        return temperatura_celsius;
    }

    float lerLuminosidade()
    {
        Serial.println("lendo sensor de luminosidade");

        int leitura_analogica = analogRead(PINO_FOTORESISTOR);

        // se a leitura está nos padrões para ESP32 (se há sensor conectado)
        if (leitura_analogica < 10 || leitura_analogica > 4090)
        {
            Serial.println("    sensor de luminosidade não respondendo");
            return NAN;
        }

        // converte leitura analógica para luminosidade em lux
        float tensao = leitura_analogica / 4096.0 * 3.3;
        float resistencia = 2000.0 * tensao / (1.0 - tensao / 3.3);

        if (resistencia <= 0)
        {
            return NAN; // evita divisão por zero
        }

        float luminosidade_lux = pow(RESISTENCIA_LDR * 1000.0 * pow(10.0, GAMA_LDR) / resistencia, (1.0 / GAMA_LDR));

        // verifica se o valor está dentro de limites razoáveis
        if (luminosidade_lux < 0.1 || luminosidade_lux > 100000.0)
        {
            Serial.println("    luminosidade fora dos limites razoáveis");
            return NAN;
        }

        Serial.print("  luminosidade lida: ");
        Serial.print(luminosidade_lux);
        Serial.println(" lux");

        return luminosidade_lux;
    }

    // gerar dados simulados quando sensores indisponíveis
    void gerarDadosMock(DadosSensores &dados)
    {
        contador_mock++;

        // temperatura mock: oscila entre 20°C e 25°C
        float variacao_temperatura = sin(contador_mock * 0.1) * 2.5;
        dados.temperatura = 22.5 + variacao_temperatura;

        // luminosidade mock: varia entre 100 e 1000 lux
        float variacao_luz = sin(contador_mock * 0.05) * 450.0;
        dados.luminosidade = 550.0 + variacao_luz;

        dados.temperatura_valida = mock_temperatura;
        dados.luminosidade_valida = mock_luminosidade;

        Serial.println("usando dados MOCK (simulados)");
    }

public:
    GerenciadorSensores()
    {
        sensores_inicializados = false;
        mock_temperatura = false;
        mock_luminosidade = false;
        contador_mock = 0;
    }

    void iniciar()
    {
        Serial.println("inicializando gerenciador de sensores...");

        // configura os pinos
        pinMode(PINO_TERMISTOR, INPUT);
        pinMode(PINO_FOTORESISTOR, INPUT);

        Serial.println("procurando sensores disponíveis...");

        // teste sensor de temperatura
        float teste_temperatura = lerTemperatura();
        if (!isnan(teste_temperatura))
        {
            Serial.println("sensor de temperatura detectado!");
            mock_temperatura = false;
        }
        else
        {
            Serial.println("sensor de temperatura não disponível - usando MOCK");
            mock_temperatura = true;
        }

        // testa sensor de luminosidade
        float teste_luminosidade = lerLuminosidade();
        if (!isnan(teste_luminosidade))
        {
            Serial.println("sensor de luminosidade detectado!");
            mock_luminosidade = false;
        }
        else
        {
            Serial.println("sensor de luminosidade não disponível - usando MOCK");
            mock_luminosidade = true;
        }

        sensores_inicializados = true;
        Serial.println("gerenciador de sensores inicializado");
    }

    /**
     * lê todos os sensores disponíveis
     * retorna estrutura com dados e flags de validade
     */

    DadosSensores lerSensores()
    {
        DadosSensores dados;

        if (!sensores_inicializados)
        {
            Serial.println("sensores não inicializados! chamando iniciar()...");
            iniciar();
        }

        dados.timestamp_leitura = millis();

        // tenta ler sensores reais
        if (!mock_temperatura)
        {
            dados.temperatura = lerTemperatura();
            dados.temperatura_valida = !isnan(dados.temperatura);

            if (!dados.temperatura_valida && SENSORES_MOCKS)
            {
                Serial.println("leitura de temperatura falhou - usando MOCK");
                mock_temperatura = true;
            }
        }

        if (!mock_luminosidade)
        {
            dados.luminosidade = lerLuminosidade();
            dados.luminosidade_valida = !isnan(dados.luminosidade);

            if (!dados.luminosidade_valida && SENSORES_MOCKS)
            {
                Serial.println("leitura de luminosidade falhou - usando MOCK");
                mock_luminosidade = true;
            }
        }

        // se precisar, usa dados mock
        if (mock_temperatura || mock_luminosidade)
        {
            gerarDadosMock(dados);
        }

        return dados;
    }

    // informa quais sensores estão usando dados reais

    void imprimirStatus()
    {
        Serial.println("\n[i] status dos sensores:");
        Serial.print("  temperatura: ");
        Serial.println(mock_temperatura ? "MOCK" : "REAL");
        Serial.print("  luminosidade: ");
        Serial.println(mock_luminosidade ? "MOCK" : "REAL");
    }
};

#endif