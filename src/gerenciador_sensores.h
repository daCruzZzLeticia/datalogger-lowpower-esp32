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

// LEITURA DE SENSORES FÍSICOS E SIMULADOS POR WOKWI SÃO IGUAIS

float lerTemperatura()
{
    Serial.println("Lendo sensor de temperatura...");

    int leitura_analogica = analogRead(PINO_TERMISTOR);

    // se a leitura está nos padrões para ESP32 (se há sensor conectado)
    if (leitura_analogica < 10 || leitura_analogica > 4090)
    {
        Serial.println("Sensor de temperatura não respondendo");
        return NAN;
    }

    // converte leitura analógica para temperatura (cálculo do NTC)
    float resistencia = 10000.0 / (4095.0 / leitura_analogica - 1.0);
    float temperatura_kelvin = 1.0 / (log(resistencia / 10000.0) / BETA_TERMISTOR + 1.0 / 298.15);
    float temperatura_celsius = temperatura_kelvin - 273.15;

    // verifica se o valor está dentro de limites razoáveis
    if (temperatura_celsius < -50.0 || temperatura_celsius > 100.0)
    {
        Serial.println("Temperatura fora dos limites razoáveis");
        return NAN;
    }

    Serial.print("Temperatura lida: ");
    Serial.print(temperatura_celsius);
    Serial.println(" °C");

    return temperatura_celsius;
}

float lerLuminosidade()
{
    Serial.println("Lendo sensor de luminosidade...");

    int leitura_analogica = analogRead(PINO_FOTORESISTOR);

    // se a leitura está nos padrões para ESP32 (se há sensor conectado)
    if (leitura_analogica < 10 || leitura_analogica > 4090)
    {
        Serial.println("Sensor de luminosidade não respondendo");
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
        Serial.println("Luminosidade fora dos limites razoáveis");
        return NAN;
    }

    Serial.print("Luminosidade lida: ");
    Serial.print(luminosidade_lux);
    Serial.println(" lux");

    return luminosidade_lux;
}

#endif