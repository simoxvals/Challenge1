#include <Adafruit_LiquidCrystal.h>

Adafruit_LiquidCrystal lcd_1(0);

int pinGenerador = 0;
int Estadoboton1 = 0;
int Estadoboton2 = 0;
int numLecturas = 100;
int pendiente = 0;
int C = 0;

float maximo = 0;
float start = 0;
float end = 0;
float N = 0;

int* lecturas = nullptr;
float* pFrecuencia = nullptr;
float* pAmplitud = nullptr;
int* pPendiente= nullptr;

bool sCuadrada = false;
bool sTriangular = false;
bool cambioBrusco = false;
bool esAnaloga = false;
bool adquiriendoDatos = false;


void liberarMemoria() 
{
    delete pFrecuencia;
    delete pAmplitud;
    delete[] lecturas;
  	delete [] pPendiente;
    pFrecuencia = nullptr;
    pAmplitud = nullptr;
    lecturas = nullptr;
}


void asignarMemoria() 
{
    pFrecuencia = new float;
    pAmplitud = new float;
    lecturas = new int[numLecturas];
  	pPendiente = new int[10];
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
        sCuadrada = false;
        sTriangular = false;
        cambioBrusco = false;
        esAnaloga = false;
      
        N = 0; 
        maximo = 0;
        start = millis();
        
        Serial.println("Iniciando adquisicion...");
        lcd_1.setCursor(0, 0);
        lcd_1.print("Adquiriendo...");

    }

    if (adquiriendoDatos) {
        for (int i = 0; i < numLecturas; i++) 
        {
            lecturas[i] = analogRead(pinGenerador);
            Serial.println(lecturas[i]);
 
            if (lecturas[i] >= maximo) 
            {
                maximo = lecturas[i];
              	pPendiente = &lecturas[i];
              	
              
              	if(i > 0 && lecturas[i] < maximo)
                {
                  pendiente = maximo - lecturas[i];
                  
                  
                  if( pendiente > 20 )
                  {
                      C++;
                  }
                }
             }

            if (i >= 10 && abs(lecturas[i] - lecturas[i-1]) > 100) {
                cambioBrusco = true;
            }

            if (cambioBrusco) {
                
                if (i >= 10 && lecturas[i] == lecturas[i-1] && lecturas[i-1] == lecturas[i-2]) {
                    sCuadrada = true;
                    
                }
            }

            if (i >= 10 && lecturas[i] > 0 && lecturas[i-1] < 0) {
                N++;
            }

            if (digitalRead(3) == HIGH) 
            {
              	adquiriendoDatos = false;
                lcd_1.clear();
                Serial.println("Adquisicion detenida.");
                
                *pAmplitud = abs(maximo / 100.0);
              	end = millis() - start;
                *pFrecuencia = (N / (end / 1000.0));
              
                int parteEntera = (int)(*pFrecuencia);
                float parteDecimal = (*pFrecuencia) - parteEntera;
              
                if (parteDecimal > 0.35) 
                {
                    *pFrecuencia = parteEntera + 1;
                }
                else 
                {
                    *pFrecuencia = parteEntera + parteDecimal;
                }

              
                if(C >= 1) 
                  
                {
					sTriangular = true;
						
                }
                else
                {
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
                lcd_1.print("Amp: ");
                lcd_1.print(*pAmplitud);
                lcd_1.print("V");
                lcd_1.setCursor(0, 1);
                lcd_1.print("Frec: ");
                lcd_1.print(*pFrecuencia);
                lcd_1.print("Hz");
				
              
                liberarMemoria();
                asignarMemoria();
                break;
                delay(10);

            }
        }
    }
} 
