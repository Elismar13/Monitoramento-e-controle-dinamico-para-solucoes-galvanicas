//===================================== Bibliotecas E Definições =====================================
#include <EEPROM.h>         //EEPROM
#include <SPI.h>            //Biblioteca necessária para comunicação SPI
#include <SD.h>             //Biblioteca necessária para comunicação SD card
#include <Wire.h>           //Biblioteca para comunicação I2C com módulo RTC
#include <LiquidCrystal.h>  //Biblioteca para display 16x2
//#include <avr/wdt.h>        //Biblioteca referente ao Timer WatchDog

//============================================= HARDWARE =============================================
//Constantes
#define DS1307_ADDRESS      0x68
#define AtualizacaoDisplay  120
#define AtrasoBotoes        300
#define EnderecoTempoSalvar 0
#define EnderecoCalibracao  1

//Pinos
#define rs          2       //D2
#define en          3       //D3
#define d4          4       //D4
#define d5          5       //D5
#define d6          6       //D6
#define d7          7       //D7

#define up          8       //D8
#define select      9       //D9
#define mais        0       //D0
#define menos       1       //D1

#define SelecaoChip 10      //D10 -Chip Select
#define MOSI        11      //D11 - MOSI
#define MISO        12      //D12 - MISO
#define SCK         13      //D13 - SCk

#define Bateria_1   14      //A0
#define Bateria_2   15      //A1
#define Bateria_3   16      //A2
#define Bateria_4   17      //A3

//============================================= Variáveis ============================================
LiquidCrystal lcd (rs, en, d4, d5, d6, d7); //Crio o objeto lcd com seus respectivos pinos
const byte SelecionadorOpcao[8] =                 //Lista para o caractere especial (O quadrado)
{
  0b00000,
  0b00000,
  0b01110,
  0b01110,
  0b01110,
  0b01110,
  0b00000,
  0b00000
};

//Caracteres especiais
  const byte C_Cedilhado[8] {                       //Lista para o caractere especial ("Ç")
    0b00000,
    0b01110,
    0b10000,
    0b10000,
    0b10000,
    0b01110,
    0b00100,
    0b00100
  };
  
  const byte O_Til[8] {                             //Lista para o caractere especial ("Õ")
    0b01110,
    0b00000,
    0b01110,
    0b10001,
    0b10001,
    0b10001,
    0b01110,
    0b00000
  };
  
  const byte A_Til[8] {                             //Lista para o caractere especial ("Ã")
    0b01110,
    0b00000,
    0b01110,
    0b00001,
    0b01111,
    0b10001,
    0b01111,
    0b00000
  };

  const byte A_Agudo[8] {                             //Lista para o caractere especial ("Ã")
    0b00010,
    0b00100,
    0b01110,
    0b00001,
    0b01111,
    0b10001,
    0b01111,
    0b00000
  };
//Seletores para os menus
byte SeletorPrincipal = 0;
byte SeletorDadosBateria = 1;
byte SeletorConfiguracoes = 0;

//De tempo
byte Timer_salvar = 0;
byte HoraAnterior_salvar = 0;
byte MinutoAnterior_salvar = 0;
byte TempoParaSalvar = 1;

//Calibracao do filtro ADC
byte CalibracaoADC = 20;    //de 0 à 40. 20 Seria é o equivalente de '0' 
float TensaoADC = 5.00;

//Número de amostras para o filtro ADC. Quanto maior, melhor, mas terá impacto no desempenho do sistema.
byte Amostras_Filtro = 30;

//Das baterias
float Bateria1 = 0.00f;
float Bateria2 = 0.00f;
float Bateria3 = 0.00f;
float Bateria4 = 0.00f;
float Teste[20] = {0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f};


//Do relogio RTC
  byte segundo = 0;               //0~59
  byte minuto = 0;                //0~59
  byte hora = 0;                  //Formato 24 horas
  byte diasemana = 0;             //0-6 -> Domingo - Sábado
  byte dia = 0;                   //Dependendo do mês, de 1~31
  byte mes = 0;                   //1~12
  byte ano = 0;                   //0~99

//=================================== Inicio do Programa ==========================================
void setup() {
  Wire.begin();
  /*     DEBUG  
  Serial.begin(9600);
  */
  //Botões de interação
  pinMode(up, INPUT_PULLUP);
  pinMode(mais, INPUT_PULLUP);
  pinMode(menos, INPUT_PULLUP);
  pinMode(select, INPUT_PULLUP);

  //Pinos ADC para as baterias
  pinMode(Bateria_1, INPUT);
  pinMode(Bateria_2, INPUT);
  pinMode(Bateria_3, INPUT);
  pinMode(Bateria_4, INPUT);

  //pinMode(en, OUTPUT);

  //Criação de caracteres especiais
  lcd.createChar(1, SelecionadorOpcao);
  lcd.createChar(2, C_Cedilhado);
  lcd.createChar(3, O_Til);
  lcd.createChar(4, A_Til);
  lcd.createChar(5 , A_Agudo);

  //Mensagem inicial display
  lcd.clear();
  lcd.begin(16, 2);
  //Boas vindas
  lcd.setCursor(2, 0);
  lcd.print("Iniciando");
  for(byte i = 0; i < 3; i++) {
    lcd.write('.');
    delay(1500);
  }

  //Verifico e atualizo o tempo
  if(EEPROM.read(EnderecoTempoSalvar) == 0) {
    TempoParaSalvar = 1;
    EEPROM.update(EnderecoTempoSalvar, 1); 
  }
  TempoParaSalvar = EEPROM.read(EnderecoTempoSalvar);
  
  //Enquanto o cartão não for inserido eu mostro o erro                     
  while(!SD.begin(SelecaoChip)) {
     erroAbrirCartao();
  }
  //Se for encontrado:
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("Cartao SD ");
  lcd.setCursor(2, 1);
  lcd.print("encontrado.");  
  delay(2000);


  //Verifico e atualizo a calibracao
  CalibracaoADC = EEPROM.read(EnderecoCalibracao);
  if(CalibracaoADC != 20) {
    for(byte i = 0; i < (abs(CalibracaoADC-20)); i++) {
      if(CalibracaoADC > 20) {
        TensaoADC += 0.001;
      }
      else {
        TensaoADC -= 0.001;
      }
    }
  }
}

void loop() {
  menuPrincipal();
}
