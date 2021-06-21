/*
ACS 233 Group 9 Assignment 3 - Burglar Alarm Program.

Sebastiano Zuddas, Radu Rotaru, Waleed Hamad, Tsun Ho Woo.

  NAME: ABM Alarm
    DATE: 24/01/2020
 
 The code below shows the functionality of an anti-burglar alarm system, presented on an LCD Screen,
 which is controlled by an IR Remote. 
 This system is made to cater a set of user and system requirements, where by the user is able to on set up and control the system with ease. 
 The system consists of 3 slide switches,3 PIRs and 3 Buzzers which will be scattered around the house. 
 Our layout of the building suggests to install the Switches onto 1 Door and 1 Window.
 The 3 PIRs onto 3 Rooms; each room containing a Buzzer.
 The system operates when the user presses the RED Button on the IR Remote
 This enables the machine to turn on to standby mode.
 The user then prompts the machine by pressing any button to then authorise themselves to the system
 When authorisation is complete, the user is redirected to the main menu where they will be able to select one of five options;
 "1.ARM", "2.DISARM", "3.COUNTDOWN TIMER", "4.CHANGE PIN", "5.POWER OFF"
 The user will be able to use the 'back' button to go to previous operations and is able to use the 'pause/start' button to move forward in the operation.
 The user has a 20 seconds timeout interval, where if no input from the remote is detected, the user is redirected back to the previous operation.
 The user is unable to turn off the system when ARMED.
 Buzzer is triggered at any moment the switches open or the PIR detects movement for more than 3 seconds
 PIR Reads an analogue value in the range of 0 to 1024. when it reaches the maximum value; Buzzer Triggers.
*/


#include <LiquidCrystal.h>
#include <IRremote.h>

#define DOOR_SW A0
#define WINDOW_SW A1
#define PIR_R1 A2
#define PIR_R2 A3
#define PIR_R3 A4
#define RECV_PIN 7 //IR RECEIVER PIN 
#define SRN_LED 8
#define BUZZER 10
#define CTD_LED 13

IRrecv irrecv(RECV_PIN);
decode_results results;
LiquidCrystal lcd(12, 11, 5, 4, 3, 2); // LCD dital pins

char* options[] = {"1.ARM", "2.DISARM", "3.CTD TIMER", "4.CHANGE PIN", "5.POWER OFF"}; //Menu Options

bool ARMED = false; //System starts UNARMED
bool SHUT_DOWN = true; //System is OFF when simulation starts

long PIN = 12345; //STARTER PIN
long CTD_TIME = 5; //COUNTDOWN TIME 
long TIME_PERIOD = 1800; //Buzzer will turn off after 30 minutes (1800 seconds)
int INTERVAL = 20; //User has a limited time to request a keyboard input, if no inputs are received go back
int ATTEMPTS = 3;



void auth(){ //Authentification Function
  lcd.clear();
    lcd.print("AUTHENTICATION!");
    lcd.setCursor(0, 1);
    lcd.print("PIN:");
    delay(500);
}

long remote_input(int cursor_val, unsigned long calledTime, bool *no_input_received){ //Read the inputs from the remote
  
  long ATTEMPT = 0; //ATTEMPT = The requested input from the keyboard
  bool exit_status = false;
  unsigned long startTime; 
  unsigned int value;
  
  lcd.setCursor(cursor_val, 1);
  do{
    startTime = millis()/1000; // Determine the time from the start of arduino
    lcd.blink();
    delay(150);
    lcd.noBlink();
    delay(150);
    if(startTime - calledTime > TIME_PERIOD){ //Compare the running time with the INTERVAL(sec), if time expired user is redirected
      //This is also used to determine the 30 minutes period for which the alarm should sound 
      *no_input_received = true;
      break;
    }
    if (irrecv.decode(&results)) {//irrecv.decode(&results) returns true if anything is recieved, and stores info in varible results  
          value = results.value;      
          switch (value) { //Switch case on unique integar codes generating by the IR Reciever
                    case 12495: 
                      if(ATTEMPT != 0){
                  lcd.print("0");
                  ATTEMPT = ATTEMPT * 10;
                        }
                        break;
              case 2295: 
                lcd.print("1");
                ATTEMPT = ATTEMPT * 10 + 1; 
                break;
              case 34935: 
                lcd.print("2");
                ATTEMPT = ATTEMPT * 10 + 2;
                break;
              case 18615: 
                lcd.print("3");
                ATTEMPT = ATTEMPT * 10 + 3;
                break;
              case 10455:
                lcd.print("4");
                      ATTEMPT = ATTEMPT * 10 + 4;
                      break;
                    case 43095: 
                lcd.print("5");
                ATTEMPT = ATTEMPT * 10 + 5;
                break;
                    case 26775:     
                lcd.print("6");
                ATTEMPT = ATTEMPT * 10 + 6;
                break;
                  case 6375: 
                lcd.print("7");
                ATTEMPT = ATTEMPT * 10 + 7;
                break;
                    case 39015: 
                lcd.print("8");
                ATTEMPT = ATTEMPT * 10 + 8;
                break;
                    case 22695: 
                lcd.print("9");
                ATTEMPT = ATTEMPT * 10 + 9;
                break;
                    case 8415: // Back button, go back
                exit_status = true;
                      *no_input_received = true; // Don't send input back
                    case 41055: //ENTER = EXIT FOLLOW INSTRUCTIONS FROM THE MAIN FUNCTION
                      exit_status = true;
                break;
          }
            
            irrecv.resume(); // Receive the next value     
              }
  }while(!exit_status); //Exit if Enter or Back button is pressed
  lcd.clear(); // In case back button is pressed
  return ATTEMPT;
  
}
void trigger(){
 
  unsigned long startTime;
  unsigned long triggeredTime = millis()/1000; //Get the time when the function was called
  bool no_input_received = false;
  bool button_pressed;
  long ATT_PIN;
  int cursor_val = 4;
  
  digitalWrite(SRN_LED, HIGH);
  analogWrite(BUZZER, 127);//Buzz with a PWM of 127
  delay(100);
  
  int attempts;
  do{
  attempts = 3; 
    lcd.clear();
    lcd.print("-------ALARM-------");
    lcd.setCursor(0, 1);
    lcd.print("  PROVIDE PIN NOW!");
    lcd.setCursor(0, 0);
    
    button_pressed = false;
    do{
        startTime = millis()/1000; //Get the time constantly to determine the run time
      lcd.scrollDisplayLeft(); //Scroll text to the left
      if (irrecv.decode(&results)) {//irrecv.decode(&results) returns true if anything is recieved, and stores info in varible results  
        
        irrecv.resume(); //Receive the next value
            button_pressed = true;
      }
        digitalWrite(SRN_LED, LOW); //Flash the red LED at 4hz
      delay(250);
        digitalWrite(SRN_LED, HIGH);
        if(startTime - triggeredTime > TIME_PERIOD) //If time exceeds 30 minutes while systm is waiting for input, break
              break;
    }while(!button_pressed); //While no button is pressed, ALARM notification should stay on until user presses a button
    
    if(startTime - triggeredTime > TIME_PERIOD) //In case no inputs are received, break if time exeeds 30 minutes
              break;
    
    auth(); //Keyboard was pressed, display authorisation mesage
    
    ATT_PIN = remote_input(cursor_val, triggeredTime,  &no_input_received); //Get the attempted PIN
    while(ATT_PIN!=PIN) {
        attempts --;
        startTime = millis()/1000;
        if(attempts == 0 || startTime - triggeredTime > TIME_PERIOD) //If no of attempts exceed or time limit exceed then break.
              break; 
      lcd.clear();
      lcd.print("INCORRECT PIN!");
        lcd.setCursor(0, 1);
      lcd.print("ATTEMPTS LEFT " + String(attempts));
        digitalWrite(SRN_LED, LOW);
        delay(1500);
        digitalWrite(SRN_LED, HIGH);
        auth(); //Display authorisation mesage
      ATT_PIN = remote_input(cursor_val, triggeredTime,  &no_input_received); //Authorise using the Attempted PIN
    }
    if(attempts == 0){ //No attempts left, system is entering the PREVENTION MODE
      lcd.clear();
      lcd.print("USER PREVENTION!");
      lcd.setCursor(0, 1);
      lcd.print("------WAIT------");
        for(int index = 0; index < 480; index ++){ //System is preventing PIN spam for two minutes
            digitalWrite(SRN_LED, HIGH);
            lcd.scrollDisplayLeft(); //Keep scrolling the message
            delay(125);
            digitalWrite(SRN_LED, LOW); //Flash the red LED at 8hz
            delay(125);
        }
    } 
  }while(attempts<=0); // Keep repeating until the PIN is entered corectly 
  lcd.clear();
  lcd.print("BUZZER STOPPED!"); //PIN entered corectly or 30 minutes passed
  analogWrite(BUZZER, 0); //Stop the buzzer
  digitalWrite(SRN_LED, LOW); // Turn off the LED
  ARMED = false; //System will enter the STAND-BY mode
  ATTEMPTS = 3; //Reset the attempt counter
  delay(1500);
  
}
void arm(){//ARM Function
  
  if (analogRead(DOOR_SW) > 1003 || analogRead(WINDOW_SW) > 1003){
    lcd.clear();
  lcd.print("DOOR OR WINDOW");
    lcd.setCursor(0, 1);
  lcd.print("OPEN!");
    delay(2000);
  }
  else{
  
      lcd.clear();
    lcd.print("SYSTEM IS ARMING");
    lcd.setCursor(0, 1);
    lcd.print("     WAIT");
    for(int index = 0; index <3; index++){
              delay(500);
              lcd.print(".");
              delay(500);
      } 
      lcd.clear();
    lcd.print("COUNTDOWN IN"); 
    lcd.setCursor(0, 1);
    lcd.print("PROGRESS.. ");
      for(int index = CTD_TIME; index >0 ; index--){ //Engage the countdown timer
          lcd.setCursor(11, 1); 
          digitalWrite(CTD_LED, HIGH);//Flash the green LED
          delay(500);
          digitalWrite(CTD_LED, LOW);
            lcd.print(index); //Print the remaining time 
          lcd.print(" ");
            delay(500);
      }
      ARMED = true; //System is armed, entering ARMED mode
  }

}
void disarm() { //Disarm Function
  
  if(ARMED){ //Check if the system is ARMED
    lcd.clear();
    lcd.print("SYSTEM");
    lcd.setCursor(0, 1);
    lcd.print("DISARMED!");
    ARMED = false; //Disarm the system
    delay(2000);
  }
  else{ //Prevention is case the system is not ARMED 
    lcd.clear();
    lcd.print("SYSTEM IS NOT");
    lcd.setCursor(0, 1);
    lcd.print("ARMED!");
    delay(2000);
  }
  
}
void countdown_timer(){ //Change Countdown Time Function
  
  unsigned long calledTime = millis()/1000; //Get the time when the function was called
  bool  no_input_received = false;
  int cursor_val = 9;
  long NEW_TIME;
  
  lcd.clear();
  lcd.print("PLEASE INPUT NEW");
  lcd.setCursor(0, 1);
  lcd.print("CTD TIME:");
  delay(1000);
  
  NEW_TIME = remote_input(cursor_val, INTERVAL + calledTime - TIME_PERIOD, &no_input_received); //Get the value of the new countdown timer
  if( (NEW_TIME < 30 || NEW_TIME > 600) && !no_input_received) // NO LESS THAN 30 sec / NO MORE THAN 5 minutes (600 sec)
    do {
      lcd.clear();
      lcd.print("INVALID INPUT!");
        delay(2000);
      lcd.clear();
      lcd.print("PLEASE INPUT NEW");
      lcd.setCursor(0, 1);
      lcd.print("CTD TIME:");
      delay(1000);
      cursor_val = 9; //Inital position of digits on the LCD
      NEW_TIME = remote_input(cursor_val, INTERVAL + calledTime - TIME_PERIOD, &no_input_received); 
    }while( (NEW_TIME < 30 || NEW_TIME > 600) && !no_input_received); //Exit if correct input given or no inputs were received
  if(!no_input_received){
    CTD_TIME = NEW_TIME;
    lcd.clear();
    lcd.print("NEW CDT SET!");
    delay(2000);
  }

} 
void change_pin(){ //Change pin function
  
  unsigned long calledTime = millis()/1000; //Get the time when the function was called
  bool  no_input_received = false;
  int cursor_val = 4; //Inital position of digits on the LCD
  
  lcd.clear();
  lcd.print("PLEASE INPUT NEW");
  lcd.setCursor(0, 1);
  lcd.print("PIN:");
  delay(1000);
  
  Serial.println(calledTime);
  long NEW_PIN = remote_input(cursor_val, INTERVAL + calledTime - TIME_PERIOD, &no_input_received ); 
  if( (NEW_PIN > 99999 || NEW_PIN < 10000) && !no_input_received)
    do {
      lcd.clear();
        lcd.setCursor(0, 0);
      lcd.print("INVALID PIN!");
        delay(2000);
        cursor_val = 4;
        lcd.clear();
      lcd.print("PLEASE INPUT NEW");
      lcd.setCursor(0, 1);
      lcd.print("PIN:");
      NEW_PIN = remote_input(cursor_val, INTERVAL + calledTime - TIME_PERIOD,  &no_input_received); 
    }while( (NEW_PIN > 99999 || NEW_PIN < 10000) && !no_input_received); //Exit if correct input given or no inputs were received
  if(!no_input_received){
    PIN = NEW_PIN;
    lcd.clear();
    lcd.print("NEW PIN SET!");
    delay(2000);
  }
  
} 
void power_off(bool * shut_down){ //Power off Function
  
  bool exit_status = false;
  unsigned int value;
  lcd.clear();
  lcd.print("PRESS RED BUTTON OR BACK TO CANCEL");
  
  do{
    lcd.scrollDisplayLeft();  
    delay(250); 
    if (irrecv.decode(&results)) {//irrecv.decode(&results) returns true if anything is recieved, and stores info in varible results  
      value = results.value;
        
        if(value == 255){ //Power button, shut down the system, pass the value to option_menu()
          lcd.clear();
        lcd.print("SHUTTING DOWN");
            for(int index = 0; index <3; index++){
              delay(400);
              lcd.print(".");
                delay(400);
            }
            
          exit_status = true;
            *shut_down = true;
        }
      else
        if(value == 8415){ //Back button, go back to the menu selection
        exit_status = true;
      }
      irrecv.resume(); //Receive the next value
    }
  }while(!exit_status);
  
} 
void option_menu(bool *shut_down_to_loop){

  ATTEMPTS = 3; // Reset the attempt counter
  unsigned long calledTime = millis()/1000; // Get the time when the function was called
  unsigned long runTime; // Time the function is running for
  unsigned int value;
  bool shut_down = false; 
  bool exit_status = false; // Exit if the back button is pressed or the user wants to shut down 
  int selected = 0; //Navigate through the options
  int cursor_position = 0; //Either first row or second row
 
  lcd.clear();
  do{
    runTime = millis()/1000; //Get the time at every step
    
    if(runTime - calledTime > INTERVAL){ //If more than INTERVAL (sec) passed, return to STAND-BY / ARMED mode
        break;
    }
    lcd.setCursor(0, 0);
    lcd.print(options[selected]);
    lcd.setCursor(0, 1);
    lcd.print(options[selected +1]);
    lcd.setCursor(0, cursor_position);
    lcd.cursor();
  delay(300);
    lcd.noCursor();
    delay(300);
    if (irrecv.decode(&results)) {//irrecv.decode(&results) returns true if anything is recieved, and stores info in varible results  
      value = results.value;
        switch (value) {
          case 4335: // Navigate down
              if(cursor_position == 0) 
                cursor_position ++;        
              else          
                selected ++;     
            break;
            case 20655: // Navigate up
              if(cursor_position == 1)
                cursor_position --;        
              else
                selected --;  
            break;
            case 8415: // Back button, return to stand-by / armed state
              exit_status = true;
            break;
            case 41055: //Enter button
              int option =  selected + cursor_position; //Get the selected option
              irrecv.resume(); // Receive the next value
              
              if(option == 1)
                  disarm();
              
              if(!ARMED) //Prevent access if the system is ARMED
                switch (option){
                  case 0:
                        arm(); //System will be armed
                        exit_status = true; //Return to ARMED state
                          break;
                      case 2:
                        countdown_timer();
                        break;
                      case 3:
                        change_pin();
                        break; 
                      case 4:
                        power_off(&shut_down); 
                        *shut_down_to_loop = shut_down; //Pass this value to loop()
                        if(shut_down){ //Retrun to stand-by mode and shut down the system
                          lcd.clear();
                              exit_status = true;
                          }
                      break; 
                }
                else{
                       lcd.clear();
                       lcd.print("DISARM SYSTEM");
                       lcd.setCursor(0, 1);
                       lcd.print("FIRST!");
                       delay(2000);       
                }
            break;
            
        }
        if(selected == -1){ //In case the first option is selected, go to last
          selected =3;
            cursor_position = 1;
        }
       if(selected == 4) { //In case the last option is selected, go to first 
          selected = 0;
            cursor_position = 0;
        }
        calledTime = millis()/1000; //Reset the time if any button is pressed
      irrecv.resume(); // Receive the next value
      lcd.clear(); //Prevent overwrittings on the LCD
    }
  }while(!exit_status);
  lcd.clear(); //Clear the LCD before returing to STAND-BY / ARMED
  
} 
void setup() {
  
  Serial.begin(9600);
  Serial.println("Enabling IRin");
  irrecv.enableIRIn(); // Enable the infra-red reading
  Serial.println("Enabled IRin");
  lcd.begin(16, 2);
  pinMode(BUZZER, OUTPUT);
  
}
void loop() { 

  delay(250);
  unsigned int value;
  unsigned long calledTime;
  bool  no_input_received = false;
  long ATT_PIN;
  int cursor_val = 4;
  
  if(SHUT_DOWN){ //Turn on the system if it's off
    do{
      if (irrecv.decode(&results)) {//irrecv.decode(&results) returns true if anything is recieved, and stores info in varible results
        value = results.value;
          if(value == 255){ //Power button is pressed, turn on the system
            lcd.clear();
          lcd.print("TURNING ON");
              for(int index = 0; index <3; index++){
                delay(400);
                lcd.print(".");
                  delay(400);
            }
            SHUT_DOWN = false; //System will turn on
          }
        irrecv.resume(); //Receive the next value
      }
      
    }while(SHUT_DOWN); //Continously check if the power button is pressed
  }
  
  if(ARMED){ //The system is ARMED, check for the sensor status
    lcd.setCursor(0, 0);
    lcd.print("ARMED!");
    delay(300);
    if(analogRead(DOOR_SW) > 1003 || analogRead(WINDOW_SW) > 1003 || analogRead(PIR_R1) > 1003
      || analogRead(PIR_R2) > 1003 || analogRead(PIR_R3) > 1003) //If sensors are detect movement/change is state
     
      trigger(); //Trigger the alarm
  }
  else{ //The system is in STAND-BY MODE 
    lcd.setCursor(0, 0);
    lcd.print(" STAND-BY MODE!");
    delay(300);
  }
  if (irrecv.decode(&results)) {//irrecv.decode(&results) returns true if anything is recieved, and stores info in varible results
      calledTime = millis()/1000; //Get the time from when the keyboard was pressed
      irrecv.resume(); // Receive the next value  
      auth();

      ATT_PIN = remote_input(cursor_val, INTERVAL + calledTime - TIME_PERIOD, &no_input_received); //Get the attempted pin
      if(!no_input_received)
        while(ATT_PIN!=PIN) { //Incorect pin received
            
              ATTEMPTS --;
          lcd.clear();
          lcd.print("INCORRECT PIN!");
              lcd.setCursor(0, 1);
          lcd.print("ATTEMPTS LEFT " + String(ATTEMPTS));
              delay(1500);
              if(ATTEMPTS == 0)
                  break;
              lcd.clear();
          lcd.print("AUTHENTICATION! ");
          lcd.setCursor(0, 1);
          lcd.print("PIN:");
              calledTime = millis()/1000; //RESET FOR EVERY ATTEMPT
          ATT_PIN = remote_input(cursor_val, INTERVAL + calledTime - TIME_PERIOD,  &no_input_received);
              if(no_input_received)
                  break;
        }
        if(!no_input_received)  
        if (ATTEMPTS >0){
              lcd.clear();
              lcd.print("    WELCOME!  ");
              lcd.setCursor(0, 1);
              lcd.print(" CHOOSE OPTION: ");
              delay(1500);
          option_menu(&SHUT_DOWN);
              }
        else
              trigger();
    }
 
}
