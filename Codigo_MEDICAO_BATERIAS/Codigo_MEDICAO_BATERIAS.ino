//===================================== Bibliotecas E Definições =====================================
#include <SPI.h>            //Biblioteca necessária para comunicação SPI
#include <SD.h>             //Biblioteca necessária para comunicação SD card
#include <Wire.h>           //Biblioteca para comunicação I2C com módulo RTC
#include <LiquidCrystal.h>  //Biblioteca para display 16x2
#include <avr/wdt.h>        //Biblioteca referente ao Timer WatchDog

//============================================= HARDWARE =============================================
#define DS1307_ADDRESS 0x68
#define Bateria_1   14      //A0
#define Bateria_2   15      //A1
#define Bateria_3   16      //A2
#define Bateria_4   17      //A3
#define CS          10      //D10
#define MOSI        11      //D11
#define MISO        12      //D12
#define SCK         13      //D13


//============================================= Variáveis ============================================
const int SelecaoChip = 4;
const int Amostras_Filtro = 30;
unsigned int Timer_salvar = 0;
unsigned int Timer_somador = 0;
unsigned int SegundoAnterior_salvar = 0;
unsigned int SegundoAnterior_somador = 0;

unsigned int TempoParaSalvar = 1;
unsigned int TempoSomador = 5;
float Bateria1 = 0.00f;
float Bateria2 = 0.00f;
float Bateria3 = 0.00f;
float Bateria4 = 0.00f;
String LinhaDados = "";
byte zero = 0x00;

//Do relogio RTC
  int segundo =0;
  int minuto = 0;                //0~59
  int hora = 0;                  //Formato 24 horas
  int diasemana = 0;             //0-6 -> Domingo - Sábado
  int dia = 0;
  int mes = 0;
  int ano = 0;

//============================================== Funções ===========================================
float LeituraBateria(int porta) {
  unsigned int media = 0;
  float tensao = 0.00f;
  
  for(int i = 0; i < Amostras_Filtro; i++){
    media += analogRead(porta);
    delay(1);
  }
  tensao = float(media)/Amostras_Filtro;
  tensao = tensao*5.08/1023;
  return tensao;  
}

void leituraEscrita() {
  LinhaDados = "";

  float Bateria1 = LeituraBateria(Bateria_1);
  LinhaDados += String(Bateria1, 3);
  LinhaDados += "V, ";
  float Bateria2 = LeituraBateria(Bateria_2);
  LinhaDados += String(Bateria2, 3);
  LinhaDados += "V, ";
  float Bateria3 = LeituraBateria(Bateria_3);
  LinhaDados += String(Bateria3, 3);
  LinhaDados += "V, ";
  float Bateria4 = LeituraBateria(Bateria_4);
  LinhaDados += String(Bateria4, 3);
  LinhaDados += "V -- ";
  LinhaDados += String(dia);
  LinhaDados += "/";
  LinhaDados += String(mes);
  LinhaDados += "/";
  LinhaDados += String(ano);
  LinhaDados += " ";
  LinhaDados += String(hora);
  LinhaDados += ":";
  LinhaDados += String(minuto);
  LinhaDados += ":";
  LinhaDados += String(segundo);
  
  Serial.println(LinhaDados);
  delay(1000);
}

byte decToBcd(byte valor) {
  return ( (valor/10*16) + (valor%10));
}

byte bcdToDec(byte val)  {
  return ( (val/16*10) + (val%16) );
}

void LeituraHora() {  
  Wire.beginTransmission(DS1307_ADDRESS);       //Inicio a comunição I2C
  Wire.write(zero);                             //Inicio
  Wire.endTransmission();                       

  Wire.requestFrom(DS1307_ADDRESS, 7);          //Pego todos os dados de hora

  //Atribuo os respectivos dados as suas variáveis
  segundo = bcdToDec(Wire.read());
  minuto = bcdToDec(Wire.read());
  hora = bcdToDec(Wire.read() & 0b111111);    //Formato 24 horas
  diasemana = bcdToDec(Wire.read());             //0-6 -> Domingo - Sábado
  dia = bcdToDec(Wire.read());
  mes = bcdToDec(Wire.read());
  ano = bcdToDec(Wire.read());

  //Meu 'timer' para salvar dados
  if(Timer_salvar < TempoParaSalvar) {
    if(SegundoAnterior_salvar != segundo) {
    SegundoAnterior_salvar = segundo;
    Timer_salvar += 1;
    }
    if(Timer_salvar == TempoParaSalvar){
    leituraEscrita();
    Timer_salvar = 0;
  }
  
  //'Timer' para mudar o estado do somador
  if(Timer_somador < TempoSomador) {
    if(SegundoAnterior_somador != segundo) {
    SegundoAnterior_somador = segundo;
    Timer_somador += 1;
    }
    if(Timer_somador == TempoSomador){
    Serial.println("Mudei somador");
    Timer_somador = 0;
  }
 }
}
}

void AtualizaRTC() {
   byte segundo   =         00;   //0~59
   byte minuto    =         00;   //0~59
   byte hora      =         00;   //0~23
   byte diasemana =          2;   //1~7
   byte dia       =          3;   //1~31
   byte mes       =         12;   //1~12
   byte ano       =         13;   //1~13

   Wire.beginTransmission(DS1307_ADDRESS);
   Wire.write(zero);

   Wire.write(decToBcd(segundo));
   Wire.write(decToBcd(minuto));
   Wire.write(decToBcd(hora));
   Wire.write(decToBcd(diasemana));
   Wire.write(decToBcd(dia));
   Wire.write(decToBcd(mes));
   Wire.write(decToBcd(ano));
  
   Wire.write(zero); 
  
   Wire.endTransmission();
}



//============================================== Programa ==========================================
void setup() {
  Serial.begin(9600);
  Wire.begin();
  
  pinMode(Bateria_1, INPUT);
  pinMode(Bateria_2, INPUT);
  pinMode(Bateria_3, INPUT);
  pinMode(Bateria_4, INPUT);
  pinMode(2, OUTPUT);

                                    
//  while(!SD.begin(SelecaoChip)) {
//     Serial.println("Erro, cartão de memória não inserido.");
//     delay(1500);
//  }
//  Serial.println("Cartão encontrado.");
}

void loop() {
  LeituraHora();
  digitalWrite(13, !digitalRead(13));
  delay(500);
}


