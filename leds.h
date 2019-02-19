const byte Blue_Led = 2;
bool Blue_Led_State = 1 ;
Ticker tkBlue_Led ;

const byte Red_Led = 0;
bool Red_Led_State = 0;
Ticker tkRed_Led ;

/*Faire des fonctions simples à appeler depuis autres functions callbacks, les fonctions locales attachent et detachent les tkLeds

fonctions extérieures appellent blueLedState (0 off, 1 on, -1 blink; time_ms)
blueStateLed attache/détache les tickers sur fonction invert
*/

void blinkLed(int Led) {
  if (Led==0) {Red_Led_State = !Red_Led_State; digitalWrite(Red_Led, !Red_Led_State) ;}
  if (Led==1) {Blue_Led_State = !Blue_Led_State; digitalWrite(Blue_Led, !Blue_Led_State);}
}


void redLedState ( int state, int time_ms) { // -1 inverts, 0 off, 1 on
  if (state == -1) {
    tkRed_Led.attach_ms( time_ms, blinkLed, 0);
    }
  else {
    tkRed_Led.detach();
    Red_Led_State = state ;
    digitalWrite(Red_Led, !Red_Led_State) ; // inverted, 1 is off and 0 is on
    }
  
  //Serial.println("Red Led "+(String)Red_Led_State);
}

void blueLedState ( int state, int time_ms) { // -1 inverts, 0 off, 1 on
  if (state == -1) {
    tkBlue_Led.attach_ms( time_ms, blinkLed, 1);
    }
  else {
    tkBlue_Led.detach();
    Blue_Led_State = state ;
    digitalWrite(Blue_Led, !Blue_Led_State) ;
    }
   // inverted, 1 is off and 0 is on
  //Serial.println("Blue Led "+(String)Blue_Led_State);
}


void setupLeds() {
  pinMode(Blue_Led, OUTPUT);
  digitalWrite(Blue_Led, Blue_Led_State);
  pinMode(Red_Led, OUTPUT);
  digitalWrite(Red_Led, Red_Led_State);//red led on at boot
  delay(2000);
  
  blueLedState(-1, 100);
  redLedState(-1, 100);
  delay (500); // blink fast during 0.5 seconds
  
  redLedState(0, 100);
  blueLedState(0, 100); // then switch all leds off
  
}

