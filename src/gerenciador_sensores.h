#ifndef GERENCIADOR_SENSOR_H
#define GERENCIADOR_SENSOR_H

#include "config.h"
#include "Arduino.h"

/*
 *  [i] estrutura para armazenar dados dos sensores
 */
struct DadosSensores
{
    float temperatura;          // temperatura em graus celsius
    float luminosidade;         // luminosidade em lux
    bool temperatura_valida;    // indica se leitura e confiavel
    bool luminosidade_valida;   // indica se leitura e confiavel
    uint32_t timestamp_leitura; // quando a leitura foi feita (millis)
};

/*
 *  [i] classe principal do gerenciador de sensores
 */
class GerenciadorSensores
{
private:
    bool sensores_inicializados;
    bool mock_temperatura;       // true se usando dados simulados para temperatura
    bool mock_luminosidade;      // true se usando dados simulados para luminosidade
    unsigned long contador_mock; // contador para variacao dos dados simulados

    /*
     * le o sensor de temperatura real (NTC)
     * retorna temperatura em celsius ou NAN se falhar
     */
    float lerTemperatura()
    {
        Serial.println("lendo sensor de temperatura...");

        int leitura_analogica = analogRead(PINO_TERMISTOR);

        // verifica se o sensor esta respondendo (leitura dentro do range valido do ADC)
        if (leitura_analogica < 10 || leitura_analogica > 4090)
        {
            Serial.println("[!] sensor de temperatura nao respondendo");
            return NAN;
        }

        // converte leitura analogica para resistencia usando divisor de tensao
        float resistencia = 10000.0 / (4095.0 / leitura_analogica - 1.0);

        // converte resistencia para temperatura usando equacao do termistor NTC
        float temperatura_kelvin = 1.0 / (log(resistencia / 10000.0) / BETA_TERMISTOR + 1.0 / 298.15);
        float temperatura_celsius = temperatura_kelvin - 273.15;

        // verifica se o valor esta dentro de limites razoaveis para ambiente
        if (temperatura_celsius < -50.0 || temperatura_celsius > 100.0)
        {
            Serial.println("[!] temperatura fora dos limites razoaveis");
            return NAN;
        }

        Serial.println("temperatura: " + String(temperatura_celsius, 2) + " °C");
        return temperatura_celsius;
    }

    /*
     * le o sensor de luminosidade real (LDR)
     * retorna luminosidade em lux ou NAN se falhar
     */
    float lerLuminosidade()
    {
        Serial.println("lendo sensor de luminosidade...");

        int leitura_analogica = analogRead(PINO_FOTORESISTOR);

        // verifica se o sensor esta respondendo
        if (leitura_analogica < 10 || leitura_analogica > 4090)
        {
            Serial.println("[!] sensor de luminosidade nao respondendo");
            return NAN;
        }

        // converte leitura analogica para tensao
        float tensao = leitura_analogica / 4096.0 * 3.3;

        // calcula resistencia do LDR usando divisor de tensao
        float resistencia = 2000.0 * tensao / (1.0 - tensao / 3.3);

        // evita divisao por zero ou valores negativos
        if (resistencia <= 0)
        {
            return NAN;
        }

        // converte resistencia para luminosidade usando curva caracteristica do LDR
        float luminosidade_lux = pow(RESISTENCIA_LDR * 1000.0 * pow(10.0, GAMA_LDR) / resistencia, (1.0 / GAMA_LDR));

        // verifica se o valor esta dentro de limites razoaveis
        if (luminosidade_lux < 0.1 || luminosidade_lux > 100000.0)
        {
            Serial.println("[!] luminosidade fora dos limites razoaveis");
            return NAN;
        }

        Serial.println("luminosidade: " + String(luminosidade_lux, 0) + " lux");
        return luminosidade_lux;
    }

    /*
     * gera dados simulados quando os sensores reais nao estao disponiveis
     * os valores variam suavemente para simular condicoes reais
     */
    void gerarDadosMock(DadosSensores &dados)
    {
        contador_mock++;

        // temperatura oscila entre 20°C e 25°C de forma suave (funcao seno)
        float variacao_temperatura = sin(contador_mock * 0.1) * 2.5;
        dados.temperatura = 22.5 + variacao_temperatura;

        // luminosidade varia entre 100 e 1000 lux (ambiente interno tipico)
        float variacao_luz = sin(contador_mock * 0.05) * 450.0;
        dados.luminosidade = 550.0 + variacao_luz;

        // marca os dados como simulados
        dados.temperatura_valida = mock_temperatura;
        dados.luminosidade_valida = mock_luminosidade;

        Serial.println("usando dados simulados");
    }

public:
    /*
     * construtor - inicializa o gerenciador
     */
    GerenciadorSensores()
    {
        sensores_inicializados = false;
        mock_temperatura = false;
        mock_luminosidade = false;
        contador_mock = 0;
    }

    /*
     * inicializa os sensores e detecta quais estao disponiveis
     * deve ser chamado uma vez no setup()
     */
    void iniciar()
    {
        Serial.println("\ninicializando gerenciador de sensores...");

        // configura os pinos dos sensores como entrada
        pinMode(PINO_TERMISTOR, INPUT);
        pinMode(PINO_FOTORESISTOR, INPUT);

        Serial.println("verificando sensores disponiveis...\n");

        // testa sensor de temperatura para ver se esta funcionando
        float teste_temperatura = lerTemperatura();
        if (!isnan(teste_temperatura))
        {
            Serial.println("sensor de temperatura: detectado\n");
            mock_temperatura = false;
        }
        else
        {
            Serial.println("[!] sensor de temperatura: usando dados simulados\n");
            mock_temperatura = true;
        }

        // testa sensor de luminosidade para ver se esta funcionando
        float teste_luminosidade = lerLuminosidade();
        if (!isnan(teste_luminosidade))
        {
            Serial.println("sensor de luminosidade: detectado\n");
            mock_luminosidade = false;
        }
        else
        {
            Serial.println("[!] sensor de luminosidade: usando dados simulados\n");
            mock_luminosidade = true;
        }

        sensores_inicializados = true;
        Serial.println("gerenciador de sensores inicializado\n");
    }

    /*
     * le todos os sensores disponiveis
     * retorna estrutura com dados e flags de validade
     */
    DadosSensores lerSensores()
    {
        DadosSensores dados;

        // se nao foi inicializado, inicializa automaticamente
        if (!sensores_inicializados)
        {
            Serial.println("[!] sensores nao inicializados - inicializando...\n");
            iniciar();
        }

        dados.timestamp_leitura = millis();

        // tenta ler sensor de temperatura real se disponivel
        if (!mock_temperatura)
        {
            dados.temperatura = lerTemperatura();
            dados.temperatura_valida = !isnan(dados.temperatura);

            // se a leitura real falhou e mocks estao habilitados, alterna para mock
            if (!dados.temperatura_valida && SENSORES_MOCKS)
            {
                Serial.println("[!] leitura de temperatura falhou - usando dados simulados\n");
                mock_temperatura = true;
            }
        }

        // tenta ler sensor de luminosidade real se disponivel
        if (!mock_luminosidade)
        {
            dados.luminosidade = lerLuminosidade();
            dados.luminosidade_valida = !isnan(dados.luminosidade);

            if (!dados.luminosidade_valida && SENSORES_MOCKS)
            {
                Serial.println("[!] leitura de luminosidade falhou - usando dados simulados\n");
                mock_luminosidade = true;
            }
        }

        // se algum sensor precisa de dados simulados, gera eles
        if (mock_temperatura || mock_luminosidade)
        {
            gerarDadosMock(dados);
        }

        return dados;
    }

    /*
     * informa quais sensores estao usando dados reais ou simulados
     * util para debugging e status do sistema
     */
    void imprimirStatus()
    {
        Serial.println("status dos sensores:");
        Serial.println("  temperatura: " + String(mock_temperatura ? "dados simulados" : "sensor real"));
        Serial.println("  luminosidade: " + String(mock_luminosidade ? "dados simulados" : "sensor real"));
        Serial.println();
    }
};

#endif