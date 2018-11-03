/*
  Breakin' Convention Denvoer 2018
  by Ho Yun "Bobby" chan
  10/26/2018

  Wireless Controlled LED

*/



/*hardware UART of the Atmega328 for communication:
   pin 0 = Rx
   pin 1 = Tx
*/

char val; //Declare character 'val' when Slave XBee receives a character
char temp_delete; //used to delete buffer and prevent false triggers when Master XBee sends character more than once

//LED to check if the LED is initialized.
const int status_LED = 13;

int counter = 0; //adding counter to prevent false triggers for a small period of time
boolean XBee_sent = false; //flag to see if we have received any characters after a certain period of time

/*---------- LEDs ----------*/

int ledR = 5;//hardware PWM
int ledG = 6;//hardware PWM
int ledB = 9; //hardware PWM

int prev_FadeVal = 0;
int current_FadeVal = 0;
boolean increasing = true;
boolean start_fade = false;

void setup() {
  Serial.begin(9600);
  Serial.println("EL Sequencer's XBee is Ready to Receive Characters");

  val = 'A'; //save as default character
  pinMode(status_LED, OUTPUT); //Set pin mode as output for status LED

  //Status LED to see if the EL Sequencer is initializing
  for (int i = 0; i < 3; ++i) {
    digitalWrite(status_LED, HIGH);//set Status LED on
    delay(50);
    digitalWrite(status_LED, LOW); //set Status LED off
    delay(50);
  }

  // initialize the digital pins as an output for LED Strip
  pinMode(ledR, OUTPUT);
  pinMode(ledG, OUTPUT);
  pinMode(ledB, OUTPUT);
  sequenceTest();//visually initialization
  allOFF(); //initialize LEDs with it turned off

}

void loop() {
  if (XBee_sent == false) {
    //we have not received a character yet after a certain period of time, we can see if the Master XBee has sent any characters
    if (Serial.available()) {
      //check if slave XBee is receiving data from master XBee
      val = Serial.read();//save whatever is in the buffer to the variable
      counter = 0;        //set counter to 0 to prevent false button presses
      XBee_sent = true;   //we have received a character

      if (start_fade == false){//start fade at same time by receiving any character
      start_fade = true;
      prev_FadeVal = 0;
      current_FadeVal = 0;
    }
    //if debugging, we can see what character is recevied
    Serial.print("Character Received = ");
      Serial.println(val);

      /*//Check to see if character sent is any of the recognized characters and jump to the sequence
        if (val == 'A') {
        redON();//RED
        }
        else if (val == 'B') {
        magentaON();//CYAN
        }
        else if (val == 'C') {
        clearblueON();//BLUE
        }

      */

    }//end buffer check
  }//end check to see if we have not received a character after a certain period of time

  if (counter >= 10) {//this section of code will reset the flag "XBee_Sent" so we can begin listening for characters again
    if (XBee_sent == true) {
      Serial.println("Counter = 10, we are ready to receive characters again");
    }
    XBee_sent = false;
  }

  if (XBee_sent == true) {//this section of code is used as a delay to prevent false button presses
    counter = ++counter;//keep adding until we reach 10, then we can reset flag and begin receiving again

    //if connected to a computer, check to see the duration of the delay
    Serial.print("Counter = ");
    Serial.println(counter);

    temp_delete = Serial.read();//try to clear false triggers in buffer provided by Master XBee until counter resets
  }

  patternFade();//update fade
}

/*
  Check out the Venn Diagram on Basic Color Mixing:

  https://learn.sparkfun.com/tutorials/lilypad-protosnap-plus-activity-guide/2-basic-color-mixing

  ROYGBIV...
  a 9V battery is not able to fully power all three LEDs simultaneously...
  MODE
  1.) red              = red[HIGH]
  .) tangerine orange =
  2.) yellow           = red[HIGH]+ green[HIGH]
  3.) green            =          + green[HIGH]
  4.) clear blue       =          + green[HIGH] + blue[HIGH]
  5.) blue             =                        + blue[HIGH]
  6.) magenta          = red[HIGH]+             + blue[HIGH]
  .) white            = red[HIGH]+ green[HIGH] + blue[HIGH]
*/

void allOFF() { //MODE 0
  analogWrite(ledR, 0);
  analogWrite(ledG, 0);
  analogWrite(ledB, 0);
}

void redON() { //MODE 1
  analogWrite(ledR, 255);
  analogWrite(ledG, 0);
  analogWrite(ledB, 0);
}

void yellowON() { //MODE 2
  analogWrite(ledR, 255);
  analogWrite(ledG, 255);
  analogWrite(ledB, 0);
}

void greenON() { //MODE 3
  analogWrite(ledR, 0);
  analogWrite(ledG, 255);
  analogWrite(ledB, 0);
}

//CYAN
void clearblueON() { //MODE 4
  analogWrite(ledR, 0);
  analogWrite(ledG, 255);
  analogWrite(ledB, 255);
}

void blueON() { //MODE 5
  analogWrite(ledR, 0);
  analogWrite(ledG, 0);
  analogWrite(ledB, 255);
}

void magentaON() { //MODE 6
  analogWrite(ledR, 255);
  analogWrite(ledG, 0);
  analogWrite(ledB, 255);
}

void whiteON() { //MODE 7 //not possible using 9V battery to power all on
  analogWrite(ledR, 150);
  analogWrite(ledG, 150);
  analogWrite(ledB, 150);
}

void sequenceTest() {
  redON();
  delay(50);
  allOFF();
  delay(50);

  yellowON();
  delay(50);
  allOFF();
  delay(50);

  greenON();
  delay(50);
  allOFF();
  delay(50);

  clearblueON();
  delay(50);
  allOFF();
  delay(50);

  blueON();
  delay(50);
  allOFF();
  delay(50);

  magentaON();
  delay(50);
  allOFF();
  delay(50);

  whiteON();
  delay(50);
  allOFF();
  delay(50);

}


void patternFade() {

  switch (val) {
    case 1://FADE RED
      analogWrite(ledR, prev_FadeVal);
      analogWrite(ledG, 0);
      analogWrite(ledB, 0);
      break;
    case 2://FADE YELLOW
      analogWrite(ledR, prev_FadeVal);
      analogWrite(ledG, prev_FadeVal);
      analogWrite(ledB, 0);
      break;
    case 3://FADE GREEN
      analogWrite(ledR, 0);
      analogWrite(ledG, prev_FadeVal);
      analogWrite(ledB, 0);
      break;
    case 5://FADE BLUE
      analogWrite(ledR, 0);
      analogWrite(ledG, 0);
      analogWrite(ledB, prev_FadeVal);
      break;
    case 'B'://FADE MAGENTA
      analogWrite(ledR, prev_FadeVal);
      analogWrite(ledG, 0);
      analogWrite(ledB, prev_FadeVal);
      break;
    case 'C'://FADE CLEAR BLUE
      analogWrite(ledR, 0);
      analogWrite(ledG, prev_FadeVal);
      analogWrite(ledB, prev_FadeVal);
      break;
    case 'O'://all off
      analogWrite(ledR, 0);
      analogWrite(ledG, 0);
      analogWrite(ledB, 0);
      break;
    default:
      analogWrite(ledR, prev_FadeVal);// DEFAULT RED 'A'
      analogWrite(ledG, 0);
      analogWrite(ledB, 0);
      break;
  }
  delay(30);


  if (increasing == true) {
    current_FadeVal += 5;
  }
  else { //decreasing
    current_FadeVal -= 5;
  }

  if (current_FadeVal > 255) {
    increasing = false;
    prev_FadeVal -= 5;//undo addition
    current_FadeVal = prev_FadeVal;

  }
  else if (current_FadeVal < 5) {
    increasing = true;
    prev_FadeVal += 5;//unto subtraction
    current_FadeVal = prev_FadeVal;
  }

  prev_FadeVal = current_FadeVal;
}
