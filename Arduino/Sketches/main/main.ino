#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;

#define BLACK   0x0000
#define PINK    0xf7dedc
#define LIGHT_BLUE 0xdcf1f7
#define LIGHT_GREEN 0xe1f5d3
#define BLUE    0x3e499e
#define RED     0xc44537
#define GREEN   0x49a83e
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define GRAY    0x8410
#define PURPLE  0xecb7ce

const int CYCLE = -1;
const int HOME = 0;
const int SPOTIFY = 1;
const int STOPWATCH = 2;

const int NUM_STATES = 3;

uint16_t version = MCUFRIEND_KBV_H_;

// const int button1 = 23;  // the number of the pushbutton pin
// const int button2 = 25;

// // variables will change:
// int button1State = 0;
// int button2State = 0;
// int button1PrevState = 0;
// int button2PrevState = 0;

int state = STOPWATCH;
int tempState = 0;

unsigned long startTime = 0; // Variable to store the start time
unsigned long elapsedTime = 0; // Variable to store the elapsed time
bool paused = true;

struct Button {
  int pin;
  int currReading = HIGH;
  int prevReading = HIGH;
  int currState = HIGH;
  unsigned long lastDebounceTime = 0;
};

Button button1;
Button button2;

const unsigned long debounceDelay = 40;    // Debounce delay in milliseconds


uint16_t colormask[] = {BLUE, GREEN, RED};

String x;

void setup() { 
  button1.pin = 23;
  button2.pin = 25;

	Serial.begin(115200); 
	Serial.setTimeout(1); 
  pinMode(button1.pin, INPUT);
  pinMode(button2.pin, INPUT);
  uint16_t ID = tft.readID();
  tft.begin(ID);
  tft.fillScreen(BLACK);
  tft.setRotation(3);

  tft.setTextColor(WHITE); // Set text color
  tft.setTextSize(4); // Set text size 
  tft.setCursor(10, 10); // Set text cursor position
  char intro[] = {"Hello!\n I'm CueCat"};
  int txtlen = sizeof(intro) -1;
  // tft.println("Hello!\n I'm CueCat"); // Print text on the screen
  for (int letter = 0; letter < txtlen; letter++)
  {
    tft.print(intro[letter]); //print each letter
    delay(100); //delay for illustration only
  }
  delay(2000);

  
  tft.fillScreen(colormask[tempState]);
  tft.fillRect(0, 0, 200, 50, BLACK); 
} 
void loop() { 
  button1.prevReading = button1.currReading;
  button2.prevReading = button2.currReading;
  button1.currReading = digitalRead(button1.pin);
  button2.currReading = digitalRead(button2.pin);

  int width = tft.width();
  int height = tft.height();

  tft.drawCircle(20, 20, 20, GREEN);
  tft.drawCircle(70, 20, 20, BLUE);
  tft.drawCircle(120, 20, 20, WHITE);
  tft.drawCircle(170, 20, 20, RED);

  tft.fillCircle(20, 20, 20, GREEN);

  switch (state) {
    case CYCLE:
      if (isPressed(button1)){
        state = tempState;
        tft.fillScreen(colormask[state]);
        tft.fillRect(0, 0, 200, 50, BLACK);
        if (state == STOPWATCH){
          elapsedTime = 0;
        }
        else if (state == SPOTIFY){
            Serial.println(1);
        }
      }
      else if (isPressed(button2)){
        delay(300);
        tempState = (tempState + 1) % NUM_STATES;
        tft.fillScreen(colormask[tempState]);
        tft.fillRect(0, 0, 200, 50, BLACK);
        tft.fillRect(tft.width()-60, 0, 60, tft.height(), GRAY);
        tft.fillTriangle(tft.width()-40, tft.height()/2-20, tft.width()-40, tft.height()/2+20, tft.width()-20, tft.height()/2, BLACK);
      }
      break;

    case HOME:
      if (isPressed(button1)) {
        cycle();
        break;
      }
      break;

    case SPOTIFY:
      if (isPressed(button2)) {
        cycle();
        break;
      }

      tft.drawRect(50,100,220,270,PURPLE);
      tft.drawRect(53,103,217,267,WHITE);
      tft.setCursor(90,160);
      tft.setTextColor(PURPLE);
      tft.print(">:D");

      tft.setCursor(53,53);
      tft.setTextSize(15);
      tft.setTextColor(0x401d2b);
      tft.print("Now Playing");
      tft.setCursor(50,50);
      tft.setTextColor(PURPLE);
      tft.print("Now Playing");

      x = Serial.readString(); 
      tft.setCursor(250, 110);
      tft.setTextSize(10);
      tft.setTextColor(BLACK); 
      tft.print(x);

      //tft.drawBMP(coverImg, 10, 30) //insert cover image later
      //insert song and artist info 

      break;

    case STOPWATCH:
      int b = checkButton();
      if (isPressed(button1)){
          paused = true;
          cycle();
          break;

      } 
      else if (b == 1) {
        paused = !paused;
        if (!paused) {
          startTime = millis(); // Record the start time when unpausing
        }
      }
      else if (b == 3) {
        elapsedTime = 0;
      }
      if (!paused) {
        increment_timer();
      }
      tft.setTextColor(WHITE); // Set text color
      tft.setTextSize(4); // Set text size 
      tft.setCursor(10, 100); // Set text cursor position
      tft.println(elapsedTime);

      break;
  }

// 	while (!Serial.available()); 
// 	x = Serial.readString(); 
// 	Serial.println();
//   tft.setCursor(0, 70);
//   tft.setTextSize(2);
//   tft.setTextColor(WHITE); 
//   tft.print(x);
} 

int debounce = 20;          // ms debounce period to prevent flickering when pressing or releasing the button
int DCgap = 250;            // max ms between clicks for a double click event
int holdTime = 1000;        // ms hold period: how long to wait for press+hold event

// Button variables
boolean buttonVal = HIGH;   // value read from button
boolean buttonLast = HIGH;  // buffered value of the button's previous state
boolean DCwaiting = false;  // whether we're waiting for a double click (down)
boolean DConUp = false;     // whether to register a double click on next release, or whether to wait and click
boolean singleOK = true;    // whether it's OK to do a single click
long downTime = -1;         // time the button was pressed down
long upTime = -1;           // time the button was released
boolean ignoreUp = false;   // whether to ignore the button release because the click+hold was triggered
boolean waitForUp = false;        // when held, whether to wait for the up event
boolean holdEventPast = false;    // whether or not the hold event happened already


int checkButton() {    
   int event = 0;
   buttonVal = digitalRead(button2.pin);
   // Button pressed down
   if (buttonVal == LOW && buttonLast == HIGH && (millis() - upTime) > debounce)
   {
       downTime = millis();
       ignoreUp = false;
       waitForUp = false;
       singleOK = true;
       holdEventPast = false;
       if ((millis()-upTime) < DCgap && DConUp == false && DCwaiting == true)  DConUp = true;
       else  DConUp = false;
       DCwaiting = false;
   }
   // Button released
   else if (buttonVal == HIGH && buttonLast == LOW && (millis() - downTime) > debounce)
   {        
       if (not ignoreUp)
       {
           upTime = millis();
           if (DConUp == false) DCwaiting = true;
           else
           {
               event = 2;
               DConUp = false;
               DCwaiting = false;
               singleOK = false;
           }
       }
   }
   // Test for normal click event: DCgap expired
   if ( buttonVal == HIGH && (millis()-upTime) >= DCgap && DCwaiting == true && DConUp == false && singleOK == true && event != 2)
   {
       event = 1;
       DCwaiting = false;
   }
   // Test for hold
   if (buttonVal == LOW && (millis() - downTime) >= holdTime) {
       // Trigger "normal" hold
       if (not holdEventPast)
       {
           event = 3;
           waitForUp = true;
           ignoreUp = true;
           DConUp = false;
           DCwaiting = false;
           //downTime = millis();
           holdEventPast = true;
       }
   }
   buttonLast = buttonVal;
   return event;
}

bool isPressed(Button& button) {
  if (button.currReading != button.prevReading) {
    button.lastDebounceTime = millis(); // Save the last time the button state changed
  }

  if ((millis() - button.lastDebounceTime) > debounceDelay) {
    // If the button state has been stable for a while, it's a valid press
    if (button.currState != button.currReading) {
      button.currState = button.currReading; // Save the current button state

      // Check if the button is pressed (assuming LOW when pressed)
      if (button.currState == HIGH) {
        return true;
      }
    }
  }
  return false;
}


void increment_timer() {
  if (!paused) {
    unsigned long currentTime = millis();
    elapsedTime += (currentTime - startTime);
    startTime = currentTime;
  }
}

void cycle(){
  state = CYCLE;
  tft.fillRect(tft.width()-60, 0, 60, tft.height(), GRAY); 
  tft.fillTriangle(tft.width()-40, tft.height()/2-20, tft.width()-40, tft.height()/2+20, tft.width()-20, tft.height()/2, BLACK);

}

