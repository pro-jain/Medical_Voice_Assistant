#include <Servo.h>  
#include <LiquidCrystal.h>
#include <RTC.h>

Servo servo; // create servo object

// constant variables for pins
const int servoPin = 6;
const int buttonPin = 9;
const int ledPin = 7;
const int buzzerPin = 8;

// other variables
int angle = 0;
int angleIncrement = 45;  // default 45 degrees for 4 compartments, change for different number of compartments
int newAngle;
int buttonState;
int movementDelay = 50;
int debounceDelay = 1000;

// time variables
int year;
int month;
int day;
int hour;
int minutes;
int seconds;

// initialize the library by associating any needed LCD interface pin
// with the arduino pin number it is connected to
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup(){ // setup code that only runs once
  pinMode(buttonPin, INPUT); // set button pin as input
  pinMode(ledPin,OUTPUT);    // set LED pin as output
  pinMode(buzzerPin,OUTPUT); // set buzzer pin as output
  digitalWrite(ledPin,LOW);  // make sure LED is off
  digitalWrite(buzzerPin,LOW);  // make sure buzzer is off
  servo.attach(servoPin);    // attach servo object to servoPin
  servo.write(angle);        // set servo to initial angle
  Serial.begin(9600);        // initialize serial for debugging

  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);

  // initialize the RTC and set the current date and time
  // you will need to manually adjust the time right before you upload the code
  RTC.begin();
  RTCTime startTime(12, Month::APRIL, 2024, 8, 59, 50, DayOfWeek::FRIDAY, SaveLight::SAVING_TIME_ACTIVE);
  RTC.setTime(startTime);
}

void loop(){  // code that loops forever
  updateLCD();  // display the current date and time on the screen (see function below)
  
  buttonState = digitalRead(buttonPin); // read button state (you can edit the code to advance the servo when you press the button, useful for debugging)
  Serial.println(angle);                // print the servo angle

  // detect certain times and rotate the servo mechanism ahead by one compartment. 
  // the example code rotates the servo once per minute when the seconds variable equals 0.
  // change to detect different times of day, for example (hours == 9 && minutes == 0 && seconds == 0) would detect 9:00:00 AM
  // use additional "else if" conditions to detect more than one time

  if(seconds == 0){               // check for seconds = 0 (one minute intervals)
    newAngle = angle + angleIncrement; // increase angle by increment
    if (newAngle<=180){           // if the new angle is less than or equal to 180, increase angle
    	while(angle < newAngle){    // increase angle until it reaches the new angle
      	angle = angle + 1;        // increase angle by 1
      	servo.write(angle);       // move the servo
        Serial.println(angle);    // print the angle
      	delay(movementDelay);     // delay to slow down movement
    	}
      // flash LED and buzzer
      flashLED(4,150);    // flashLED(number of flashes, delay in milliseconds), see function below
    }
    else{ // if the new angle is greater than 180, reset angle to 0
      while(angle>0){         // decrease angle until it reaches 0
        angle = angle - 1;    // decrease angle by 1
        servo.write(angle);   // move the servo
        Serial.println(angle);// print the angle
        delay(movementDelay); // delay to slow down movement
      }
    }
  }
}

void flashLED(int numFlashes, int flashDelay){  // alarm function to flash LED and sound buzzer
  lcd.clear();              // clear the LCD screen
  lcd.setCursor(0, 0);      // set cursor to top left
  lcd.print("Take medicine!");  // display message
  for (int i = 0; i<numFlashes; i++){  // loop to flash LED/buzzer numFlashes times
    digitalWrite(ledPin,HIGH);         // turn LED on
    digitalWrite(buzzerPin,HIGH);      // turn buzzer on
    delay(flashDelay);                 // delay
    digitalWrite(ledPin,LOW);          // turn LED off
    digitalWrite(buzzerPin,LOW);       // turn buzzer off
    delay(flashDelay);                 // delay
  }
  // wait for button press - the code will get stuck in this loop until you press the button
  while(digitalRead(buttonPin) == LOW){}; 
  delay(1000);    // delay before clearing screen
  lcd.clear();    // clear screen
}

void updateLCD(){    // function to update LCD screen
  // get current time from the RTC
  RTCTime currentTime;
  RTC.getTime(currentTime);
  // store current time variables
  year = currentTime.getYear();
  month = Month2int(currentTime.getMonth());
  day = currentTime.getDayOfMonth();
  hour = currentTime.getHour();
  minutes = currentTime.getMinutes();
  seconds = currentTime.getSeconds();

  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 0);
  // print month/day/year (rearrange for different date formats)
  // add leading spaces if month or day are less than 10 to keep spacing consistent
  // (always use 2 characters for month and day)
  if(month<10){   // print extra space if month is less than 10
    lcd.print(" ");
  }
  lcd.print(month);  // print the month
  lcd.print("/");
  if(day<10){        // print extra space if the day is less than 10
    lcd.print(" ");
  }
  lcd.print(day);    // print the day
  lcd.print("/");
  lcd.print(year);   // print the year

  // print time in hh:mm:ss format
  // add leading zeroes if minutes or seconds are less than ten to keep spacing consistent
  lcd.setCursor(0, 1);  // move cursor to second row
  if(hour<10){          // print leading space if hour is less than 10
    lcd.print(" ");
  }
  lcd.print(hour);      // print the hour
  lcd.print(":");
  if(minutes<10){       // print leading zero if minute is less than 10
    lcd.print("0");
  }
  lcd.print(minutes);   // print the minute
  lcd.print(":");
  if(seconds<10){       // print leading zero if second is less than 10
    lcd.print("0");
  }
  lcd.print(seconds);   // print the second
}
  
  
      
  
