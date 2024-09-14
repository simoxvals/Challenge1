#include <Adafruit_LiquidCrystal.h>
Adafruit_LiquidCrystal lcd_1(0);

int pinGenerador = 0;
int Estadoboton1 = 0;
int Estadoboton2 = 0;
int numLecturas = 500;
int* lecturas = nullptr;
bool adquiriendoDatos = false;
int tiempo_minimo = 100;
float maximo=0;
float* pFrecuencia = nullptr;
float* pAmplitud = nullptr;
float start = 0;
float end  = 0;
float N = 0;

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
        adquiriendoDatos = true;
        Serial.println("Iniciando adquisicion de datos...");
        start = millis();
    }
    
    if (adquiriendoDatos) {
        
        for (int i = 0; i < numLecturas; i++) {
            lecturas[i] = analogRead(pinGenerador);
            Serial.println(lecturas[i]);
            delay(10);
            
            
            if (lecturas[i]>=maximo) {
                maximo=lecturas[i];
                if(lecturas[i]== maximo){
                    N++;
                }
            }
            
            if (digitalRead(3) == HIGH) {
                Serial.println("Adquisicion de datos detenida por boton 2.");
                adquiriendoDatos = false;
                *pAmplitud = (maximo/100.0);
                end = millis() - start;
                *pFrecuencia = (N / (end/1000));
                break;
            }
            
        }
        if (!adquiriendoDatos) {
            lcd_1.clear();
            lcd_1.setCursor(0, 0);
            lcd_1.print("Amp:");
            lcd_1.print((*pAmplitud));
            lcd_1.print("V");
            lcd_1.setCursor(0, 1);
            lcd_1.print("Fr: ");
            lcd_1.print((*pFrecuencia ));
            lcd_1.print("Hz");
            liberarMemoria();
            asignarMemoria();
        }
    }
}




