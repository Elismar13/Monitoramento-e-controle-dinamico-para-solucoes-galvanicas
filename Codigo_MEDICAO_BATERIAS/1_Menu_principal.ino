//************************* FUNÇÕES DO MENU PRINCIPAL *********************************
//MenuPrincipal do programa
void menuPrincipal() {

    //Necessario para ficar neste menu
  while(true) {               
    //Antes de tudo, limpo todo conteúdo do display e verifico a hora e o timer
    LeituraHora(false);                 
    lcd.clear();

     //Leitura de botões e atualização de variáveis
    if(!digitalRead(mais) == HIGH) {
      SeletorPrincipal += 1;
      delay(AtrasoBotoes);
    } 
    else if(!digitalRead(menos) == HIGH) {
      SeletorPrincipal -= 1; 
      delay(AtrasoBotoes);
    }
  
    //Escrever a mensagem do menu principal e opções do menu
    //lcd.print("Menu Principal");
    lcd.setCursor(1, 0);
    lcd.write('M');
    lcd.write('e');
    lcd.write('n');
    lcd.write('u');
    lcd.write(' ');
    lcd.write('P');
    lcd.write('r');
    lcd.write('i');
    lcd.write('n');
    lcd.write('c');
    lcd.write('i');
    lcd.write('p');
    lcd.write('a');
    lcd.write('l');

    //Escrevo a mensagem referente ao menu selecionado                
      //Opções disponíveis para o usuário
      lcd.setCursor(10, 1);
      lcd.print("Config");   
      lcd.setCursor(1, 1);
      lcd.print("Niveis");

    
    //Limites para o menu principal
    if(SeletorPrincipal > 1){
      SeletorPrincipal = 0;
    }
    else if(SeletorPrincipal < 0) {
      SeletorPrincipal = 1;
    }
  

    //Alterando a posição do cursor (">") para informar ao usuário qual menu ele deseja escolher
    if(SeletorPrincipal == 0) {
      lcd.setCursor(9, 1);
      lcd.write(' ');   
      lcd.setCursor(0, 1);
      lcd.write('>');
    }
    else {
      lcd.setCursor(0, 1);
      lcd.write(' ');
      lcd.setCursor(9, 1);
      lcd.write('>');   
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
