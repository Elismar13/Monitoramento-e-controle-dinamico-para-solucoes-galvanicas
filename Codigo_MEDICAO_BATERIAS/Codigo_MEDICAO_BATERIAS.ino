//===================================== Bibliotecas E Definições =====================================
#include <SPI.h>            //Biblioteca necessária para comunicação SPI
#include <SD.h>             //Biblioteca necessária para comunicação SD card

#define Bateria_1 A0
#define Bateria_2 A1
#define Bateria_3 A2
#define Bateria_4 A3

//============================================= Variáveis ============================================
const int SelecaoChip = 4;

//============================================== Funções ===========================================
float LeituraBateria(int porta) {
  float tensao = float((analogRead(porta)*(5/1023));
  return tensao;  
}


void setup() {
  Serial.begin(9600);
  while(!SD.begin(SelecaoChip)) {
     Serial.println("Erro, cartão de memória não inserido.");
     delay(1500);
  }
  Serial.println("Cartão encontrado.");
}

void loop() {
  String LinhaDados = "";

  float Bateria1 = LeituraBateria(Bateria_1);
  float Bateria2 = LeituraBateria(Bateria_2);
  float Bateria3 = LeituraBateria(Bateria_3);
  float Bateria4 = LeituraBateria(Bateria_4);

}
