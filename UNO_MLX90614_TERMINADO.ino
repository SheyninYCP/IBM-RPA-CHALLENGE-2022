/*************************************************** 
  This is a library example for the MLX90614 Temp Sensor

  Designed specifically to work with the MLX90614 sensors in the
  adafruit shop
  ----> https://www.adafruit.com/products/1748
  ----> https://www.adafruit.com/products/1749

  These sensors use I2C to communicate, 2 pins are required to  
  interface
  Adafruit invests time and resources providing this open source code, 
  please support Adafruit and open-source hardware by purchasing 
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.  
  BSD license, all text above must be included in any redistribution
 ****************************************************/

#include <Wire.h>
#include <Adafruit_MLX90614.h>
#include <LiquidCrystal.h>    // importa libreria

const int temp_true=8;
const int dni_confir=9;
const int abrir_barrera=13;
const int sensor_barrera_arriba=0;
const int sensor_persona=1;

const int dosis1=10;
const int dosis2=11;
const int dosis3=12;

int var_dosis1=0;
int var_dosis2=0;
int var_dosis3=0;

int var_sen_persona;
int var_barrera;
int  sen_bar_arriba;
int value_dni;
int dni_buscando;

float valor_temp= 0;

Adafruit_MLX90614 mlx = Adafruit_MLX90614();
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);  // pines RS, E, D4, D5, D6, D7 de modulo 1602A

void setup() {
 
  lcd.begin(16, 2);     // inicializa a display de 16 columnas y 2 lineas
  pinMode(temp_true, OUTPUT);
   pinMode(sensor_barrera_arriba,INPUT);
  pinMode(abrir_barrera, OUTPUT);
  pinMode(sensor_persona, INPUT);
  pinMode(dni_confir, INPUT);
   pinMode(dosis1, INPUT);
    pinMode(dosis2, INPUT);
     pinMode(dosis3, INPUT);
  
  digitalWrite(abrir_barrera, LOW);
  digitalWrite(temp_true, LOW);
  
  mlx.begin();  

}

void loop() {
 lcd.setCursor(0, 0);      // ubica cursor en columna 0 y fila 0   
  lcd.print("SU TEMPERATURA");    // escribe texto 
  lcd.setCursor(0, 1);      // ubica cursor en columna 0 y fila 1
  lcd.print(mlx.readObjectTempC());
  lcd.print(" C");// escribe texto    
  delay(500);
if ((mlx.readObjectTempC()<37.5) && (mlx.readObjectTempC()>31)){ //Se mide la temperatura para ingresar como requisito
  valor_temp = mlx.readObjectTempC();
  lcd.clear();
  while (  valor_temp <37.5){
  digitalWrite(temp_true, HIGH);
  value_dni= digitalRead(dni_confir);
  lcd.setCursor(0, 0);      // ubica cursor en columna 0 y fila 0  
  lcd.print("TEMP: ");// escribe texto   
  lcd.print(valor_temp);    // escribe texto 
  lcd.print(" C");// escribe texto  
  lcd.setCursor(0, 1);      // ubica cursor en columna 0 y fila 1
  lcd.print("INGRESE DNI");    // escribe texto 

  if (value_dni == LOW){
    delay(500);
    lcd.clear();
    dni_buscando = 10;
    while( dni_buscando > 3 ){
  
  lcd.setCursor(0, 0);      // ubica cursor en columna 0 y fila 0  
  lcd.print("    BUSCANDO    ");// escribe texto   
  lcd.setCursor(0, 1);      // ubica cursor en columna 0 y fila 1
  lcd.print("  INFORMACION  ");    // escribe texto 
  delay(400);
  lcd.clear();
  lcd.setCursor(0, 0);      // ubica cursor en columna 0 y fila 0  
  lcd.print("     RETIRE     ");// escribe texto   
  lcd.setCursor(0, 1);      // ubica cursor en columna 0 y fila 1
  lcd.print("     SU DNI     ");    // escribe texto 
  delay(400);
  var_dosis1 = digitalRead(dosis1);
  var_dosis2 = digitalRead(dosis2);
  var_dosis3 = digitalRead(dosis3);

   if ((var_dosis1 == HIGH) or (var_dosis2 == HIGH) or (var_dosis3 == HIGH)){
  delay(50);
  var_dosis1 = digitalRead(dosis1);
  var_dosis2 = digitalRead(dosis2);
  var_dosis3 = digitalRead(dosis3);
   }
    if ((var_dosis1 == HIGH)&&(var_dosis2 == LOW) && (var_dosis3 == LOW))
    {
      lcd.clear();
    lcd.setCursor(0, 0);      // ubica cursor en columna 0 y fila 0  
    lcd.print(" TIENE 1 DOSIS ");// escribe texto   
    lcd.setCursor(0, 1);      // ubica cursor en columna 0 y fila 1
    lcd.print("NO PUEDE ENTRAR");    // escribe texto 
    dni_buscando = 0;
     valor_temp = 38;
     delay(5000);
     lcd.clear();
    }
     if ((var_dosis1 == HIGH)&&(var_dosis2 == HIGH) && (var_dosis3 == LOW))
    {
      lcd.clear();
    lcd.setCursor(0, 0);      // ubica cursor en columna 0 y fila 0  
    lcd.print(" TIENE 2 DOSIS ");// escribe texto   
    lcd.setCursor(0, 1);      // ubica cursor en columna 0 y fila 1
    lcd.print(" PUEDE INGRESAR ");    // escribe texto 
    //AQUI AUMENTAR UN PARLANTE DE SONIDO QUE INDICA QUE NO PUEDE o UNA COMPUERTA QUE SE ACTIVO POR X SEG.
     dni_buscando = 0;
     valor_temp = 38;
     var_barrera= 20;
     while (var_barrera> 10){ 
       digitalWrite(abrir_barrera, HIGH);
       sen_bar_arriba=analogRead(sensor_barrera_arriba);
     if  (sen_bar_arriba > 500) {
     while (var_barrera> 10){
      var_sen_persona=analogRead(sensor_persona);
        if (var_sen_persona < 100){
          delay(1000);
           digitalWrite(abrir_barrera,LOW);
            var_barrera = 7;
            lcd.clear();
        }
     }
    }
     }
    }
     if (((var_dosis1 == HIGH)&&(var_dosis2 == HIGH) && (var_dosis3 == HIGH)))
    {
      lcd.clear();
    lcd.setCursor(0, 0);      // ubica cursor en columna 0 y fila 0  
    lcd.print(" TIENE 3 DOSIS ");// escribe texto   
    lcd.setCursor(0, 1);      // ubica cursor en columna 0 y fila 1
     lcd.print(" PUEDE INGRESAR ");    // escribe texto 
    dni_buscando = 0;
     valor_temp = 38;
      var_barrera= 20;
     while (var_barrera> 10){ 
       digitalWrite(abrir_barrera, HIGH);
       sen_bar_arriba=analogRead(sensor_barrera_arriba);
    if  (sen_bar_arriba > 500) {
     while (var_barrera> 10){
      var_sen_persona=analogRead(sensor_persona);
        if (var_sen_persona < 100){
          delay(1000);
           digitalWrite(abrir_barrera,LOW);
            var_barrera = 7;
            lcd.clear();
        }
     }
    }
     }
    }
    }
}
}
}
}
