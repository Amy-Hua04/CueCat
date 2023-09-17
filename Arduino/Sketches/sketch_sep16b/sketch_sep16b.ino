#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv tft;

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF
#define GRAY    0x8410

const int CYCLE = -1;
const int HOME = 0;
const int SPOTIFY = 1;
const int STOPWATCH = 2;


const int NUM_STATES = 3;

uint16_t version = MCUFRIEND_KBV_H_;

const int button1 = 23;  // the number of the pushbutton pin
const int button2 = 25;

// variables will change:
int button1State = 0;
int button2State = 0;

int state = STOPWATCH;
int tempState = 0;

unsigned long startTime = 0; // Variable to store the start time
unsigned long elapsedTime = 0; // Variable to store the elapsed time
bool paused = true;

uint16_t colormask[] = {BLUE, GREEN, RED };

String x;

void setup() { 
	Serial.begin(115200); 
	Serial.setTimeout(1); 
  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
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

  tft.fillScreen(BLACK);
} 
void loop() { 

  button1State = digitalRead(button1);
  button2State = digitalRead(button2);

  int width = tft.width();
  int height = tft.height();

  tft.drawCircle(20, 20, 20, GREEN);
  tft.drawCircle(70, 20, 20, BLUE);
  tft.drawCircle(120, 20, 20, WHITE);
  tft.drawCircle(170, 20, 20, RED);

  tft.fillCircle(20, 20, 20, GREEN);


  switch (state) {
    case CYCLE:
      if (button1State == HIGH){
        state = tempState;
        tft.setTextSize(4);
        tft.println(state);
        delay(100);
        if (state == STOPWATCH){
          elapsedTime = 0;
        }
      }
      else if (button2State == HIGH){
        delay(300);
        tempState = (tempState + 1) % NUM_STATES;
        tft.fillScreen(colormask[tempState]);
        tft.fillRect(0, 0, 200, 50, BLACK);

      }
      break;

    case HOME:
      if (button1State == HIGH) {
        state = CYCLE;
        tft.fillScreen(BLACK);


      }

      break;

    case SPOTIFY:
      if (button1State == HIGH) {
        state = CYCLE;
        tft.fillScreen(BLACK);


        
      }

      //tft.drawBMP(coverImg, 10, 30) //insert cover image later
      //insert song and artist info

      break;

    case STOPWATCH:
      int b = checkButton();
      if (button1State == HIGH){
          paused = true;
          state = CYCLE;
          tft.fillScreen(BLACK);
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
      tft.setCursor(10, 10); // Set text cursor position
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
   buttonVal = digitalRead(button2);
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


void increment_timer() {
  if (!paused) {
    unsigned long currentTime = millis();
    elapsedTime += (currentTime - startTime);
    startTime = currentTime;
  }
}

