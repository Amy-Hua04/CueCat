// #include <FreeMonoBoldOblique12pt7b.h>
// #include <FreeSevenSegNumFontPlusPlus.h>


// #include <TFT_PRINTGLUE.h>
// #include <UTFTGLUE.h>

#include <MCUFRIEND_kbv.h> // Include the MCUFRIEND_kbv library

//For screen
// #include <Adafruit_GFX.h>
// #include <Adafruit_ILI9341.h> // Include the Adafruit TFT library
// #include <Adafruit_TouchScreen.h> // Include the Adafruit TouchScreen library

const int CYCLE = -1;
const int HOME = 0;
const int SPOTIFY = 1;
const int STOPWATCH = 2;
const int NUM_STATES = 3;

const utin16_t bgColour = ((0xffe5 >> 3) << 11) | ((0xe7 >> 2) << 5) | (0xe7 >> 3);
const utin16_t BLACK = 0x0000;

int state = 0;
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

MCUFRIEND_kbv tft; // Create an instance of the TFT display

void setup() {
  button1.pin = 23;
  button2.pin = 25;

  tft.begin(); // Initialize TFT display
  tft.setRotation(3); // Set the screen rotation (0, 1, 2, or 3)

  tft.fillScreen(bgColour); // Fill the screen with a background color

  tft.setTextColor(BLACK); // Set text color
  tft.setTextSize(2); // Set text size 
  tft.setCursor(10, 10); // Set text cursor position
  tft.println("Hello!"); // Print text on the screen
  delay(4000);
  // You can also draw shapes and graphics here
}

void loop() {
  button1.currReading = digitalRead(button1.pin);
  button2.currReading = digitalRead(button2.pin);

  switch (state){
    case CYCLE:
      if (isPressed(button1)){
        state = tempState;
        if (state == STOPWATCH){
          elapsedTime = 0;
        }
      }
      else if (isPressed(button2)){
        tempState = (tempState + 1) % NUM_STATES;
      }
      break;

    case HOME:
      if (isPressed(button1)){
        state = CYCLE;
      }
      
      break;

    case SPOTIFY:
      if (isPressed(button1)){
        state = CYCLE;
        break;
      }

      //tft.drawBMP(coverImg, 10, 30) //insert cover image later
      //insert song and artist info


      break;

    case STOPWATCH:
      if (isPressed(button1)){
        if (elapsedTime == 0){
          state = CYCLE;
          break;
        }
        else{
          elapsedTime  = 0;
          paused = true;
        }
      }
      else if (isPressed(button2)){
        paused = !paused;
        if (!paused) {
          startTime = millis(); // Record the start time when unpausing
        }
      }
      if (!paused){
        increment_timer();
      }
      tft.fillScreen(bgColour);
      tft.println(elapsedTime);

      break;
  }

  button1.prevReading = button1.currReading;
  button2.prevReading = button2.currReading;
  
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
      if (button.currState == LOW) {
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