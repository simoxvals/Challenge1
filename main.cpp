#include <Adafruit_LiquidCrystal.h>

Adafruit_LiquidCrystal lcd_1(0);

int pinGenerador = 0;
int Estadoboton1 = 0;
int Estadoboton2 = 0;
int numLecturas = 500;
int* lecturas = nullptr;
bool adquiriendoDatos = false;
float maximo = 0;
float* pFrecuencia = nullptr;
float* pAmplitud = nullptr;
float start = 0;
float end = 0;
float N = 0;
bool sCuadrada = false;
bool sTriangular = false;
bool cambioBrusco = false;
bool esAnaloga = false;

void liberarMemoria() {
    delete pFrecuencia;
    delete pAmplitud;
    delete[] lecturas;
    pFrecuencia = nullptr;
    pAmplitud = nullptr;
    lecturas = nullptr;
}

void asignarMemoria() {
    pFrecuencia = new float;
    pAmplitud = new float;
    lecturas = new int[numLecturas];
    *pFrecuencia = 0;
    *pAmplitud = 0;
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
        N = 0; 
        maximo = 0;
        adquiriendoDatos = true;
        sCuadrada = false;
        sTriangular = false;
        cambioBrusco = false;
        esAnaloga = false;
        Serial.println("Iniciando adquisicion de datos...");
        start = millis();
    }

    if (adquiriendoDatos) {
        for (int i = 0; i < numLecturas; i+=20) {
            lecturas[i] = analogRead(pinGenerador);
            Serial.println(lecturas[i]);
            delay(10);

            if (lecturas[i] >= maximo) {
                maximo = lecturas[i];
            }

            if (i >= 50 && abs(lecturas[i] - lecturas[i-20]) > 100) {
                cambioBrusco = true;
            }

            if (cambioBrusco) {
                
                if (i >= 40 && lecturas[i] == lecturas[i-20] && lecturas[i-20] == lecturas[i-40]) {
                    sCuadrada = true;
                    maximo = lecturas[i];
                }

                if (i >= 40) {
                    bool creciente = true;
                    bool decreciente = true;
                    for (int j = i-20; j >= i-40; j -= 20) {
                        if (lecturas[j] < lecturas[j+20]) {
                            decreciente = false;
                        }
                        if (lecturas[j] > lecturas[j+20]) {
                            creciente = false;
                        }
                    }
                    if (creciente && decreciente) {
                        sTriangular = true;
                    }
                }
            }

            if (i >= 5 && lecturas[i] > 0 && lecturas[i-20] < 0) {
                N++;
            }

            if (digitalRead(3) == HIGH) {
                Serial.println("Adquisicion de datos detenida por boton 2.");
                adquiriendoDatos = false;
                *pAmplitud = abs(maximo / 100.0);
                end = millis() - start;
                *pFrecuencia = (N / (end / 1000.0));
                
                int parteEntera = (int)(*pFrecuencia);
                float parteDecimal = (*pFrecuencia) - parteEntera;
                if (parteDecimal > 0.35) {
                    *pFrecuencia = parteEntera + 1;
                } else {
                    *pFrecuencia = parteEntera + parteDecimal;
                }

                if (!cambioBrusco) {
                    esAnaloga = true; 
                }
 
                lcd_1.clear();
                lcd_1.setCursor(0, 0);
                if (sCuadrada) {
                    lcd_1.print("Senal Cuadrada:");
                } else if (sTriangular) {
                    lcd_1.print("Senal Triangular:");
                } else if (esAnaloga) {
                    lcd_1.print("Senal Analogica:");
                } else {
                    lcd_1.print("Senal Desconocida:");
                }
                lcd_1.setCursor(0, 1);
                lcd_1.print("Amplitud: ");
                lcd_1.print(*pAmplitud);
                lcd_1.print("V");
                lcd_1.setCursor(0, 1);
                lcd_1.print("Frecuencia: ");
                lcd_1.print(*pFrecuencia);
                lcd_1.print("Hz");

                liberarMemoria();
                asignarMemoria();
                break;
            }
        }
    }
}


