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
     LeituraHora(true);          //Verifico o timer e consequentemente a hora
     NiveisBaterias();       //Leio os níveis das 4 baterias
     
     lcd.clear();           //Limpar o que tem na tela
     lcd.setCursor(3, 0);   //Posiciono o cursor para printar o número da SOLUÇÃO selecionada
     lcd.print("Solu");
     lcd.write((byte)2);
     lcd.write((byte)4);
     lcd.write('o');
     lcd.write(' ');
     
     
  //Mostrar o status das bateria selecionada

     //BATERIA X:
     if(ValorBat == 1) {
        lcd.write('1');
        NivelBateria = Bateria1;
     }
     else if(ValorBat == 2) {
        lcd.write('2');
        NivelBateria = Bateria2;
     }
     else if(ValorBat == 3) 
     {        
        lcd.write('3');
        NivelBateria = Bateria3;
     }
     else if(ValorBat == 4) {
        lcd.write('4');
        NivelBateria = Bateria4;
      }
     lcd.write(':');


   //Mostrar o valor da bateria em VOLTS 
    lcd.setCursor(5, 1);
    if(NivelBateria >= 1.00) {
        lcd.print(NivelBateria, 3);
        lcd.setCursor(11, 1);
        lcd.write('V');
      }

    //Mostro o valor da bateria em miliVolts
    else {
      lcd.setCursor(5, 1);
      TensaoEmString = String(NivelBateria, 4);
      for(byte i = 2; i < 5; i++) {
        lcd.write(TensaoEmString[i]);
      }
      lcd.write(' ');
      lcd.write('m');
      lcd.write('V');
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
