// Josué Daniel Barrios Morales - Diego Roberto Roche Palacios - Percy Matthew Jacobs Orellana - Ingeniería Electrónica - Universidad de San Carlos de Guatemala
// josuedbm7@gmail.com - dr.roche97@gmail.com - percyjacobso@gmail.com
// Potenciómetro
// 10/08/2020

// Librerías para pantalla LCD con protocolo de comunicación I2C.
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

//Inicialización de la pantalla con su respectiva dirección y estableciéndola a 16 caracteres y 2 líneas
LiquidCrystal_I2C lcd(0x27,16,2);

//------------------------Temperatura------------------------------

#include <OneWire.h> 
#include <DallasTemperature.h>

#define ONE_WIRE_BUS A1

OneWire oneWire(ONE_WIRE_BUS); 

DallasTemperature sensors(&oneWire);

//------------------------Temperatura------------------------------

//-------------Variables para leer los botones---------------------
const int b2 = 2; //-PULSADOR- Pin 2, para seleccionar la muestra (pH 4,7 o 10)-OP-
const int b3 = 3; //-PULSADOR- Pin 3, para tomar muestra
const int b4 = 4; //-SWITCH- Pin 4, para seleccionar modo calibración o modo medición
int b4_modo = 0;
int stateb4 = 0;
//-------------Variables para leer los botones---------------------

//-----------------------Banderas----------------------------------
bool flagcal = false;
bool flagph4 = false;
bool flagph7= false;
bool flagph10 = false;
bool flagmedicion = false;

bool medir = false;

//-----------------------Banderas----------------------------------

//----------------Constantes de pH---------------------------------
const int ph4 = 4;
const int ph7 = 7;
const int ph10 = 10;
float y = 0;
float x = 0;
float m = 0;
float b = 0;
float pH = 0;
//----------------Constantes de pH---------------------------------

//---------Variables para guardar valores de la calibración--------
volatile float muestravoltaje = 0;
volatile float v1 = 0;
volatile float v2 = 0;
volatile float v3 = 0;
//---------Variables para guardar valores de la calibración--------

//--Variable para iteración de la medición que se está realizando--
volatile int op = 0;
//--Variable para iteración de la medición que se está realizando--


//------------Variable para guardar valor de voltaje---------------
volatile float voltaje;
//------------Variable para guardar valor de voltaje---------------

//------------Variable para conteos---------------
volatile int n = 0;
volatile int n1 = 0;
int contador = 0;
//------------Variable para conteos---------------

void setup() {
  Serial.begin(9600);
  pinMode(b2, INPUT);
  pinMode(b3, INPUT);
  pinMode(b4, INPUT);  

  //Temperatura
  sensors.begin();

  //Interrupciones de ambos botones
  attachInterrupt(digitalPinToInterrupt(b2), boton1, RISING);
  attachInterrupt(digitalPinToInterrupt(b3), boton3, RISING);

  //Inicializar LCD
  lcd.init();
  lcd.backlight();
  
  //LCD - Mensaje de bienvenida
  lcd.setCursor(0,0);
  lcd.print("Potenciometro");
  lcd.setCursor(10,1);
  lcd.print("USAC");
  delay(4000);
  lcd.clear();  
}

void loop() {


  int sensorValue = analogRead(A0);
  voltaje = sensorValue * (5.0 / 1023.0);

//Seleccion de modo de operacion - switch cerrado, calibracion - switch abierto, medicion
  b4_modo = digitalRead(b4);
  //stateb4 = digitalRead(b4);

  if (b4_modo == HIGH && flagcal == false){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Modo calibracion");
    calibracion();
    }
   delay(300);

//-----------------------------------------------

   if (v1 != 0 && v2 != 0 && v3 != 0 && flagcal == true){
       x = v2 - v1;
       y = ph7 - ph4;
       m = y/x;
       b = ph4 - v1*m;
       flagmedicion = true;   

       if (b4_modo == LOW && flagcal == true){
         medicion();
       }
      }

//-----------------------------------------------

}

void medicion(){

    if (flagcal == true && flagmedicion == true){
      
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Modo medicion");
        int state = 0;

        if(Serial.available() > 0){ // Checks whether data is comming from the serial port
         state = Serial.read(); // Reads the data from the serial port
         }
      
        if (medir == true || state == '1'){
         sensors.requestTemperatures();
         pH = m*voltaje + b;
         lcd.setCursor(0,1);
         lcd.print("pH:");
         lcd.print(pH);
         lcd.setCursor(10,1);
         lcd.print("T:");
         lcd.print(sensors.getTempCByIndex(0));
         //Serial.print("h" + String(sensors.getTempCByIndex(0), 2) + "," + String(pH,2) + "," + String(pH,2) + "f");
         Serial.print("h" + String(sensors.getTempCByIndex(0), 2) + "," + String(pH,2) + "f");
         delay(3000);
         medir = false;

        //Almacenamiento de variables en EEPROM

        //EEPROM.put( eeAddress, v1);  //Grabamos el valor
        //eeAddress += sizeof(float);  //Obtener la siguiente posicion para escribir
        //EEPROM.put( eeAddress, v2);  
        //eeAddress += sizeof(float);  
        //EEPROM.put( eeAddress, v3);  
        //eeAddress += sizeof(float);  
        //EEPROM.put( eeAddress, v3);
        //if(eeAddress >= EEPROM.length()) eeAddress = 0;  //Comprobar que no hay desbordamiento
         
        }
            
    }
    
}

void calibracion(){

  lcd.setCursor(2,1);
  lcd.print(voltaje);

  switch (op) {
    case 1:      
      lcd.setCursor(8,1);
      lcd.print("OP: ");
      lcd.print(op);
      break;
    case 2:      
      lcd.setCursor(8,1);
      lcd.print("OP: ");
      lcd.print(op);
      break;
    case 3:
      lcd.setCursor(8,1);
      lcd.print("OP: ");
      lcd.print(op);
      break;
    default:
      // statements
      break;
  }

    if (op == 1 && v1 == 0.00) {
    
      if (3.1 > voltaje){ //toma 10 muestras de voltaje que estén dentro del rango de voltaje del pH4
        if (2.9 < voltaje){
          for(n = 0; n <= 5; n++){
            contador += 1;
            muestravoltaje += voltaje;
            delay(500);
            if (contador == 5){ //promedia las muestras de voltaje para el pH4
              v1 = muestravoltaje / contador;
              muestravoltaje = 0;
              flagph4 = true;     
              contador = 0;
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("Muestra 1 tomada");
              delay(2000);              
              }           
            break;
          }
        }
      }
    }

    if (op == 2 && v1 != 0 && v2 == 0) {
    
      if (2.6 > voltaje){ //toma 10 muestras de voltaje que estén dentro del rango de voltaje del pH4
        if (2.4 < voltaje){
          for(n = 0; n <= 5; n++){
            contador += 1;
            muestravoltaje += voltaje;
            delay(500);
            if (contador == 5){ //promedia las muestras de voltaje para el pH4
              v2 = muestravoltaje / contador;
              muestravoltaje = 0;
              flagph7 = true; 
              contador = 0;
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("Muestra 2 tomada");
              delay(2000);                      
              }           
            break;
          }
        }
      }
    }

    if (op == 3 && v2 != 0 && v3 == 0) {      
          
      if (2.1 > voltaje){ //toma 10 muestras de voltaje que estén dentro del rango de voltaje del pH4
        if (1.9 < voltaje){
          for(n = 0; n <= 5; n++){
            contador += 1;
            muestravoltaje += voltaje;
            delay(500);
            if (contador == 5){ //promedia las muestras de voltaje para el pH4
              v3 = muestravoltaje / contador;
              muestravoltaje = 0;
              flagph10 = true;    
              contador = 0;
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("Muestra 3 tomada");
              delay(2000);
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("Calibracion");              
              lcd.setCursor(6,1);
              lcd.print("completada");              
              }           
            break;
          }
        }
      }
    }

    if (flagph4 == true && flagph7 == true && flagph10 == true){
      flagcal = true;      
      }else{
        flagcal = false;
        }    
}

void boton1(){  
    op = op + 1;
    delay(1000);
    
    if (op == 4){
      op = 1;
      }
  }

void boton3(){  
    medir = true;
    delay(1000);
  }
