/*
 * Algoritmo para control y calibracion servomotores de un brazo robotico con 6 grados de libertad usando Arduino, 
 * creado por Michael Ceballos Paulino (michcpaulino@gmail.com) de Rep. Dominicana, 
 * Distribuido para uso educativo bajo licencia Attribution-ShareAlike 4.0 International (CC BY-SA 4.0)
 * This work is licensed under a Creative Commons Attribution-ShareAlike 4.0 International License.
*/

char DATO;
int LED_placa = 13;
int LED_Adicional = 9;
int LED_Adicional_Negativo = 8;

#include <Servo.h>
Servo Hombro;
Servo Deltoide;
Servo Codo;
Servo Muneca;
Servo Palma;
Servo Pinza;
static int Posicion;
static bool Ajustar;
static bool Sentido;
int UltimoAngulo;
const int Variacion = 5; //Variable para controlar la variacion paso por paso hacia arriba o hacia abajo, presente en metodos Mover...
int velocidad = 20; //Variable para control de la velocidad
/*Frecuencias ajustadas de operacion servo-motores, obtenidas experimentalmente. Si
se detecta vibracion de los mismos estando parados cercano a los angulos extremos y sin carga o
en posicion de reposo, reajustar los valores del angulo que da problemas. 
Valores por defecto de la libreria Servo min=544 y max=2400. Preguntas a michcpaulino@gmail.com
*/
const int Frec_B_MG996 = 560; //Frecuencia Baja para angulo inferior Servo MG-996
const int Frec_A_MG996 = 2100; //Frecuencia Alta para angulo superior Servo MG-996
const int Frec_B_SG90 = 750; //Frecuencia Baja para angulo inferior Servo SG-90
const int Frec_A_SG90 = 2690; //Frecuencia Alta para angulo inferior Servo SG-90
const int PinHombro = 2;
const int PinDeltoide = 3;
const int PinCodo = 4;
const int PinMuneca = 5;
const int PinPalma = 6;
const int PinPinza = 7;

//Configuracion de la Centella
int Centella1 = A0;
int Centella1n = A1;
int Centella2 = A2;
int Centella2n = A3;
int Centella3 = A4;
int Centella3n = A5;
bool CambioCentella = false;


#include <SoftwareSerial.h>
int BluetoothTx = 11;
int BluetoothRx = 10;
SoftwareSerial Bluetooth(BluetoothRx, BluetoothTx);

void setup() {  
    
    pinMode(LED_placa, OUTPUT);
    pinMode(LED_Adicional, OUTPUT);
    pinMode(LED_Adicional_Negativo,OUTPUT);
    pinMode(Centella1,OUTPUT);
    pinMode(Centella1n,OUTPUT);
    pinMode(Centella2,OUTPUT);
    pinMode(Centella2n,OUTPUT);
    pinMode(Centella3,OUTPUT);
    pinMode(Centella3n,OUTPUT);
    digitalWrite(Centella1,LOW);
    digitalWrite(Centella1n,LOW);
    digitalWrite(Centella2,LOW);
    digitalWrite(Centella2n,LOW);
    digitalWrite(Centella3,LOW);
    digitalWrite(Centella3n,LOW);
    digitalWrite(LED_placa, LOW);
    digitalWrite(LED_Adicional, LOW);
    digitalWrite(LED_Adicional_Negativo, LOW);

    Serial.begin(9600); 
    Bluetooth.begin(9600);

    ActivarHombro(true);
    ActivarDeltoide(true);
    ActivarCodo(true);
    ActivarMuneca(true);
    ActivarPalma(true);
    ActivarPinza(true);
    
    //UltimoAngulo = Hombro.read();     
    //MoverHombro(110);

    delay(120);
}

void loop() {

  Bluetooth.listen();

  if(Bluetooth.available())
  {
    DATO = Bluetooth.read();
    Serial.write(DATO);
    Serial.write(" ");
  }

  if(Serial.available())
  {
    DATO = Serial.read();
    Serial.write(DATO);
    Serial.write(" ");
  }
    
  switch(DATO)
  {
    //Valores para ajustar la posicion en pasos de 10 grados. Sugerencias a michcpaulino@gmail.com
    case '0':
      Posicion=0;
    break;
    case '1':
      Posicion=10;
    break;
    case '2':
      Posicion=20;
    break;
    case '3':
      Posicion=30;
    break;
    case '4':
      Posicion=40;
    break;
    case '5':
      Posicion=50;
    break;
    case '6':
      Posicion=60;
    break;
    case '7':
      Posicion=70;
    break;
    case '8':
      Posicion=80;
    break;
    case '9':
      Posicion=90;
    break;
    case 'A':
      Posicion=100;
    break;
    case 'B':
      Posicion=110;
    break;
    case 'C':
      Posicion=120;
    break;
    case 'D':
      Posicion=130;
    break;
    case 'E':
      Posicion=140;
    break;
    case 'F':
      Posicion=150;
    break;
    case 'G':
      Posicion=160;
    break;
    case 'H':
      Posicion=170;
    break;
    case 'I':
      Posicion=180;
    break;
    //Llamadas a las articulaciones. Preguntas a michcpaulino@gmail.com
    case 'J':
      MoverHombro(Posicion);
    break;
    case 'K':
      MoverDeltoide(Posicion);
    break;    
    case 'L':
      MoverCodo(Posicion);
    break;
    case 'M':
      MoverMuneca(Posicion);
    break;
    case 'N':
      MoverPalma(Posicion);
    break;
    case 'O':
      MoverPinza(Posicion);
    break;
    //Ajustes finos. Preguntas a michcpaulino@gmail.com
    case 'P':
      AjusteAbajo();
    break;
    case 'Q':
      AjusteArriba();
    break;
    case 'R':
    //Ver los angulos en que se encuentran los servomotores. Preguntas a michcpaulino@gmail.com
      RevisarMotores(); 
    break;
    case 'S':
    //Parada de emergencia. Preguntas a michcpaulino@gmail.com
      digitalWrite(LED_placa, LOW);
      digitalWrite(LED_Adicional, LOW);
      ActivarHombro(false);
      ActivarDeltoide(false);
      ActivarCodo(false);
      ActivarMuneca(false);
      ActivarPalma(false);
      ActivarPinza(false);
      Serial.println("Todo desactivado.");
    break;
    case 'T':
    //Switch para el LED en la placa.
      volatile bool P_Estado;
      
      P_Estado = digitalRead(LED_placa);
      
      P_Estado = !P_Estado;
      
      digitalWrite(LED_placa, P_Estado);
      
      if(P_Estado == HIGH){
        Serial.println("LED Placa encendido");
      }else{
        Serial.println("LED Placa apagado");
      }
    break;
    case 'U':
      digitalWrite(LED_Adicional, HIGH);
      Serial.println("LED Adicional encendido");
    break;
    default:
      break;
  }
  
  Centella();  
  Limpiar();
  //delay(120);
  //delay(90);
}

void Limpiar()
{
  DATO=' ';
}

void ImprimirAngulo(String Extremidad,int valor)
{
  Serial.print(Extremidad);
  Serial.print(", ");
  Serial.print("angulo: ");
  Serial.println(valor);
}

void Centella()
{
  CambioCentella = !CambioCentella;
  digitalWrite(Centella1, CambioCentella);
  delay(100);
  digitalWrite(Centella2, CambioCentella);
  delay(100);
  digitalWrite(Centella3, CambioCentella);
  delay(100);
}

void ActivarHombro(bool op)
{
  if(op == true){
  //PIN, Angulo minimo en microsegundos y maximo, este ajuste se obtuvo experimentando. Comentarios a michcpaulino@gmail.com
  Hombro.attach(PinHombro,Frec_B_MG996,Frec_A_MG996);
  }else{
    if(Hombro.attached())
    {
      Hombro.detach();
    }
  }
}

void ActivarDeltoide(bool op)
{
  if(op == true){
  //PIN, Angulo minimo en microsegundos y maximo, este ajuste se obtuvo experimentando. Dudas a michcpaulino@gmail.com
  Deltoide.attach(PinDeltoide,Frec_B_MG996,Frec_A_MG996);
  }else{
    if(Deltoide.attached())
    {
      Deltoide.detach();
    }
  }
}

void ActivarCodo(bool op)
{
  if(op == true){
  //PIN, Angulo minimo en microsegundos y maximo, este ajuste se obtuvo experimentando. Mas en michcpaulino@gmail.com
  Codo.attach(PinCodo,Frec_B_MG996,Frec_A_MG996);
  }else{
    if(Codo.attached())
    {
      Codo.detach();
    }
  }
}

void ActivarMuneca(bool op)
{
  if(op == true){
  //PIN, Angulo minimo en microsegundos y maximo, este ajuste se obtuvo experimentando. Que quieres saber a michcpaulino@gmail.com
  Muneca.attach(PinMuneca,Frec_B_MG996,Frec_A_MG996);
  }else{
    if(Muneca.attached())
    {
      Muneca.detach();
    }
  }
}

void ActivarPalma(bool op)
{
  if(op == true){
  //PIN, Angulo minimo en microsegundos y maximo, este ajuste se obtuvo experimentando. Curiosidades a a michcpaulino@gmail.com
  Palma.attach(PinPalma,Frec_B_SG90,Frec_A_SG90);
  }else{
    if(Palma.attached())
    {
      Palma.detach();
    }
  }
}

void ActivarPinza(bool op)
{
  if(op == true){
  //PIN, Angulo minimo en microsegundos y maximo, este ajuste se obtuvo experimentando. Sugerencias a michcpaulino@gmail.com
  Pinza.attach(PinPinza,Frec_B_SG90,Frec_A_SG90);
  }else{
    if(Pinza.attached())
    {
      Pinza.detach();
    }
  }
}

// Metodos para micro ajuste paso por paso de la posicion, presente en los metodos Mover... Preguntas comentarios a michcpaulino@gmail.com
void SoloAngulo()
{
  Ajustar = 0;
  Sentido = 0;
}
void AjusteArriba()
{
  Ajustar = 1;
  Sentido = 1;
}
void AjusteAbajo()
{
  Ajustar = 1;
  Sentido = 0;
}

void MicroPaso()
{
  if(Ajustar == 1 && Sentido == 1)
  {
     Posicion = UltimoAngulo;
     Posicion = Posicion + Variacion;
     SoloAngulo();
  }
  else
  {
    if(Ajustar == 1 && Sentido == 0)
    {
       Posicion = UltimoAngulo;
       Posicion = Posicion - Variacion;
       SoloAngulo();
    } 
  }
}

/*
void RevisarMotores()
{
  String text1 = "Hombro: "+ Hombro.read();
  String text2 = "Deltoide: "+ Deltoide.read(); 
  String text3 = "Codo: "+ Codo.read();
  String text4 = "Munieca: "+ Muneca.read();
  String text5 = "Palma"+ Palma.read();
  String text6 = "Pinza: "+ Pinza.read();
  Serial.println(text1);
  Serial.println(text2);
  Serial.println(text3);
  Serial.println(text4);
  Serial.println(text5);
  Serial.println(text6);
}
*/

void RevisarMotores()
{
  Serial.print("Hombro: ");
  Serial.println(Hombro.read());
  Serial.print("Deltoide: ");
  Serial.println(Deltoide.read());
  Serial.print("Codo: ");
  Serial.println(Codo.read());
  Serial.print("Munieca: ");
  Serial.println(Muneca.read());
  Serial.print("Palma: ");
  Serial.println(Palma.read());
  Serial.print("Pinza: ");
  Serial.println(Pinza.read());
}


void MoverHombro(int Posicion)
{
  Posicion = constrain(Posicion, 0, 180);
  
  if(!Hombro.attached())
  {
    ActivarHombro(true);
  }
  
  UltimoAngulo=Hombro.read(); 

  MicroPaso();

  if(Posicion != UltimoAngulo)
  {
    if(Posicion < UltimoAngulo)
    {
      for(int i = UltimoAngulo; i > Posicion; i--)
      {
        Hombro.write(i);
        delay(velocidad);
      }
    }
    else
    {
      for(int i = UltimoAngulo; i < Posicion; i++)
      {
        Hombro.write(i);
        delay(velocidad);
      }
    }
  }
  ImprimirAngulo("Hombro",Hombro.read());
}

void MoverDeltoide(int Posicion)
{
  Posicion = constrain(Posicion, 0, 180);
  
  if(!Deltoide.attached())
  {
    ActivarDeltoide(true);
  }
  
  UltimoAngulo=Deltoide.read(); 

  MicroPaso();

  if(Posicion != UltimoAngulo)
  {
    if(Posicion < UltimoAngulo)
    {
      for(int i = UltimoAngulo; i > Posicion; i--)
      {
        Deltoide.write(i);
        delay(velocidad);
      }
    }
    else
    {
      for(int i = UltimoAngulo; i < Posicion; i++)
      {
        Deltoide.write(i);
        delay(velocidad);
      }
    }
  }
  ImprimirAngulo("Deltoide",Deltoide.read());
}

void MoverCodo(int Posicion)
{
  Posicion = constrain(Posicion, 0, 180);
  
  if(!Codo.attached())
  {
    ActivarCodo(true);
  }
  
  UltimoAngulo=Codo.read(); 

  MicroPaso();

  if(Posicion != UltimoAngulo)
  {
    if(Posicion < UltimoAngulo)
    {
      for(int i = UltimoAngulo; i > Posicion; i--)
      {
        Codo.write(i);
        delay(velocidad);
      }
    }
    else
    {
      for(int i = UltimoAngulo; i < Posicion; i++)
      {
        Codo.write(i);
        delay(velocidad);
      }
    }
  }
  ImprimirAngulo("Codo",Codo.read());
}

void MoverMuneca(int Posicion)
{
  Posicion = constrain(Posicion, 0, 180);
  
  if(!Muneca.attached())
  {
    ActivarMuneca(true);
  }
  
  UltimoAngulo=Muneca.read();

  MicroPaso();

  if(Posicion != UltimoAngulo)
  {
    if(Posicion < UltimoAngulo)
    {
      for(int i = UltimoAngulo; i > Posicion; i--)
      {
        Muneca.write(i);
        delay(velocidad);
      }
    }
    else
    {
      for(int i = UltimoAngulo; i < Posicion; i++)
      {
        Muneca.write(i);
        delay(velocidad);
      }
    }
  }
  ImprimirAngulo("Munieca",Muneca.read());
}

void MoverPalma(int Posicion)
{
  Posicion = constrain(Posicion, 0, 180);
  
  if(!Palma.attached())
  {
    ActivarPalma(true);
  }
  
  UltimoAngulo=Palma.read();

  MicroPaso(); 

  if(Posicion != UltimoAngulo)
  {
    if(Posicion < UltimoAngulo)
    {
      for(int i = UltimoAngulo; i > Posicion; i--)
      {
        Palma.write(i);
        delay(velocidad);
      }
    }
    else
    {
      for(int i = UltimoAngulo; i < Posicion; i++)
      {
        Palma.write(i);
        delay(velocidad);
      }
    }
  }
  ImprimirAngulo("Palma",Palma.read());
}

void MoverPinza(int Posicion)
{
  Posicion = constrain(Posicion, 0, 180);
  
  if(!Pinza.attached())
  {
    ActivarPinza(true);
  }
  
  UltimoAngulo=Pinza.read();

  MicroPaso(); 

  if(Posicion != UltimoAngulo)
  {
    if(Posicion < UltimoAngulo)
    {
      for(int i = UltimoAngulo; i > Posicion; i--)
      {
        Pinza.write(i);
        delay(velocidad);
      }
    }
    else
    {
      for(int i = UltimoAngulo; i < Posicion; i++)
      {
        Pinza.write(i);
        delay(velocidad);
      }
    }
  }
  ImprimirAngulo("Pinza",Pinza.read());
}
