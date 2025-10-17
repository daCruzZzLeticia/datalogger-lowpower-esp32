#include <Arduino.h>

const int potPin = 34; // Pin potensiometer
const int ledPin = 4;  // Pin LED

int potValue = 0;
int ledValue = 0;

void setup()
{
    Serial.begin(115200);
    pinMode(potPin, INPUT);
    pinMode(ledPin, OUTPUT);
    Serial.println("Monitoring potensiometer dan LED siap!");
}

void loop()
{
    // Baca nilai potensiometer (0–4095)
    potValue = analogRead(potPin);

    // Ubah ke skala 0–255 untuk kecerahan LED
    ledValue = map(potValue, 0, 4095, 0, 255);

    // Atur kecerahan LED langsung dengan analogWrite
    analogWrite(ledPin, ledValue);

    // Cetak ke Serial Monitor
    Serial.print("Nilai Pot: ");
    Serial.print(potValue);
    Serial.print(" | Kecerahan LED: ");
    Serial.println(ledValue);

    delay(100);
}