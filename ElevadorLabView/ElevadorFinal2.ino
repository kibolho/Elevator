//#include "Threads.h"
//#include "ThreadsController.h"
char command;
String string;
boolean recebi = false;
boolean erro1 = false;
//ELEVADOR1
int motor1a=5;
int motor1b=3;
long timer, lastTimer =0;
long interval =200;
// Direção = 1 Descendo
// Direção = 2 Subindo
int direcao = 0;
//FINS DE CURSO
byte  UMSensor1Andar = 36;
byte  UMSensor2Andar = 37;
byte  UMSensor3Andar = 38;
byte  UMSensor4Andar = 39;

//BOTOES EXTERNOS
byte  primeiroAndar = 22;
byte  segundoAndarD = 23;
byte  segundoAndarS = 24;
byte  terceiroAndarD = 25;
byte  terceiroAndarS = 26;
byte  quartoAndar = 27;

//BOTOES INTERNOS
byte  UMInt1Andar = 29;
byte  UMInt2Andar = 30;
byte  UMInt3Andar = 31;
byte  UMInt4Andar = 32;
byte  Emergencia = 28;

//COLOCANDO OS BOTÕES EM UM ARRAY PARA SIMPLIFICAR O SETUP
int buttonsOutput[]={motor1a,motor1b};

#define NUMOUTPUTS sizeof(buttonsOutput)

//Variavel que guarda os andares a serem atendidos 
//andaresParaAtender={0,0,0,0,0,1} = Atender 4 andar
//andaresParaAtender={0,0,0,0,1,0} = Atender 3 andar cima
//andaresParaAtender={0,0,0,1,0,0} = Atender 3 andar baixo
//andaresParaAtender={0,0,1,0,0,0} = Atender 2 andar cima
//andaresParaAtender={0,1,0,0,0,0} = Atender 2 andar baixo
//andaresParaAtender={1,0,0,0,0,0} = Atender 1 andar
int andaresParaAtender[6]={0,0,0,0,0,0};

//Variavel que guarda os andares para entregar
//andaresParaEntregar={0,0,0,1} = Entregar 4 andar
//andaresParaEntregar={0,0,1,0} = Entregar 3 andar
//andaresParaEntregar={0,1,0,0} = Entregar 2 andar
//andaresParaEntregar={1,0,0,0} = Entregar 1 andar
int andaresParaEntregar1[4]={0,0,0,0};

//Guarda o andar atual do elevador
int andarAtualElevador1 =1;

//Situação do elevador
int parado1=1;
int andarIndoAtender = 0;
/*
int elevador1ocupado = 0;
int elevador2ocupado = 0;
*/

//ESTADO DOS FINS DE CURSO
int andar1[4]={0,0,0,0};

int x = 0;
int y = 0;

//Tempo de porta aberta do elevador
#define OpenTime 2000
long timeElevator1 = 0;
long timeElevator2 = 0;

//DEBOUNCE DE TODOS OS PINOS DE INPUT
#define DEBOUNCE 10  // button debouncer, how many ms to debounce, 5+ ms is usually plenty

byte buttons[] = {UMSensor1Andar, UMSensor2Andar, UMSensor3Andar, UMSensor4Andar,
                  primeiroAndar, segundoAndarD, segundoAndarS, terceiroAndarD, terceiroAndarS, quartoAndar,
                  UMInt1Andar, UMInt2Andar, UMInt3Andar, UMInt4Andar, Emergencia};
//determine how big the array up above is, by checking the size
#define NUMBUTTONS sizeof(buttons)
 
//track if a button is just pressed, just released, or 'currently pressed' 
byte pressed[NUMBUTTONS], justpressed[NUMBUTTONS], justreleased[NUMBUTTONS];
byte previous_keystate[NUMBUTTONS], current_keystate[NUMBUTTONS];
 
void setup(){
  byte i;
  Serial.begin(9600); //set up serial port
  // Make input &amp; enable pull-up resistors on switch pins
  for (i=0; i< NUMBUTTONS; i++) {
    pinMode(buttons[i], INPUT);
    digitalWrite(buttons[i], HIGH);
  }
  for (i=0; i< NUMOUTPUTS; i++) {
    pinMode(buttonsOutput[i], OUTPUT);
  }
  while(digitalRead(UMSensor1Andar)){
    desce1();
    andarIndoAtender = 1;
  }
  displayElevador1(1);
  para1(1);
  /*
  if(!digitalRead(UMSensor1Andar)){
    displayElevador1(1);
    para1(1);
  }
  else if(!digitalRead(UMSensor2Andar)){
    displayElevador1(2);
    para1(21);
    para1(22);
  }
  else if(!digitalRead(UMSensor3Andar)){
    displayElevador1(3);
    para1(31);
    para1(32);
  }
  else if(!digitalRead(UMSensor4Andar)){
    displayElevador1(4);
    para1(4);
  }
  */
}
 
void loop() {
if(Serial.available()>0){
    String recebido = (String)Serial.read();
    string = recebido;
    if (string == "82" ){
      recebi = true;
      Serial.flush();
    }
    else if(string == "49"){
      andaresParaEntregar1[0] =1;
      Serial.flush();
    }
    else if(string == "50"){
      andaresParaEntregar1[1] =1;
      Serial.flush();
    }
    else if(string == "51"){
      andaresParaEntregar1[2] =1;
      Serial.flush();
    }
    else if(string == "52"){
      andaresParaEntregar1[3] =1;
      Serial.flush();
    }
    else if(string == "53"){
      for(int i =0; i<4; i++){
      andaresParaEntregar1[i] =0;
      }
      Serial.flush();
    }
    string = 0;      
  }
  timer = millis();
  if((timer - lastTimer)>interval){
    if (recebi){
      responder();
      recebi = false;
      lastTimer = millis();
    }
  }
  byte thisSwitch=thisSwitch_justPressed();
  switch(thisSwitch){  
    //ELEVADOR1 CHEGOU ANDAR 1
    case 0: 
      displayElevador1(1);
      para1(1);
    break;
    //ELEVADOR1 CHEGOU ANDAR 2
    case 1: 
      displayElevador1(2);
      //andarAtualElevador1=2;
      //PRECISA ENTREGAR ALGUEM NESSE ANDAR?
      if(andaresParaEntregar1[1]==1){
        para1(21);
      }
      //PRECISA PEGAR ALGUEM NESSE ANDAR e A PESSOA VAI SUBIR E NÃO TEM NINGUEM PEDINDO PRA DESCER
      else if( (andaresParaAtender[2]==1) && (direcao==1)){
        para1(22);
      }
      //PRECISA PEGAR ALGUEM NESSE ANDAR e A PESSOA VAI DESCER E NÃO TEM NINGUEM PEDINDO PRA SUBIR
      else if( (andaresParaAtender[1]==1) && (direcao==2)){
        para1(21);
      }
    break;
    //ELEVADOR1 CHEGOU ANDAR 3
    case 2: 
    displayElevador1(3);
      //andarAtualElevador1=3;
      //PRECISA ENTREGAR ALGUEM NESSE ANDAR?
      if(andaresParaEntregar1[2]==1){
        para1(31);
      }
      //PRECISA PEGAR ALGUEM NESSE ANDAR e A PESSOA VAI SUBIR E NÃO TEM NINGUEM PEDINDO PRA DESCER
      else if( (andaresParaAtender[4]==1) && (direcao==1)){
        para1(32);
      }
      //PRECISA PEGAR ALGUEM NESSE ANDAR e A PESSOA VAI DESCER E NÃO TEM NINGUEM PEDINDO PRA SUBIR
      else if( (andaresParaAtender[3]==1) && (direcao==2)){
        para1(31);
      }
    break;
    //ELEVADOR1 CHEGOU ANDAR 4
    case 3:
      displayElevador1(4); 
      para1(4);
    break;
    //CHAMOU EXTERNO ANDAR1
    case 4: 
      andaresParaAtender[0]=1;
    break; 
    //CHAMOU EXTERNO ANDAR2 DESCE
    case 5: 
      andaresParaAtender[1]=1;
    break; 
    //CHAMOU EXTERNO ANDAR2 SOBE
    case 6: 
      andaresParaAtender[2]=1;
    break; 
    //CHAMOU EXTERNO ANDAR3 DESCE
    case 7: 
      andaresParaAtender[3]=1;
    break;  
    //CHAMOU EXTERNO ANDAR3 SOBE    
    case 8: 
      andaresParaAtender[4]=1;
    break;
    //CHAMOU EXTERNO ANDAR4
    case 9: 
      andaresParaAtender[5]=1;
    break;
    //ELEV1 MANDOU INTERNO ANDAR1
    case 10: 
      andaresParaEntregar1[0]=1;
    break;       
    //ELEV1 MANDOU INTERNO ANDAR2
    case 11: 
      andaresParaEntregar1[1]=1;
    break;       
    //ELEV1 MANDOU INTERNO ANDAR3
    case 12: 
      andaresParaEntregar1[2]=1;
    break;
    //ELEV 1 MANDOU INTERNO ANDAR4
    case 13: 
      andaresParaEntregar1[3]=1;
    break;
    case 14:
      for(int x=0;x<4;x++){
        andaresParaEntregar1[x]=0;
      }
      for(x=0;x<6;x++){
        andaresParaAtender[x]=0;
      }
    break;
    default:
    break;
  }
  /*
  if(andaresParaEntregar1[0]||andaresParaEntregar1[1]||andaresParaEntregar1[2])
    elevador1ocupado=1;
  }else{
    elevador1ocupado=0;
  }
  if(andaresParaEntregar2[0]||andaresParaEntregar2[1]||andaresParaEntregar2[2])
    elevador2ocupado=1;
  }else{
    elevador2ocupado=0;
  }
  */
  //ELEVADOR1
  if(millis()-timeElevator1>OpenTime && parado1 == 1){
    //Esta no primeiro
    if(andarAtualElevador1==1){
      //Precisa abrir a porta
      if(andaresParaEntregar1[0]==1||andaresParaAtender[0]==1){
        if(pressed[0])
          para1(1);
      }
      //precisa entregar no segundo ou terceiro ou quarto
      else if(andaresParaEntregar1[1]==1){
        andarIndoAtender = 2;
        sobe1();
      }else if(andaresParaEntregar1[2]==1){
        andarIndoAtender = 3;
        sobe1();
      }else if(andaresParaEntregar1[3]==1){
        andarIndoAtender = 4;
        sobe1();
      }
      //Alguem chamou no segundo pra descer
      else if((andaresParaAtender[1]==1)){
        andarIndoAtender = 2;
        sobe1();
      }
      //Alguem chamou no segundo pra subir
      else if((andaresParaAtender[2]==1)){
        andarIndoAtender = 2;
        sobe1();
      }
      //Alguem chamou no terceiro para descer
      else if((andaresParaAtender[3]==1)){
        andarIndoAtender = 3;
        sobe1();
      }
      //Alguem chamou no terceiro para subir
      else if((andaresParaAtender[4]==1)){
        andarIndoAtender = 3;
        sobe1();
      }
      //Alguem chamou no quarto
      else if((andaresParaAtender[5]==1)){
        andarIndoAtender = 4;
        sobe1();
      }
    }
    //Esta no segundo
    else if(andarAtualElevador1==2){
      if((andaresParaEntregar1[1]==1||andaresParaAtender[1]==1)&&(direcao==1)){
        if(pressed[1])
          para1(21);
      }
      else if(andaresParaAtender[2]==1 && (direcao==2)){
        if(pressed[1])
          para1(22);
      }
      //precisa entregar no primeiro
      else if(andaresParaEntregar1[0]==1){
        andarIndoAtender = 1;
        desce1();
      }
      //precisa entregar no terceiro ou no quarto
      else if(andaresParaEntregar1[2]==1){
        andarIndoAtender = 3;
        sobe1();
      }
      else if(andaresParaEntregar1[3]==1){
        andarIndoAtender = 4;
        sobe1();
      }
      //Alguem chamou no primeiro
      else if((andaresParaAtender[0]==1)){
        andarIndoAtender = 1;
        desce1();
      }
      //Alguem chamou no terceiro para descer
      else if((andaresParaAtender[3]==1)){
        andarIndoAtender = 3;        
        sobe1();
      }
      //Alguem chamou no terceiro para subir
      else if((andaresParaAtender[4]==1)){
        andarIndoAtender = 3;        
        sobe1();
      }
      //Alguem chamou no quarto
      else if((andaresParaAtender[5]==1)){
        andarIndoAtender = 4;
        sobe1();
      }
    }
    //Esta no terceiro
    else if(andarAtualElevador1==3){
      if((andaresParaEntregar1[2]==1||andaresParaAtender[3]==1)&&(direcao==1)){
        if(pressed[2])
          para1(31);
      }
      else if(andaresParaAtender[4]==1&&(direcao==2)){
        if(pressed[2])
          para1(32);
      }
       //precisa entregar no segundo ou primeiro
      else if((andaresParaEntregar1[0]==1)){
        andarIndoAtender = 1;
        desce1();
      }
      else if(andaresParaEntregar1[1]==1){
        andarIndoAtender = 2;
        desce1();
      }
      //precisa entregar no quarto
      else if(andaresParaEntregar1[3]==1){
        andarIndoAtender = 4;
        sobe1();
      }
      //Alguem chamou no segundo pra descer
      else if((andaresParaAtender[1]==1)){
        andarIndoAtender = 2;
        desce1();
      }
      //Alguem chamou no segundo pra subir
      else if((andaresParaAtender[2]==1)){
        andarIndoAtender = 2;
        desce1();
      }
      //Alguem chamou no primeiro
      else if((andaresParaAtender[0]==1)){
        andarIndoAtender = 1;
        desce1();
      }
      //alguem chamou no quarto
      else if ((andaresParaAtender[5]==1)){
        andarIndoAtender = 4;
        sobe1();
      }
    }
    //Esta no quarto
    if(andarAtualElevador1==4){
      //Precisa abrir a porta
      if(andaresParaEntregar1[4]==1||andaresParaAtender[5]==1){
        if(pressed[3])
          para1(4);
      }
      //precisa entregar no segundo ou terceiro ou primeiro
      else if(andaresParaEntregar1[0]==1){
        andarIndoAtender = 1;
        desce1();
      }else if(andaresParaEntregar1[1]==1){
        andarIndoAtender = 2;
        desce1();
      }else if(andaresParaEntregar1[2]==1){
        andarIndoAtender = 3;
        desce1();
      }
      //Alguem chamou no segundo pra descer
      else if((andaresParaAtender[1]==1)){
        andarIndoAtender = 2;
        desce1();
      }
      //Alguem chamou no segundo pra subir
      else if((andaresParaAtender[2]==1)){
        andarIndoAtender = 2;
        desce1();
      }
      //Alguem chamou no terceiro para descer
      else if((andaresParaAtender[3]==1)){
        andarIndoAtender = 3;
        desce1();
      }
      //Alguem chamou no terceiro para subir
      else if((andaresParaAtender[4]==1)){
        andarIndoAtender = 3;
        desce1();
      }
      //Alguem chamou no primeiro
      else if((andaresParaAtender[0]==1)){
        andarIndoAtender = 1;
        desce1();
      }
  }}

  //ELEVADOR1
  //Todos os andares estão com problema no fim de curso
  for (int i =0; i<4; i++){
    for(int j=0;j<4; j++){
      if (i!=j){
        if(pressed[i]&&pressed[j]){
          erro1 = true;
          zerarComandos(1);
        }
      }
    }
  }
  //Problema resolvivo
  for (int i =0; i<4; i++){
    for(int j=0;j<4; j++){
      if (i!=j){
        if(pressed[i]&&justReleased[j]){
          erro1 = false;
        }
      }
    }
  }
  //Nenhum fim de curso acionado
  if(!pressed[0]&&!pressed[1]&&!pressed[2]&&!pressed[3]){
    if(parado1){
      switch (andarAtualElevador1) {
          case 1:
            sobe1();
            break;
          case 2:
            sobe1();
            break;
          case 3:
            sobe1();
            break;
          case 4:
            desce1();
            break;
          default:
            break;
      }
    }
  }  
}
 
void check_switches(){
  static byte previousstate[NUMBUTTONS];
  static byte currentstate[NUMBUTTONS];
  static long lasttime;
  byte index;
  if (millis() < lasttime) {
    // we wrapped around, lets just try again
    lasttime = millis();
  }
  if ((lasttime + DEBOUNCE) > millis()) {
    // not enough time has passed to debounce
    return; 
  }
  // ok we have waited DEBOUNCE milliseconds, lets reset the timer
  lasttime = millis();
  for (index = 0; index < NUMBUTTONS; index++) {
    justpressed[index] = 0;       //when we start, we clear out the "just" indicators
    justreleased[index] = 0;
    currentstate[index] = digitalRead(buttons[index]);   //read the button
    if (currentstate[index] == previousstate[index]) {
      if ((pressed[index] == LOW) && (currentstate[index] == LOW)) {
        // just pressed
        justpressed[index] = 1;
      }
      else if ((pressed[index] == HIGH) && (currentstate[index] == HIGH)) {
        justreleased[index] = 1; // just released
      }
      pressed[index] = !currentstate[index];  //remember, digital HIGH means NOT pressed
    }
    previousstate[index] = currentstate[index]; //keep a running tally of the buttons
  }
}
 
byte thisSwitch_justPressed() {
  byte thisSwitch = 255;
  check_switches();  //check the switches &amp; get the current state
  for (byte i = 0; i < NUMBUTTONS; i++) {
    current_keystate[i]=justpressed[i];
    if (current_keystate[i] != previous_keystate[i]) {
      if (current_keystate[i]) 
        thisSwitch=i;
    }
    previous_keystate[i]=current_keystate[i];
  }  
  return thisSwitch;
}

void erro1(int andar){
  switch (andar) {
      case 1:
        displayElevador1(1);
        break;
      case 2:
        displayElevador1(2);
        break;
      case 3:
        displayElevador1(3);
        break;
      case 4:
        displayElevador1(4);
        break;
      default:
         break;
  }
}
void sobe1(){
  parado1=0;
  direcao = 2;
  //ELEVADOR1
  analogWrite(motor1a,180);
  analogWrite(motor1b, 0);
}

void desce1(){
  parado1=0;
    direcao = 1;
  //ELEVADOR1
  analogWrite(motor1a,0);
  analogWrite(motor1b,180);
}
void zerarComandos(int elevador){
  if(elevador==1){
    andaresParaEntregar1[0]=0;
    andaresParaEntregar1[1]=0;
    andaresParaEntregar1[2]=0;
    andaresParaEntregar1[3]=0;
    andaresParaAtender[0]=0;
    andaresParaAtender[1]=0;
    andaresParaAtender[2]=0;
    andaresParaAtender[3]=0;
    andaresParaAtender[4]=0;
    andaresParaAtender[5]=0;
  }
}
void para1(int andar){
  //ELEVADOR1
  parado1=1;
  timeElevator1 = millis();
  digitalWrite(motor1a,LOW);
  digitalWrite(motor1b,LOW);
  if(andar==1){
    if(andaresParaEntregar1[0]==1||andaresParaAtender[0]==1)
      displayElevador1(1); 
    andarAtualElevador1=1;
    andaresParaEntregar1[0]=0;
    andaresParaAtender[0]=0;
  }else if(andar==21){
     if(andaresParaEntregar1[1]==1||andaresParaAtender[1]==1||andaresParaAtender[2]==1)
      displayElevador1(2); 
      andarAtualElevador1=2;
      andaresParaEntregar1[1]=0;
      andaresParaAtender[1]=0;
  }else if(andar==22){
    if(andaresParaEntregar1[1]==1||andaresParaAtender[1]==1||andaresParaAtender[2]==1)
      displayElevador1(2); 
      andarAtualElevador1=2;
      andaresParaEntregar1[1]=0;
      andaresParaAtender[2]=0;
  }else if(andar==31){
    if(andaresParaEntregar1[2]==1||andaresParaAtender[2]==1)
    displayElevador1(3); 
    andarAtualElevador1=3;
    andaresParaEntregar1[2]=0;
    andaresParaAtender[3]=0;
  }
  else if(andar==32){
    if(andaresParaEntregar1[2]==1||andaresParaAtender[2]==1)
    displayElevador1(3); 
    andarAtualElevador1=3;
    andaresParaEntregar1[2]=0;
    andaresParaAtender[4]=0;
  }
  else if(andar==4){
      displayElevador1(4); 
      andarAtualElevador1=4;
      andaresParaEntregar1[3]=0;
      andaresParaAtender[5]=0;
  }
}

void displayElevador1(int andar){
  switch (andar) {
      case 1:
       andarAtualElevador1 = 1;
        break;
      case 2:
      andarAtualElevador1 = 2;       
        break;
      case 3:
      andarAtualElevador1 = 3;        
        break;
      case 4:
      andarAtualElevador1 = 4;        
        break;
  }
}
void responder(){
    int x=0;
    for (x=0;x<6;x++){
      Serial.print(andaresParaAtender[x],DEC);
      Serial.print("\t");
    }
    for (x=0;x<4;x++){
      Serial.print(andaresParaEntregar1[x],DEC);
      Serial.print("\t");
    }
    for (x=0;x<4;x++){
      Serial.print(pressed[x],DEC);
      Serial.print("\t");
    }
    Serial.print(andarAtualElevador1,DEC);
    Serial.print("\t"); 
    Serial.print("99");
    Serial.print("\t");
    delay(200);    
}
