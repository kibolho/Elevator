//#include <AccelStepper.h>
//#define HALFSTEP 8
#include <Stepper.h> // Incluir Biblioteca 'Stepper'


int passos = 500; // Número de passos dados
const int passosPorVolta = 500;

// Motor pin definitions
#define motorPin1  9     // IN1 on the ULN2003 driver 1
#define motorPin2  10     // IN2 on the ULN2003 driver 1
#define motorPin3  11    // IN3 on the ULN2003 driver 1
#define motorPin4  12     // IN4 on the ULN2003 driver 1
int UpEnd = 8;
int DownEnd = 7;
int bt1 = 6;
int bt2 = 5;
int bt3 = 4;
int bt4 = 3;
int portaAberta = 13;
#define tempoPortaAberta 2000
long tempoParado = 0;
int parado = 1;

Stepper myStepper(passosPorVolta, 9, 11, 10, 12); 
// Initialize with pin sequence IN1-IN3-IN2-IN4 for using the AccelStepper with 28BYJ-48
//AccelStepper stepper1(HALFSTEP, motorPin1, motorPin3, motorPin2, motorPin4);
int andarAtual = 4;
int andaresAtender[4]={0,0,0,0};
int andaresEntregar[4]={0,0,0,0};

//DEBOUNCE DE TODOS OS PINOS DE INPUT
#define DEBOUNCE 10  // button debouncer, how many ms to debounce, 5+ ms is usually plenty

byte buttons[] = {bt1, bt2, bt3, bt4, UpEnd, DownEnd};
//determine how big the array up above is, by checking the size
#define NUMBUTTONS sizeof(buttons)
 
//track if a button is just pressed, just released, or 'currently pressed' 
byte pressed[NUMBUTTONS], justpressed[NUMBUTTONS], justreleased[NUMBUTTONS];
byte previous_keystate[NUMBUTTONS], current_keystate[NUMBUTTONS];

void setup() {
  Serial.begin(9600);
  delay(10);
  Serial.print("Setup");
  pinMode(motorPin1, OUTPUT);
  pinMode(motorPin2, OUTPUT);
  pinMode(motorPin3, OUTPUT);
  pinMode(motorPin4, OUTPUT);
  pinMode(portaAberta, OUTPUT);
  pinMode(bt1, INPUT_PULLUP);
  pinMode(bt2, INPUT_PULLUP);
  pinMode(bt3, INPUT_PULLUP);
  pinMode(bt4, INPUT_PULLUP);
  pinMode(UpEnd, INPUT_PULLUP);
  pinMode(DownEnd, INPUT_PULLUP);

  myStepper.setSpeed(60); // velocidade do motor em 60;
  delay(50);
  while(digitalRead(UpEnd)==1){
    myStepper.step(-passos); // sentido horário
    delay(10);
  }
  /*
  stepper1.stop();
  Serial.print("End Setup\n");
  */

}//--(end setup )---

void loop() {
  byte thisSwitch=thisSwitch_justPressed();
  switch(thisSwitch){ 

    //ELEVADOR1 CHAMOU ANDAR 1
    case 0: 
      andaresAtender[0] = 1;
    break;

    //ELEVADOR1 CHAMOU ANDAR 2
    case 1: 
      andaresAtender[1] = 1;
    break;

    //ELEVADOR1 CHAMOU ANDAR 3
    case 2: 
      andaresAtender[2] = 1;
    break;

    //ELEVADOR2 CHAMOU ANDAR 4
    case 3: 
      andaresAtender[3] = 1;
    break;

    //ELEVADOR2 CHEGOU ANDAR 4
    case 4: 
      parar(4);
      andarAtual = 4;
    break;

    //ELEVADOR2 CHEGOU ANDAR 1
    case 5: 
      parar(1);
      andarAtual = 1;
    break;  
  }

    Serial.print(andaresAtender[0]);  
  Serial.print("\t");  
  Serial.print(andaresAtender[1]);
    Serial.print("\t");
  Serial.print(andaresAtender[2]);
    Serial.print("\t");
  Serial.print(andaresAtender[3]);
    Serial.print("\t");
  Serial.print(andarAtual);  
  Serial.print("\n");
  if(millis()-tempoParado>tempoPortaAberta){
    switch (andarAtual) {
        case 1:
          if(andaresAtender[0]&&parado)
            parar(1);
          else if(andaresAtender[1]||andaresAtender[2]||andaresAtender[3])
            subir(1);
          break;
        case 2:
          if(andaresAtender[1]&&parado)
            parar(2);
          else if(andaresAtender[0])
            descer(4);
          else if(andaresAtender[2]||andaresAtender[3])
            subir(1);
          break;
        case 3:
           if(andaresAtender[2]&&parado)
            parar(3);
          else if(andaresAtender[3])
            subir(4);
          else if(andaresAtender[0]||andaresAtender[1])
            descer(1);
          break;
        case 4:
          if(andaresAtender[3]&&parado)
            parar(4);
          else if(andaresAtender[0]||andaresAtender[1]||andaresAtender[2])
            descer(1);
          break;
        default:
        break;
          // do something
    }
  }
}
void subir(int andar){
  parado = 0;
  digitalWrite(portaAberta, LOW);
  switch (andar) {
      case 1:
          myStepper.step(-passos); // sentido horário
          delay(100);
          parar(andarAtual+1);
        break;
      case 4:
         while(digitalRead(UpEnd)==1){
          myStepper.step(-passos); // sentido horário
          delay(100);
        }
        parar(4);
        break;
      default:
        break;
        // do something
  }
}
void descer(int andar){
  parado = 0;
  digitalWrite(portaAberta, LOW);
  switch (andar) {
      case 1:
          myStepper.step(passos); // sentido horário
          delay(100);
          parar(andarAtual-1);
        break;
      case 4:
         while(digitalRead(DownEnd)==1){
          myStepper.step(passos); // sentido horário
          delay(100);
        }
        parar(1);
        break;
      default:
      break;
        // do something
  }
}
void parar(int andar){
  andarAtual=andar;
  parado = 1;
  tempoParado=millis();
  digitalWrite(portaAberta, HIGH);
  switch (andar) {
     case 1:
          andaresAtender[0]=0;
     break;
     case 2:
          andaresAtender[1]=0;
     break;
     case 3:
          andaresAtender[2]=0;
     break;
     case 4:
          andaresAtender[3]=0;
     break;
     default:
     break;
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
