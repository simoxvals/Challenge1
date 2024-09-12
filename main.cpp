#include <Adafruit_LiquidCrystal.h>
Adafruit_LiquidCrystal lcd_1(0);

int pinGenerador = 0;
int Estadoboton1 = 0;
int Estadoboton2 = 0;
int numLecturas = 500;
int* lecturas = nullptr;
bool adquiriendoDatos = false;
bool amplitudDetectada = false;
int minimo = 100;

int* pFrecuencia = nullptr;
int* pAmplitud = nullptr;

void liberarMemoria() {
    delete pFrecuencia;
    delete pAmplitud;
    delete[] lecturas;
    pFrecuencia = nullptr;
    pAmplitud = nullptr;
    lecturas = nullptr;
}

void asignarMemoria() {
    pFrecuencia = new int;
    pAmplitud = new int;
    lecturas = new int[numLecturas];
    *pFrecuencia = 0;
    *pAmplitud = 0;
    amplitudDetectada = false;
}

void setup() {
    lcd_1.begin(16, 2);
    Serial.begin(9600);
    pinMode(2, INPUT);
    pinMode(3, INPUT);
    asignarMemoria();
}

void loop() {
    Estadoboton1 = digitalRead(2);
    Estadoboton2 = digitalRead(3);

    if (Estadoboton1 == HIGH && !adquiriendoDatos) {
        adquiriendoDatos = true;
        Serial.println("Iniciando adquisicion de datos...");
    }

    if (adquiriendoDatos) {
        bool senalValida = false;

        for (int i = 0; i < numLecturas; i++) {
            lecturas[i] = analogRead(pinGenerador);

            if (lecturas[i] >= minimo) {
                senalValida = true;
            }

            if (senalValida && !amplitudDetectada && i > 0 && lecturas[i] < lecturas[i - 1]) {
                *pAmplitud = lecturas[i - 1];
                amplitudDetectada = true;
                Serial.println("Amplitud detectada en Volts:");
                Serial.println(*pAmplitud/100);


            }

            if (digitalRead(3) == HIGH) {
                Serial.println("Adquisicion de datos detenida por boton 2.");
                adquiriendoDatos = false;
                break;
            }

            delay(0);
        }

        if (!adquiriendoDatos) {
            if (amplitudDetectada) {
                lcd_1.clear();
                lcd_1.setCursor(0, 0);
                lcd_1.print("Amplitud:");
                lcd_1.setCursor(0, 1);
                lcd_1.print((*pAmplitud)/100);
                lcd_1.print("V");
            } else {
                lcd_1.clear();
                lcd_1.setCursor(0, 0);
                lcd_1.print("Amplitud no");
                lcd_1.setCursor(0, 1);
                lcd_1.print("detectada");
            }

            liberarMemoria();
            asignarMemoria();
        }
    }
}




