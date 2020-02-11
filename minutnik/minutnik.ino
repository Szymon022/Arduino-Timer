#include <LiquidCrystal.h>
#define restartButton 11
#define launchButton 12
#define timeSetButton 13
#define buzzer 10
LiquidCrystal lcd(2,3,4,5,6,7);

unsigned long debounceDelay = 50;

//RestartButton:
unsigned long lastRestartButtonTrigger = 0;
int restartButtonReading;
int lastRestartButtonState = HIGH;
int restartButtonState = HIGH;

//LaunchButton:
unsigned long lastLaunchButtonTrigger = 0;
int launchButtonReading;
int lastLaunchButtonState = HIGH;
int launchButtonState = HIGH;

//TimeSetButton:
unsigned long lastTimeSetButtonTrigger = 0;
int timeSetButtonReading;
int lastTimeSetButtonState = HIGH;
int timeSetButtonState = HIGH;

//Time set in seconds:
unsigned long masterTime = 0;
unsigned long lastTimerTrigger = 0;
int timeDifference = 60;
int timeSet = 0;
int timeLeft = 0;

//Buzzer
unsigned long buzzerDelay = 200;
unsigned long lastBuzzerTrigger = 0;
int buzzerState = LOW;
boolean buzzerON = false;

boolean timerON = false;


//takes both timeSet and timeLeft and shows bot times on LCD screen
void showTimeSet(int timeSet, int timeLeft){  
  int secondsSet = 0;
  int minutesSet = 0;
  int hoursSet = 0; 
   
  int secondsLeft = 0;
  int minutesLeft = 0;
  int hoursLeft = 0;

  hoursSet = timeSet / 3600;
  minutesSet = (timeSet - hoursSet*3600) / 60;
  secondsSet= (timeSet - hoursSet*3600 - minutesSet*60);
  
  hoursLeft = timeLeft / 3600;
  minutesLeft = (timeLeft - hoursLeft*3600) / 60;
  secondsLeft = (timeLeft - hoursLeft*3600 - minutesLeft*60);
    
  printLcdGrid(hoursSet, minutesSet, secondsSet, hoursLeft, minutesLeft, secondsLeft); 
}

//it is used by showTimeSet function as template for printing each times
void printLcdGrid(int hoursSet, int minutesSet, int secondsSet, int hoursLeft, int minutesLeft, int secondsLeft){
  lcd.setCursor(0,0);
  lcd.print("Set: ");
  
  //printing hours  
  if(hoursSet / 10 == 0){
    lcd.setCursor(8,0);
    lcd.print("0");
    lcd.setCursor(9,0);
    lcd.print(hoursSet);
  }
  else{
    lcd.setCursor(8,0);
    lcd.print(hoursSet);
  }
  
  lcd.setCursor(10,0);
  lcd.print(":");
  
  //printing minutes
  if(minutesSet / 10 == 0){
    lcd.setCursor(11,0);
    lcd.print("0");
    lcd.setCursor(12,0);
    lcd.print(minutesSet);
  }
  else{
    lcd.setCursor(11,0);
    lcd.print(minutesSet);
  }
  
  lcd.setCursor(13,0);
  lcd.print(":");
  
  //printing seconds
  if(secondsSet / 10 == 0){
    lcd.setCursor(14,0);
    lcd.print("0");
    lcd.setCursor(15,0);
    lcd.print(secondsSet);
  }
  else{
    lcd.setCursor(14,0);
    lcd.print(secondsSet);
  }
  //------------------------------------------
  //printing time that left
  
  lcd.setCursor(0,1);
  lcd.print("Left: ");
  
  //printing hours  
  if(hoursLeft / 10 == 0){
    lcd.setCursor(8,1);
    lcd.print("0");
    lcd.setCursor(9,1);
    lcd.print(hoursLeft);
  }
  else{
    lcd.setCursor(8,1);
    lcd.print(hoursLeft);
  }
  
  lcd.setCursor(10,1);
  lcd.print(":");
  
  //printing minutes
  if(minutesLeft / 10 == 0){
    lcd.setCursor(11,1);
    lcd.print("0");
    lcd.setCursor(12,1);
    lcd.print(minutesLeft);
  }
  else{
    lcd.setCursor(11,1);
    lcd.print(minutesLeft);
  }
  
  lcd.setCursor(13,1);
  lcd.print(":");
  
  //printing seconds
  if(secondsLeft / 10 == 0){
    lcd.setCursor(14,1);
    lcd.print("0");
    lcd.setCursor(15,1);
    lcd.print(secondsLeft);
  }
  else{
    lcd.setCursor(14,1);
    lcd.print(secondsLeft);
  }
}


void setup(){
  Serial.begin(9600);
  lcd.begin(16,2);
  
  pinMode(timeSetButton, INPUT_PULLUP);
  pinMode(launchButton, INPUT_PULLUP);
  pinMode(restartButton, INPUT_PULLUP);
  pinMode(buzzer, OUTPUT); 
  
  showTimeSet(0, 0);    
}

void loop(){
  masterTime = millis();
  if(timerON == false){
    lastTimerTrigger = masterTime;
  }
  //if timer is on (timeLeft != 0) we cannot change timSet - reading of set buttons are blocked by if statement  
  if(timeLeft == 0){
    timeSetButtonReading = digitalRead(timeSetButton);    
  }  
  launchButtonReading = digitalRead(launchButton);
  restartButtonReading = digitalRead(restartButton);
  
  
  //time set button code
  if(timeSetButtonReading != lastTimeSetButtonState){
    lastTimeSetButtonTrigger = millis();
  }  
  
  if(millis() - lastTimeSetButtonTrigger > debounceDelay){

    if(timeSetButtonReading != timeSetButtonState){
      timeSetButtonState = timeSetButtonReading; 
      //if tieSetButton is pressed then both timer and buzzer are turned OFF
      if(timeSetButtonState == LOW){
        buzzerON = false;
        timerON = false;
        digitalWrite(buzzer, LOW);
        timeSet += timeDifference;
        timeLeft = 0;
        showTimeSet(timeSet, timeLeft);
      }
    }
  }

  //launch button
  if(launchButtonReading != lastLaunchButtonState){
    lastLaunchButtonTrigger = millis();
  }

  if(millis() - lastLaunchButtonTrigger > debounceDelay){

    if(launchButtonReading != launchButtonState){
      launchButtonState = launchButtonReading;

      if(launchButtonState == LOW && timeSet != 0){
        //turns time to on/off
        timerON = !timerON;
        buzzerON = false;        
        digitalWrite(buzzer, LOW);
        
        //if timer is on and timeLeft is 0 then print both times: timeSet, timeLeft
        //it works for time setting, which means when timeLeft == 0 
        if(timerON == true && timeLeft == 0){
          timeLeft = timeSet;
          Serial.println("Start");
          
          showTimeSet(timeSet,timeLeft); 
        }
        
        //if timer is on, but time left != 0
        //it works when we want to start the timer after stoping it previously
        else if(timerON == true && timeLeft != 0){
          Serial.println("Start");          
          showTimeSet(timeSet,timeLeft);
        }
      } 
    }
  }

  //restart button
  if(restartButtonReading != lastRestartButtonState){
    lastRestartButtonTrigger = millis(); 
  }

  if(millis() - lastRestartButtonTrigger > debounceDelay){

    if(restartButtonReading != restartButtonState){
      restartButtonState = restartButtonReading;

      if(restartButtonState == LOW){
        buzzerON = false;
        timerON = false;
        digitalWrite(buzzer, LOW);
        
        //if time left is != 0 then we set bot timeSet and timeLeft to 0
        if(timeLeft != 0){
          //timerON = false;          
          timeSet = 0;
          timeLeft = 0;
          showTimeSet(timeSet,timeLeft);
          Serial.println("reset");
        }
        //if timeLeft is 0 we are able to decrease timeSet by timeDefference (default - 60 s)
        else if(timeLeft == 0){
          if(timeSet>0){
            timeSet -= timeDifference;
            showTimeSet(timeSet,timeLeft);
          }
        }
      }
    }
  }

  //seconds counter
  //if timer is on then timeLeft will decrease by 1 second every second. If the timeLeft is 0, then the buzzer will alarm
  if(timerON == true){    
    if(masterTime - lastTimerTrigger >= 1000){ 
          
      if(timeLeft > 0){
        timeLeft -= 1;
      }
     //if timeLeft = 0 then the buzzer will turn ON. We can turn the buzzer OFF  by clicking any button
      else{
        buzzerON = true;        
      }          
      showTimeSet(timeSet, timeLeft);
      lastTimerTrigger = masterTime;
    }    
  }
  if(buzzerON == true){    
    if(masterTime - lastBuzzerTrigger >= buzzerDelay){
      buzzerState = !buzzerState;
      digitalWrite(buzzer, buzzerState);
      lastBuzzerTrigger = masterTime; 
    }
  }
  
  lastTimeSetButtonState = timeSetButtonReading;
  lastLaunchButtonState = launchButtonReading;
  lastRestartButtonState = restartButtonReading;
}
