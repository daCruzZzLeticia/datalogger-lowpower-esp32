#include "Arduino.h"

#define BOTAO_PIN 33 // botão conectado ao pino 33

int estadoBotao = HIGH; 

void setup() {
  Serial.begin(115200);
  delay(500);

  pinMode(BOTAO_PIN, INPUT_PULLUP);

  Serial.println("Programa iniciado!");
}

void loop() {
    estadoBotao = digitalRead(BOTAO_PIN);
    
    if(estadoBotao == LOW){
        Serial.println("Botão pressionado!");
        delay(200);
    }
}