//===================================== Bibliotecas E Definições =====================================
//#include <SPI.h>            Biblioteca necessária para comunicação SPI
//#include <SD.h>             Biblioteca necessária para comunicação SD card

#define Bateria_1 14
#define Bateria_2 15
#define Bateria_3 16
#define Bateria_4 17

//============================================= Variáveis ============================================
const int SelecaoChip = 4;

//============================================== Funções ===========================================
float LeituraBateria(int porta) {
  float tensao = (float(analogRead(porta))*5/1023);
  Serial.print(analogRead(porta));
  Serial.print(" , ");
  Serial.println(tensao,7);
  return tensao;  
}


void setup() {
  Serial.begin(9600);
  pinMode(Bateria_1, INPUT);
  pinMode(Bateria_2, INPUT);
  pinMode(Bateria_3, INPUT);
  pinMode(Bateria_4, INPUT);

//  while(!SD.begin(SelecaoChip)) {
//     Serial.println("Erro, cartão de memória não inserido.");
//     delay(1500);
//  }
//  Serial.println("Cartão encontrado.");
}

void loop() {
  String LinhaDados = "";

  float Bateria1 = LeituraBateria(Bateria_1);
  LinhaDados += String(Bateria1);
  LinhaDados += ", ";
  float Bateria2 = LeituraBateria(Bateria_2);
  LinhaDados += String(Bateria2);
  LinhaDados += ", ";
  float Bateria3 = LeituraBateria(Bateria_3);
  LinhaDados += String(Bateria3);
  LinhaDados += ", ";
  float Bateria4 = LeituraBateria(Bateria_4);
  LinhaDados += String(Bateria3);
  LinhaDados += ", ";
  Serial.println(LinhaDados);
  delay(1000);
}
