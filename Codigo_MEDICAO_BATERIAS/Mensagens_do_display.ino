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
  lcd.setCursor(0, 1);
  lcd.write('N');
  lcd.write((byte)4);
  lcd.print("o encontrado.");
  //Mantenho a mensagem por 1 segundo
  delay(1500);
  
  lcd.clear();
  return;   
}
