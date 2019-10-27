//Função de alterar a hora
void funcaoAlterarHora() {
  LeituraHora(false);        //Pego os dados da hora neste instante

  //Seto as variáveis atual de tempo para mostrar o tempo atual
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

    //minuto configurado
     lcd.setCursor(3, 1);
     if(MinutoConfigurado < 10){ 
      lcd.write('0');
     }
     lcd.print(MinutoConfigurado);

    //Hora configurada
     lcd.setCursor(11, 1);
     if(HoraConfigurado < 10){ 
       lcd.write('0');
     }
     lcd.print(HoraConfigurado);

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

     //Dia configurado
      lcd.setCursor(2, 1);
      if(DiaConfigurado < 10) {
        lcd.write('0');
      }
      lcd.print(DiaConfigurado);

    //Mes configurado
      lcd.setCursor(8, 1);
      if(MesConfigurado < 10) {
        lcd.write('0');
      }
      lcd.print(MesConfigurado);

    //Ano configurado
      lcd.setCursor(14, 1);
      if(AnoConfigurado < 10) {
        lcd.write('0');
      }
      lcd.print(AnoConfigurado);


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
       lcd.setCursor(5, 0);
       lcd.write((byte)1);
       lcd.print("Dia");
       lcd.setCursor(4, 1);
       lcd.write('>');

       
        if(DiaSemanaConfigurado == 0) {
            lcd.print("DOMINGO");
        }
        else if(DiaSemanaConfigurado == 1) {
            lcd.print("SEGUNDA");
        }
        else if(DiaSemanaConfigurado == 2) {
            lcd.print("TERCA");
        }
        else if(DiaSemanaConfigurado == 3) {
            lcd.print("QUARTA");
        }
        else if(DiaSemanaConfigurado == 4) {
            lcd.print("QUINTA");
        }
        else if(DiaSemanaConfigurado == 5) {
            lcd.print("SEXTA");
        }
        else if(DiaSemanaConfigurado == 6) {
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
    //Limites para atualização de valores
    byte LimiteSuperior = 0;
    byte LimiteInferior = 0;
    
    byte ValorAtualizado = 0;
    bool ManterMenu = true;
    while(ManterMenu) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.write((byte)1);
      lcd.setCursor(9, 0);
      lcd.print("RELOGIO");
  
      lcd.setCursor(1, 0);
      //Verifico qual opção o usuário está alterando
      switch(OpcaoSelecionada) {
        case 0:
          LimiteSuperior = 59;          //Minuto
          LimiteInferior = 0;
          lcd.print("Minuto");
          break;
        case 1:
          LimiteSuperior = 23;          //Hora
          LimiteInferior = 0;
          lcd.print("Hora");
          break;
        case 2:
          LimiteSuperior = 31;          //Data
          LimiteInferior = 1;
          lcd.print("Data");
          break;
        case 3:
          LimiteSuperior = 12;          //Mês
          LimiteInferior = 1;
          lcd.print("Mes"); 
          break;
        case 4:
          LimiteSuperior = 99;          //Ano
          LimiteInferior = 0;
          lcd.print("Ano");
          break;
        case 5:
          LimiteSuperior = 6;           //Dia
          LimiteInferior = 0;
          lcd.print("Dia");
          break;
        }
  
        //Leitura dos botoes e limitacao de range
        if((!digitalRead(up) == HIGH) || (!digitalRead(select) == HIGH)) {
          ManterMenu = false;
          delay(AtrasoBotoes); 
        }
        if(!digitalRead(mais) == HIGH) {
          ValorAtualizado++;
          delay(AtrasoBotoes); 
          while(!digitalRead(mais) == HIGH) {
            ValorAtualizado += 5;
            delay(AtrasoBotoes); 
          }
        }
        if(!digitalRead(menos) == HIGH) {
          ValorAtualizado--;
          delay(AtrasoBotoes);
          while(!digitalRead(menos) == HIGH) {
            ValorAtualizado -= 5;
            delay(AtrasoBotoes); 
          } 
        }
        if(ValorAtualizado > LimiteSuperior) {
          ValorAtualizado = LimiteInferior;
        }
        else if (ValorAtualizado < LimiteInferior) {
          ValorAtualizado = LimiteSuperior;
        }

      if(OpcaoSelecionada <= 4) {
        lcd.setCursor(1, 1);
        lcd.write('>');
        if(ValorAtualizado < 10) {
          lcd.write('0');
        }
        lcd.print(ValorAtualizado);
      }
      else {
        lcd.setCursor(1, 1);
        lcd.write('>');
        
        //Mostrar dias
        if(ValorAtualizado == 0) {
            lcd.print("DOMINGO");
        }
        else if(ValorAtualizado == 1) {
            lcd.print("SEGUNDA");
        }
        else if(ValorAtualizado == 2) {
            lcd.print("TERCA");
        }
        else if(ValorAtualizado == 3) {
            lcd.print("QUARTA");
        }
        else if(ValorAtualizado == 4) {
            lcd.print("QUINTA");
        }
        else if(ValorAtualizado == 5) {
            lcd.print("SEXTA");
        }
        else if(ValorAtualizado == 6) {
            lcd.print("SABADO");
        }
      }
     delay(75);   //Atualizar o display
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
    lcd.setCursor(3, 1);
    lcd.print("SIM");
    lcd.setCursor(10, 1);
    lcd.write('N');
    lcd.write((byte)4);
    lcd.write('O');
    //lcd.print("NAO");
   
    if(DesejaSalvar) {
        lcd.setCursor(2, 1);
        lcd.write('>');
        lcd.setCursor(9, 1);
        lcd.write(' ');
      }
     else {
        lcd.setCursor(2, 1);
        lcd.write(' ');
        lcd.setCursor(9, 1);
        lcd.write('>');
      }
  }

  if(DesejaSalvar) {
    //Função para salvar vem aki
    AtualizaRTC(minuto, hora, dia, mes, ano, diasemana);
    salvoComSucesso();
  }
  return;
}
