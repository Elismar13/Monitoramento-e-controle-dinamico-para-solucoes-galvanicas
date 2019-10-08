//===================================== Bibliotecas E Definições =====================================
#include <EEPROM.h>         //EEPROM
#include <SPI.h>            //Biblioteca necessária para comunicação SPI
#include <SD.h>             //Biblioteca necessária para comunicação SD card
#include <Wire.h>           //Biblioteca para comunicação I2C com módulo RTC
#include <LiquidCrystal.h>  //Biblioteca para display 16x2
#include <avr/wdt.h>        //Biblioteca referente ao Timer WatchDog

//============================================= HARDWARE =============================================
//Constantes
#define DS1307_ADDRESS 0x68
#define AtualizacaoDisplay  150
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
byte MinutoAnterior_salvar = 0;
byte TempoParaSalvar = 1;

//Número de amostras para o filtro ADC. Quanto maior, melhor, mas terá impacto no desempenho do sistema.
float TensaoCalibracao = 5.00;
byte Amostras_Filtro = 30;

//Das baterias
float Bateria1 = 0.00f;
float Bateria2 = 0.00f;
float Bateria3 = 0.00f;
float Bateria4 = 0.00f;
float Teste[20] = {0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f};

byte zero = 0x00;

//Do relogio RTC
  byte segundo = 0;               //0~59
  byte minuto = 0;                //0~59
  byte hora = 0;                  //Formato 24 horas
  byte diasemana = 0;             //0-6 -> Domingo - Sábado
  byte dia = 0;                   //Dependendo do mês, de 1~31
  byte mes = 0;                   //1~12
  byte ano = 0;                   //0~99



//************ FUNÇÕES DA PARTE DE LEITURA E ESCRITA DOS DADOS NO CARTAO SD *********************

//Função para ler os níveis da baterias
float LeituraBateria(int porta) {
  unsigned int media = 0;
  float tensao = 0.00f;
  
  for(int i = 0; i < Amostras_Filtro; i++){
    media += analogRead(porta);
    delayMicroseconds(1000);
  }
  tensao = float(media)/Amostras_Filtro;
  tensao = tensao*5.05/1023;
  return tensao;  
}

//Função "link" para os níveis das baterias 
void NiveisBaterias() {
  Bateria1 = LeituraBateria(Bateria_1);
  Bateria2 = LeituraBateria(Bateria_2);
  Bateria3 = LeituraBateria(Bateria_3);
  Bateria4 = LeituraBateria(Bateria_4);
}

//Função para ler os dados e criar um String
void leituraEscrita() {
  /*              PADRÃO DA ESCRITA 
   *  0.000V, 0.000V, 0.000V, 0.000V -- 00/00/0000 00:00:00
   */
   
  NiveisBaterias();
  String LinhaDados = "";

  LinhaDados += String(Bateria1, 3);
  LinhaDados += "V, ";
  LinhaDados += String(Bateria2, 3);
  LinhaDados += "V, ";
  LinhaDados += String(Bateria3, 3);
  LinhaDados += "V, ";
  LinhaDados += String(Bateria4, 3);
  LinhaDados += "V -- ";

  //Dia
  if(dia < 10) LinhaDados += '0';     //Se o dia for menor que 10, adiciono um zero à frente.
  LinhaDados += String(dia);
  LinhaDados += "/";

  //Mês
  if(mes < 10) LinhaDados += '0';     //Se o mes for menor que 10, adiciono um zero à frente.
  LinhaDados += String(mes);
  LinhaDados += "/";

  //Ano
  LinhaDados += "20";
  if(ano < 10) LinhaDados += '0';     //Se o ano for menor que 10, adiciono um zero à frente.
  LinhaDados += String(ano);
  LinhaDados += " ";

  //Hora
  if(hora < 10) LinhaDados += '0';    //Se a hora for menor que 10, adiciono um zero à frente
  LinhaDados += String(hora);
  LinhaDados += ":";

  //Minuto
  if(minuto < 10) LinhaDados += '0';  //Se o minuto for menor que 10, adiciono um zero à frente
  LinhaDados += String(minuto);
  LinhaDados += ":";

  //Segundo
  if(segundo < 10) LinhaDados += '0'; //Se o segundo for menor que 10, adiciono um zero à frente
  LinhaDados += String(segundo);

  //Por fim, escrevo os dados.
  escreverCartao(LinhaDados);
  delay(1000);
}

//Conversões necessárias para leitura e escrita no relógio RTC
byte decToBcd(byte valor) {
  return ( (valor/10*16) + (valor%10) );
}
byte bcdToDec(byte val)  {
  return ( (val/16*10) + (val%16) );
}


//Leitura da hora e atualização das variáveis
void LeituraHora() {  
  Wire.beginTransmission(DS1307_ADDRESS);       //Inicio a comunição I2C
  Wire.write(zero);                             //Inicio
  Wire.endTransmission();                       

  Wire.requestFrom(DS1307_ADDRESS, 7);          //Pego todos os dados de hora

  //Atribuo os respectivos dados as suas variáveis
  segundo = bcdToDec(Wire.read());
  minuto = bcdToDec(Wire.read());
  hora = bcdToDec(Wire.read() & 0b111111);       //Formato 24 horas
  diasemana = bcdToDec(Wire.read());             //0-6 -> Domingo - Sábado
  dia = bcdToDec(Wire.read());
  mes = bcdToDec(Wire.read());
  ano = bcdToDec(Wire.read());

  //Meu 'timer' para salvar dados
  if(Timer_salvar < TempoParaSalvar) {
    if(MinutoAnterior_salvar != minuto) {
      MinutoAnterior_salvar = minuto;
      Timer_salvar += 1;
      }
    if(Timer_salvar == TempoParaSalvar){
    leituraEscrita();
    Timer_salvar = 0;
  }
 }
}

//Atualização dos dados de horário no RTC
void AtualizaRTC(byte minuto, byte hora, byte dia, byte mes, byte ano, byte diasemana) {
   byte segundo   =         00;   //0~59

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

//Função para abrir o arquivo e escrever os dados
void escreverCartao(String Dados) {
  File dataFile = SD.open("dadosbaterias.txt", FILE_WRITE);

  //Mensagem "Salvar dados"
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Salvando Dados");
  lcd.setCursor(2, 1);
  lcd.print("No cartao");
  for(byte i = 0; i<3; i++) {
    lcd.write('.');
    delay(1000);
  }
  if(dataFile) {             //Se tudo estiver configurado com o cartão
    dataFile.println(Dados); //Escrevo a linha com os dados
    dataFile.close();        //Fecho o arquivo (para evitar erros)
    salvoComSucesso();
  }
  else {
    erroGeral();
  }
}


//**************************************** MENUS DO LCD ****************************************

//******************** FUNCOES REFERENTES A MENUS AUXILIARES **************************
//Mensagem de "salvo com sucesso"
void salvoComSucesso() {
  lcd.clear();            
  lcd.setCursor(3, 0);
  lcd.print("Salvo com");
  lcd.setCursor(4, 1);
  lcd.print("Sucesso!");
  //Mantenho a mensagem por 1 segundo
  delay(1000);
  
  lcd.clear();
  return;
}

//Mensagem de erro geral
void erroGeral() {
  lcd.clear();  
  lcd.setCursor(4, 0);
  lcd.print("Erro ao");
  lcd.setCursor(4, 1);
  lcd.print("Salvar!"); 

  //Mantenho a mensagem por 1,5 segundo(s)
  delay(1500);
  
  lcd.clear();
  return;         
}

void erroCartao() {
  lcd.clear();  
  lcd.setCursor(1, 0);
  lcd.print("Erro ao salvar");
  lcd.setCursor(3, 1);
  lcd.print("no cartao SD!"); 
   
  //Mantenho a mensagem por 1 segundo
  delay(1000);
  
  lcd.clear();
  return;     
}

void erroAbrirCartao() {
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("Cartao SD ");
  lcd.setCursor(1, 1);
  lcd.print("Nao encontrado.");  
  //Mantenho a mensagem por 1 segundo
  delay(1500);
  
  lcd.clear();
  return;   
}

//************************* FUNÇÕES DO MENU PRINCIPAL *********************************
//MenuPrincipal do programa
void menuPrincipal() {
    //Necessario para ficar neste menu
  while(true) {               
    //Antes de tudo, limpo todo conteúdo do display e verifico a hora e o timer
    lcd.clear(); 
    //LeituraHora();                 
    
    //Leitura de botões e atualização de variáveis
    if(!digitalRead(mais) == HIGH) {
      SeletorPrincipal += 1;
      delay(AtrasoBotoes);
    } 
    else if(!digitalRead(menos) == HIGH) {
      SeletorPrincipal -= 1; 
      delay(AtrasoBotoes);
    }
  
    //Limites para o menu principal
    if(SeletorPrincipal > 1){
      SeletorPrincipal = 0;
    }
    else if(SeletorPrincipal < 0) {
      SeletorPrincipal = 1;
    }
  
    //Escrever a mensagem do menu principal
    lcd.setCursor(1, 0);
    lcd.print("Menu Principal");
  
    //Alterando a posição do cursor (">") para informar ao usuário qual menu ele deseja escolher
    if(SeletorPrincipal == 0) {
      lcd.setCursor(9, 1);
      lcd.print(" Config");   
      lcd.setCursor(0, 1);
      lcd.print(">Niveis  ");
    }
    else {
      lcd.setCursor(0, 1);
      lcd.print(" Niveis ");
      lcd.setCursor(9, 1);
      lcd.print(">Config");   
    }
  
    //Quando o botao de selecionar menu for pressionado, o programa entrará no menu onde o cursor está
    if(!digitalRead(select) == HIGH) {
      delay(AtrasoBotoes);
      switch(SeletorPrincipal) {
        case 0: 
          dadosBateria();       //Menu dos dados da bateria
          break;
        case 1:
          configuracoes();      //Menu das configurações de hora, relogio, sistema.
          break;    
      }
    }
    delay(AtualizacaoDisplay);  //Atraso para Atualizar o display
  }
} 


//************************* FUNÇÕES PARA O MENU DE DADOS BATERIA *********************************
void dadosBateria() {
  while(!digitalRead(up) == LOW) {          //Loop necessário para o programa ficar nesse menu
    //Leitura de botões e atualização de variáveis
    if(!digitalRead(mais) == HIGH) {
        SeletorDadosBateria += 1;
        delay(AtrasoBotoes);
    } else if(!digitalRead(menos) == HIGH) {
        SeletorDadosBateria -= 1; 
        delay(AtrasoBotoes);
    }
  
    //Limites no menu da bateria
    if(SeletorDadosBateria < 1) {
      SeletorDadosBateria = 4;
    }
    else if(SeletorDadosBateria > 4) {
      SeletorDadosBateria = 1;
    }
    
    //Trocar entre os menus
      switch(SeletorDadosBateria) {
      case 1:
        statusBat(Bateria1, 1);
        break;
      case 2:
        statusBat(Bateria2, 2); 
        break;
      case 3:
        statusBat(Bateria3, 3);
        break;
      case 4:
        statusBat(Bateria4, 4);
        break;
    }
  }
}

//Função para mostrar o dado da bateria selecionada
void statusBat(float NivelBateria, int ValorBat) {
  String TensaoEmString = "";
  
  while(true) {
     LeituraHora();          //Verifico o timer e consequentemente a hora
     NiveisBaterias();       //Leio os níveis das 4 baterias
     
     lcd.clear();           //Limpar o que tem na tela
     lcd.setCursor(3, 0);   //Posiciono o cursor para printar o número da bateria selecionada
     
  //Mostrar o status das bateria selecionada
     if(ValorBat == 1) {
        lcd.print("Bateria 1:");
        NivelBateria = Bateria1;
     }
     else if(ValorBat == 2) {
        lcd.print("Bateria 2:");
        NivelBateria = Bateria2;
     }
     else if(ValorBat == 3) 
     {        
        lcd.print("Bateria 3:");
        NivelBateria = Bateria3;
     }
     else if(ValorBat == 4) {
        lcd.setCursor(3, 0);
        lcd.print("Bateria 4:");
        NivelBateria = Bateria4;
      }


   //Mostrar o valor da bateria em VOLTS 
    lcd.setCursor(5, 1);
    if(NivelBateria >= 1.00) {
        lcd.print(NivelBateria, 3);
        lcd.setCursor(10, 1);
        lcd.write('V');
      }

    //Mostro o valor da bateria em miliVolts
    else {
      lcd.setCursor(5, 1);
      TensaoEmString = String(NivelBateria, 4);
      for(byte i = 2; i < 5; i++) {
        lcd.write(TensaoEmString[i]);
      }
      lcd.print(" mV");
     }

    //Verifico os botões
    if(!digitalRead(up) == HIGH) { 
      delay(AtrasoBotoes);
      break;
      }
    if(!digitalRead(mais) == HIGH) {
      SeletorDadosBateria += 1;
      delay(AtrasoBotoes);
      break;
     }
    if(!digitalRead(menos) == HIGH) {
      SeletorDadosBateria -= 1;
      delay(AtrasoBotoes);
      break;
     }
    delay(AtualizacaoDisplay);     //Atraso para Atualizar o display
   }
}

//************************** FUNÇÕES PARA O MENU DE CONFIGURAÇÕES **********************************
void configuracoes() {
    while(!digitalRead(up) == LOW) {          //Loop necessário para o programa ficar nesse menu
      lcd.clear();
      //Printar as palavras
      lcd.setCursor(2, 0);
      lcd.print("Configuracoes");
      
      lcd.setCursor(1, 1);
      lcd.print("Arquivo");
      lcd.setCursor(12, 1);
      lcd.print("Hora");

    
      if(!digitalRead(mais) == HIGH) {
        SeletorConfiguracoes += 1;
        delay(AtrasoBotoes);
      } 
      else if(!digitalRead(menos) == HIGH) {
        SeletorConfiguracoes -= 1; 
        delay(AtrasoBotoes);
      }
    
      //Limites para o menu de configuracoes
      if(SeletorConfiguracoes > 1){
        SeletorConfiguracoes = 0;
      }
      else if(SeletorConfiguracoes < 0) {
        SeletorConfiguracoes = 1;
      }
    
      if(SeletorConfiguracoes == 0) {
          lcd.setCursor(0, 1);
          lcd.print(">");
          lcd.setCursor(11, 1);
          lcd.write(' ');
      }
      else {
          lcd.setCursor(0, 1);
          lcd.print(' ');
          lcd.setCursor(11, 1);
          lcd.write('>');
      }

      if(!digitalRead(select) == HIGH) {
        delay(AtrasoBotoes);
        switch(SeletorConfiguracoes) {
          case 0:
            configurarArquivo();
            break;
          case 1:
            mostrarHora();
            break;
          case 2: 
            //calibracao();
            break;
        }
      }
     delay(AtualizacaoDisplay);     //Atraso para Atualizar o display
    }
}

void configurarArquivo() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Atualiza arquivo");
  lcd.setCursor(0, 1);
  lcd.print("a cada: ");

  while(true) {
    delay(AtualizacaoDisplay);     //Atraso para Atualizar o display
    if(!digitalRead(select) == HIGH) {
      delay(AtrasoBotoes);
      break;
    }
    if(!digitalRead(menos) == HIGH) {
      delay(AtrasoBotoes);
      TempoParaSalvar--;
    }
    if(!digitalRead(mais) == HIGH) {
      delay(AtrasoBotoes);
      TempoParaSalvar++;
    }
    if(TempoParaSalvar < 1) {
      TempoParaSalvar = 60;
    }
    else if(TempoParaSalvar > 60) {
      TempoParaSalvar = 1;
    }

    lcd.setCursor(9, 1);
    if(TempoParaSalvar < 10) {
      lcd.write('0');
      lcd.setCursor(10, 1);
    }
    lcd.print(TempoParaSalvar);
    lcd.print(" Min");
  }
  EEPROM.update(EnderecoTempoSalvar, TempoParaSalvar);      //Atualizo o intervalo entre cada vez que o conteúdo é salvo.
  salvoComSucesso();
  return;
}

void mostrarHora() {
  bool ManterMenu = false;
  bool AlterarHora = true;
  //String DadoDoTempo = "";
  
  lcd.clear();
  delay(300);

  while(!ManterMenu) {
    LeituraHora();
    lcd.setCursor(1 ,0);
    lcd.print("Hor");
    lcd.write((byte)5);
    lcd.print("rio atual:");
    lcd.setCursor(0, 1);
    String DataHora = "";
    
    if(hora < 10) {               //Escrever a hora no display
      DataHora += '0';
    }
    DataHora += String(hora);
    DataHora += ':';
       
    if(minuto < 10) {             //Escrever o minuto no display
      DataHora += '0';
    }
    DataHora += String(minuto);
    DataHora += '-';

    if(dia < 10) {                //Escrever o dia no display
      DataHora += '0';
    }
    DataHora += String(dia); 
    DataHora += ':';
  
    if(mes < 10) {                 //escrever o mês no LCD
      DataHora += '0';      
    }
    DataHora += String(mes);
    DataHora += ":20";

    if(ano < 10) {
      DataHora += '0';
    }
    DataHora += String(ano);

    //Por fim, imprimo a data no display
    lcd.print(DataHora);
    
    if(!digitalRead(select) == HIGH) {
      ManterMenu = true;
      delay(AtrasoBotoes);
    }
    if(!digitalRead(up) == HIGH) {
      ManterMenu = false;
      delay(AtrasoBotoes);
      return;  
    }
    delay(AtualizacaoDisplay);     //Atraso para Atualizar o display
  }

  //Após verificar a hora, pergunto ao usuário se ele deseja alterar a hora
  lcd.clear();
  //Reaproveito a variável "ManterMenu" dessa vez no proxímo menu
  ManterMenu = false;              
  
  while(!ManterMenu) {
    lcd.setCursor(0, 0);
    lcd.print("Configurar Data?");
    
    lcd.setCursor(3, 1);
    lcd.print("SIM");
    lcd.setCursor(10, 1);
    lcd.print("NAO");
    
    if(AlterarHora) {
      lcd.setCursor(2, 1);
      lcd.write('>');
      lcd.setCursor(9, 1);
      lcd.write(' ');
    }
    else {
      lcd.setCursor(2, 1);
      lcd.write(" ");
      lcd.setCursor(9, 1);
      lcd.write(">");
    }

    if((!digitalRead(mais) == HIGH) || (!digitalRead(menos) == HIGH)) {
      AlterarHora = !AlterarHora;
      delay(AtrasoBotoes);
    }
    if(!digitalRead(select) == HIGH) {
      ManterMenu = true;
      delay(AtrasoBotoes);
    }
    delay(AtualizacaoDisplay);     //Atraso para Atualizar o display
  }
  lcd.clear();

  
  if(AlterarHora) {
    funcaoAlterarHora();
  }
}
/*
void calibracao() {
  byte ValorCalibracao = 127;

  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Siga os passos");
  lcd.setCursor(2, 1);
  lcd.print("do manual!");
  delay(3000);
  
  while(true) {
    //Verificacao dos botões
    if(!digitalRead(up) == HIGH) {
      delay(AtrasoBotoes);
      return;
    }
    if(!digitalRead(select) == HIGH) {
      delay(AtrasoBotoes);
      break;
    }
    if(!digitalRead(mais) == HIGH) {
      ValorCalibracao += 1; 
      delay(AtrasoBotoes);
    }
    if(!digitalRead(menos) == HIGH) {
      ValorCalibracao -= 1;
      delay(AtrasoBotoes);
    }

    lcd.clear();
    
    lcd.print(2, 0);
    lcd.print("Calibra");
    lcd.write((byte)2);
    lcd.write((byte)3);
    lcd.write('o');

    lcd.setCursor(6, 1);
    lcd.print(ValorCalibracao);
  }
}
*/
void funcaoAlterarHora() {
  LeituraHora();        //Pego os dados da hora neste instante
  
  byte MinutoConfigurado = minuto;
  byte HoraConfigurado = hora;
  byte DiaConfigurado = dia; 
  byte MesConfigurado = mes;
  byte AnoConfigurado = ano;
  byte DiaSemanaConfigurado = diasemana;

  byte ValorASerAtualizado = 0;

  byte OpcaoSelecionada = 0;
  bool ManterMenu = true;
  
  //Antes de tudo, limpo o conteúdo do display
  lcd.clear();

  //Mantenho o menu perguntando ao usuário o que ele deseja alterar
  while(ManterMenu) {
    //Leitura dos botoes e limitacao de range
    if(!digitalRead(up) == HIGH) {
      ManterMenu = false;
      delay(AtrasoBotoes);
    }
    if(!digitalRead(mais) == HIGH) {
      OpcaoSelecionada++;
      delay(AtrasoBotoes); 
    }
    if(!digitalRead(menos) == HIGH) {
      OpcaoSelecionada--;
      delay(AtrasoBotoes); 
    }
    if (OpcaoSelecionada < 0) {
      OpcaoSelecionada = 0;
    }
    
    //Menu 1 - Minuto e hora
    if((OpcaoSelecionada == 0) || (OpcaoSelecionada == 1)) {
      if(OpcaoSelecionada == 0) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.write((byte)1);                    // imprimindo o carcter do seletor para opção "Minutos"
        lcd.setCursor(2, 1);
        lcd.write('>');
      }
      else {
        lcd.clear();
        lcd.setCursor(9, 0);
        lcd.write((byte)1);                    // imprimindo o carcter do seletor para opção "Horas"
        lcd.setCursor(10, 1);
        lcd.write('>');
      }

     lcd.setCursor(3, 1);
     if(MinutoConfigurado < 10){ 
      lcd.write('0');
      lcd.setCursor(4, 1);
      lcd.print(MinutoConfigurado);
      }
     else if(MinutoConfigurado >= 10) {
        lcd.print(MinutoConfigurado);
     }
     lcd.setCursor(11, 1);
     if(HoraConfigurado < 10){ 
       lcd.write('0');
       lcd.print(HoraConfigurado);
      }
     else if(HoraConfigurado >= 10) {
         lcd.print(HoraConfigurado);
      }
     lcd.setCursor(1, 0);
     lcd.print("Minutos");
     lcd.setCursor(10, 0);
     lcd.print("Horas");
    }
    
    //Menu 2 - Data e mês
    if((OpcaoSelecionada == 2) || (OpcaoSelecionada == 3) || (OpcaoSelecionada == 4)) {
      if(OpcaoSelecionada == 2) { 
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.write((byte)1);                    // imprimindo o carcter especial 1
        lcd.setCursor(1, 1);
        lcd.write('>');
      }
      
      else if(OpcaoSelecionada == 3) {
        lcd.clear();
        lcd.setCursor(6, 0);
        lcd.write((byte)1);
        lcd.setCursor(7, 1);
        lcd.write('>');
      }

     else {
        lcd.clear();
        lcd.setCursor(12, 0);
        lcd.write((byte)1);
        lcd.setCursor(13, 1);
        lcd.write('>');
        }

     
      lcd.setCursor(2, 1);
      if(DiaConfigurado < 10) {
        lcd.write('0');
        lcd.print(DiaConfigurado);
      }
      else if (DiaConfigurado >= 10) {
        lcd.print(DiaConfigurado);
      }

      lcd.setCursor(8, 1);
      if(MesConfigurado < 10) {
        lcd.write('0');
        lcd.print(MesConfigurado);
      }
      else if(MesConfigurado >= 10) {
        lcd.print(MesConfigurado);
      }

      lcd.setCursor(14, 1);
      if(AnoConfigurado < 10) {
        lcd.write('0');
        lcd.print(AnoConfigurado);
      }
      else if(AnoConfigurado >= 10) {
        lcd.print(AnoConfigurado);
      }

     lcd.setCursor(1, 0);
     lcd.print("Data");
     lcd.setCursor(7, 0);
     lcd.print("Mes");
     lcd.setCursor(13, 0);
     lcd.print("Ano");
    }      


    
    //Por último, o dia da semana
    if(OpcaoSelecionada == 5) {
       lcd.clear();
       lcd.setCursor(6, 0);
       lcd.write((byte)1);
       lcd.print("Dia");
       lcd.setCursor(3, 1);
       lcd.write('>');
       
       switch(DiaSemanaConfigurado) {
          case 0:
            lcd.print("DOMINGO");
          case 1:
            lcd.print("SEGUNDA");
          case 2:
            lcd.print("TERCA");
          case 3:
            lcd.print("QUARTA");
          case 4:
            lcd.print("QUINTA");
          case 5:
            lcd.print("SEXTA");
          case 6:
            lcd.print("SABADO");
        }
    }
    
    //Verifico se o botao de selecao foi pressionado
    if(!digitalRead(select) == HIGH) {
      delay(AtrasoBotoes); 
      //Verificando a opcao selecionada do usuário
      switch(OpcaoSelecionada) {
        case 0:   //Minuto 
          MinutoConfigurado = atualizaValor(OpcaoSelecionada, MinutoConfigurado);
          break;
        case 1:   //Hora
          HoraConfigurado = atualizaValor(OpcaoSelecionada, HoraConfigurado);
          break;
        case 2:
          DiaConfigurado = atualizaValor(OpcaoSelecionada, DiaConfigurado);
          break; 
        case 3:
          MesConfigurado = atualizaValor(OpcaoSelecionada, MesConfigurado);
          break;
        case 4:
          AnoConfigurado = atualizaValor(OpcaoSelecionada, AnoConfigurado);
          break;
        case 5:      
          DiaSemanaConfigurado = atualizaValor(OpcaoSelecionada, DiaSemanaConfigurado);
          break;     
      }
    }
    if(OpcaoSelecionada > 5) {
      SalvarData(MinutoConfigurado, HoraConfigurado, DiaConfigurado, MesConfigurado, AnoConfigurado, DiaSemanaConfigurado);
      return;
    }
    delay(AtualizacaoDisplay);
  } //Fim do loop menu
  
  lcd.clear();
}


byte atualizaValor(byte OpcaoSelecionada, byte ValorASerAtualizado) {
    byte Limite = 0;
    byte ValorAtualizado = 0;
    bool ManterMenu = true;

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.write((byte)1);
    lcd.setCursor(9, 0);
    lcd.print("RELOGIO");

    lcd.setCursor(1, 0);
    //Verifico qual opção o usuário está alterando
    switch(OpcaoSelecionada) {
      case 0:
        Limite = 59;          //Minuto
        lcd.print("Minuto");
        break;
      case 1:
        Limite = 24;          //Hora
        lcd.print("Hora");
        break;
      case 2:
        Limite = 31;          //Data
        lcd.print("Data");
        break;
      case 3:
        Limite = 12;          //Mês
        lcd.print("Mes"); 
        break;
      case 4:
        Limite = 99;          //Ano
        lcd.print("Ano");
        break;
      case 5:
        Limite = 6;           //Dia
        lcd.print("Dia");
        break;
    }

    while(ManterMenu) {
      //Leitura dos botoes e limitacao de range
      if((!digitalRead(up) == HIGH) || (!digitalRead(select) == HIGH)) {
        ManterMenu = false;
        delay(AtrasoBotoes); 
      }
      if(!digitalRead(mais) == HIGH) {
        ValorAtualizado++; 
        delay(AtrasoBotoes); 
      }
      if(!digitalRead(menos) == HIGH) {
        ValorAtualizado--;
        delay(AtrasoBotoes); 
      }
      if(ValorAtualizado > Limite) {
        ValorAtualizado = Limite;
      }
      else if (ValorAtualizado < 0) {
        ValorAtualizado = 0;
      }

      if(OpcaoSelecionada <= 4) {
        lcd.setCursor(1, 1);
        lcd.write('>');
        if(ValorAtualizado < 10) {
          lcd.write('0');
        }
        lcd.print(ValorAtualizado);
        delay(AtualizacaoDisplay);
      }
      else {
        lcd.setCursor(1, 1);
        lcd.write('>');
        switch(ValorAtualizado) {
          case 0:
            lcd.print("DOMINGO");
          case 1:
            lcd.print("SEGUNDA");
          case 2:
            lcd.print("TERCA");
          case 3:
            lcd.print("QUARTA");
          case 4:
            lcd.print("QUINTA");
          case 5:
            lcd.print("SEXTA");
          case 6:
            lcd.print("SABADO");
        }
      }
   }
  return ValorAtualizado;
}

bool SalvarData(byte minuto, byte hora, byte dia, byte mes, byte ano, byte diasemana) {
  bool DesejaSalvar = true;
  
  lcd.clear();
  lcd.setCursor(2, 0);
  lcd.print("Salvar Data?");

  //Laço necessário para manter o menu
  while(true) {
    if((!digitalRead(mais) == HIGH) || (!digitalRead(menos) == HIGH)){
      DesejaSalvar = !DesejaSalvar;
      delay(AtrasoBotoes);
    }
    if(!digitalRead(select) == HIGH) {
      break;
    }
    
    if(DesejaSalvar) {
        lcd.setCursor(2, 1);
        lcd.print(">SIM");
        lcd.setCursor(9, 1);
        lcd.print(" NAO");
      }
     else {
        lcd.setCursor(2, 1);
        lcd.print(" SIM");
        lcd.setCursor(9, 1);
        lcd.print(">NAO");
      }
  }

  if(DesejaSalvar) {
    //Função para salvar vem aki
    AtualizaRTC(minuto, hora, dia, mes, ano, diasemana);
    salvoComSucesso();
  }
  return;
}


//=================================== Inicio do Programa ==========================================
void setup() {
  Wire.begin();

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

  if(EEPROM.read(EnderecoTempoSalvar) == 0) {
    TempoParaSalvar = 1;
    EEPROM.update(EnderecoTempoSalvar, 1); 
  }
  TempoParaSalvar = EEPROM.read(EnderecoTempoSalvar);
  
  /*                         
  while(!SD.begin(SelecaoChip)) {
     erroAbrirCartao();
  }
 
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("Cartao SD ");
  lcd.setCursor(3, 1);
  lcd.print("encontrado.");  
  delay(2000);
 */
}

void loop() {
  menuPrincipal();
  delay(1);
}
