//************************** FUNÇÕES PARA O MENU DE CONFIGURAÇÕES **********************************
void configuracoes() {
    while(!digitalRead(up) == LOW) {          //Loop necessário para o programa ficar nesse menu
      lcd.clear();
      //Printar as palavras
      lcd.setCursor(2, 0);
      lcd.print("Configuracoes");

      if(SeletorConfiguracoes < 2) {
        lcd.setCursor(1, 1);
        lcd.print("Arquivo");
        lcd.setCursor(12, 1);
        lcd.print("Hora");
      }

      if(!digitalRead(mais) == HIGH) {
        SeletorConfiguracoes += 1;
        delay(AtrasoBotoes);
      } 
      else if(!digitalRead(menos) == HIGH) {
        SeletorConfiguracoes -= 1; 
        delay(AtrasoBotoes);
      }
    
      //Limites para o menu de configuracoes
      if(SeletorConfiguracoes > 2){
        SeletorConfiguracoes = 0;
      }
      else if(SeletorConfiguracoes < 0) {
        SeletorConfiguracoes = 0;
      }
    
      if(SeletorConfiguracoes == 0) {
          lcd.setCursor(0, 1);
          lcd.print(">");
          lcd.setCursor(11, 1);
          lcd.write(' ');
      }
      else if(SeletorConfiguracoes == 1) {
          lcd.setCursor(0, 1);
          lcd.print(' ');
          lcd.setCursor(11, 1);
          lcd.write('>');
      }
      else {
          lcd.setCursor(0, 1);
          lcd.write('>');
          lcd.print("Calibrar ADC");
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
            calibracao();
            break;
        }
      }
     delay(AtualizacaoDisplay);     //Atraso para Atualizar o display
    }
}

void configurarArquivo() {
  int Horas = 0;
  
  lcd.clear();
  lcd.setCursor(0, 0);
  //lcd.print("Atualiza arquivo");
  lcd.write('A');
  lcd.write('t');
  lcd.write('u');
  lcd.write('a');
  lcd.write('l');
  lcd.write('i');
  lcd.write('z');
  lcd.write('a');
  lcd.write(' ');
  lcd.write('a');
  lcd.write('r');
  lcd.write('q');
  lcd.write('u');
  lcd.write('i');
  lcd.write('v');
  lcd.write('o');

  //lcd.print("a cada: ");
  lcd.setCursor(0, 1);
  lcd.write('a');
  lcd.write(' ');
  lcd.write('c');
  lcd.write('a');
  lcd.write('d');
  lcd.write('a');
  lcd.write(':');
  lcd.write(' ');


  while(true) {
    delay(AtualizacaoDisplay);     //Atraso para Atualizar o display

    if(!digitalRead(select) == HIGH) {
      delay(AtrasoBotoes);
      break;
    }
    if(!digitalRead(menos) == HIGH) {
      delay(AtrasoBotoes*2);
      TempoParaSalvar--;
      /*
      while(!digitalRead(menos) == HIGH) {
        TempoParaSalvar-= 5;
        delay(AtrasoBotoes); 
      }
      */
    }
    if(!digitalRead(mais) == HIGH) {
      delay(AtrasoBotoes*2);
      TempoParaSalvar++;
      /*
      while(!digitalRead(mais) == HIGH) {
        TempoParaSalvar += 5;
        delay(AtrasoBotoes); 
      }
      */
    }
    if(TempoParaSalvar < 1) {
      TempoParaSalvar = 240;
    }
    else if(TempoParaSalvar > 240) {
      TempoParaSalvar = 1;
    }

    lcd.setCursor(9, 1);
    Horas = (int)TempoParaSalvar/60;
    lcd.write('0');

    if(Horas == 0) {
      lcd.write('0');
    }
    else if(Horas == 1){
      lcd.write('1');
    }
    else if(Horas == 2) {
      lcd.write('2');
    }
    else if(Horas == 3) {
      lcd.write('3');
    }
    else if(Horas == 4){
      lcd.write('4');
    }
    lcd.write('h');
    
    if(TempoParaSalvar < 60) {
      if(TempoParaSalvar-(60*Horas) < 10) {
        lcd.write('0');
      }
      lcd.print(TempoParaSalvar);
    }
    else if(TempoParaSalvar >= 60 && TempoParaSalvar < 120) {
      if(TempoParaSalvar-(60*Horas) < 10) {
        lcd.write('0');
      }
      lcd.print(TempoParaSalvar-60);
    }
    else if(TempoParaSalvar >= 120 && TempoParaSalvar < 180) {
      if(TempoParaSalvar-(60*Horas) < 10) {
        lcd.write('0');
      }
      lcd.print(TempoParaSalvar-120);
    }
    else if(TempoParaSalvar >= 180 && TempoParaSalvar < 240) {
      if(TempoParaSalvar-(60*Horas) < 10) {
        lcd.write('0');
      }    
      lcd.print(TempoParaSalvar-180);
    }
    else if (TempoParaSalvar >= 240) {
      if(TempoParaSalvar-(60*Horas) < 10) {
        lcd.write('0');
      }
      lcd.print(TempoParaSalvar - 240);
    }
    lcd.write('m');
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
    LeituraHora(false);
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
    lcd.write('N');
    lcd.write((byte)4);
    lcd.write('O');
    
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


//Função para calibrar o ADC
void calibracao() {
  CalibracaoADC = EEPROM.read(EnderecoCalibracao);
  
  while (true) {
    lcd.clear();
  
    //"Constante de calibracao: XX.XX"
    lcd.setCursor(2, 0);
    lcd.write('C');
    lcd.write('o');
    lcd.write('n');
    lcd.write('s');
    lcd.write('t');
    lcd.write('a');
    lcd.write('n');
    lcd.write('t');
    lcd.write('e');
    lcd.write(' ');
    lcd.write('d');
    lcd.write('e');
  
    lcd.setCursor(0, 1);
    lcd.write('c');
    lcd.write('a');
    lcd.write('l');
    lcd.write('i');
    lcd.write('b');
    lcd.write('r');
    lcd.write('a');
    lcd.write((byte)2);
    lcd.write((byte)4);
    lcd.write('o');
    lcd.write(':');
    lcd.write(' ');
      
    lcd.print(CalibracaoADC-20);
  
    //Leitura botoes 
    if(!digitalRead(mais) == HIGH) {
      CalibracaoADC += 1;
      delay(AtrasoBotoes*2);
    }
    if(!digitalRead(menos) == HIGH) {
      CalibracaoADC -= 1;
      delay(AtrasoBotoes*2);
    }
    if(!digitalRead(select) == HIGH) {
      delay(AtrasoBotoes);
      break;
    }
    if(!digitalRead(up) == HIGH) {
      delay(AtrasoBotoes*4);
      return;
    }
  
    //Limites 
    if(CalibracaoADC < 0) {
      CalibracaoADC = 40;
    }
    else if(CalibracaoADC > 40) {
      CalibracaoADC = 0;
      }

    delay(AtualizacaoDisplay);
  }


  //Se o usuário decidiu ficar neste menu, não seja por isso, atualizo a o valor da calibragem.
  if(CalibracaoADC != 20) {
    for(byte i = 0; i<(abs(CalibracaoADC-20)); i++) {
      if(CalibracaoADC < 20) {
        TensaoADC += 0.001;
      }
      else {
        TensaoADC -= 0.001;
      }
    }
  }
  EEPROM.update(EnderecoCalibracao, CalibracaoADC);
  salvoComSucesso();
}
