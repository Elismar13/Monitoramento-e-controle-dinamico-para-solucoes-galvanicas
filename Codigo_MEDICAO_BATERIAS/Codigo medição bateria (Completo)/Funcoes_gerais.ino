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
  tensao = tensao*TensaoADC/1023;
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
   *  0.000V, 0.000V, 0.000V, 0.000V - 00/00/0000 00:00:00
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
  LinhaDados += "V - ";

  //Dia
  if(dia < 10) {
    LinhaDados += '0';     //Se o dia for menor que 10, adiciono um zero à frente.
  }
  LinhaDados += String(dia);
  LinhaDados += "/";

  //Mês
  if(mes < 10) {
    LinhaDados += '0';     //Se o mes for menor que 10, adiciono um zero à frente.
  }
  LinhaDados += String(mes);
  LinhaDados += "/";

  //Ano
  LinhaDados += "20";
  if(ano < 10) {
    LinhaDados += '0';     //Se o ano for menor que 10, adiciono um zero à frente.
  }
  LinhaDados += String(ano);
  LinhaDados += " ";

  //Hora
  if(hora < 10) {
    LinhaDados += '0';    //Se a hora for menor que 10, adiciono um zero à frente
  }
  LinhaDados += String(hora);
  LinhaDados += ":";

  //Minuto
  if(minuto < 10) {
    LinhaDados += '0';  //Se o minuto for menor que 10, adiciono um zero à frente
  }
  LinhaDados += String(minuto);
  LinhaDados += ":";

  //Segundo
  if(segundo < 10) {
    LinhaDados += '0'; //Se o segundo for menor que 10, adiciono um zero à frente
  }
  LinhaDados += String(segundo);

  /*     DEBUG  
  Serial.println(LinhaDados);
  */
  
      //Por fim, escrevo os dados.
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

  //Salvando o arquivo
  File dataFile = SD.open("dados.txt", FILE_WRITE);

  if(dataFile) {             //Se tudo estiver configurado com o cartão
    dataFile.println(LinhaDados); //Escrevo a linha com os dados
    dataFile.close();        //Fecho o arquivo (para evitar erros)
    salvoComSucesso();
  }
  else {
    erroGeral();
  }
  delay(1200);
}

//Conversões necessárias para leitura e escrita no relógio RTC
byte decToBcd(byte valor) {
  return ( (valor/10*16) + (valor%10) );
}
byte bcdToDec(byte val)  {
  return ( (val/16*10) + (val%16) );
}


//Leitura da hora e atualização das variáveis
void LeituraHora(boolean Atualizar) {  
  Wire.beginTransmission(DS1307_ADDRESS);       //Inicio a comunição I2C
  Wire.write(0x00);                             //Inicio
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
  //Meu 'timer' para salvar dados

  if(Atualizar) {
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
}



//Atualização dos dados de horário no RTC
void AtualizaRTC(byte minuto, byte hora, byte dia, byte mes, byte ano, byte diasemana) {
   byte segundo   =         00;   //0~59

    //Condição para escrever os dias
    if(ano%4 != 0) {
      if((mes == 2) && (dia > 28)) {
        dia = 28; 
      }
    }
    else {
      if((mes == 2) && (dia > 29)) {
        dia = 29;
      }
    }
  
    if((mes == 4) || (mes == 6) || (mes == 9) || (mes == 1)) {
      if(dia > 30) {
        dia = 30;
      }
    }
    
   //Parte da atualização do relógio
   Wire.beginTransmission(DS1307_ADDRESS);
   Wire.write(0x00);

   Wire.write(decToBcd(segundo));
   Wire.write(decToBcd(minuto));
   Wire.write(decToBcd(hora));
   Wire.write(decToBcd(diasemana));
   Wire.write(decToBcd(dia));
   Wire.write(decToBcd(mes));
   Wire.write(decToBcd(ano));
  
   Wire.write(0x00); 
  
   Wire.endTransmission();
}
