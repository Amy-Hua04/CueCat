#include <FreeDefaultFonts.h>
#include <FreeSevenSegNumFontPlusPlus.h>


#include <TFT_PRINTGLUE.h>
#include <UTFTGLUE.h>

#include <MCUFRIEND_kbv.h> // Include the MCUFRIEND_kbv library

// //For touch screen
// #include <Adafruit_GFX.h>
// #include <Adafruit_ILI9341.h> // Include the Adafruit TFT library
// #include <Adafruit_TouchScreen.h> // Include the Adafruit TouchScreen library

const int CYCLE = -1;
const int HOME = 0;
const int SPOTIFY = 1;
const int STOPWATCH = 2;
const int NUM_STATES = 3;

const utin16_t bg_colour = uint16_t myColor = ((0xffe5 >> 3) << 11) | ((0xe7 >> 2) << 5) | (0xe7 >> 3);

int state = 0;
int temp_state = 0;

double time = 0;
bool paused = true;

int reading;

const int button1 = 1; //Pin to button1

const int button2 = 2; // Pin to button2
int buttonState = HIGH;  // Current state of the button
int lastButtonState = HIGH; // Previous state of the button
unsigned long lastDebounceTime = 0;  // Last time the button state changed
unsigned long debounceDelay = 50;    // Debounce delay in milliseconds


MCUFRIEND_kbv tft; // Create an instance of the TFT display

void setup() {

  tft.begin(); // Initialize TFT display
  tft.setRotation(3); // Set the screen rotation (0, 1, 2, or 3)

  tft.fillScreen(bg_colour); // Fill the screen with a background color (optional)

  tft.setTextColor(ILI9486_WHITE); // Set text color (optional)
  tft.setTextSize(2); // Set text size 
  tft.setCursor(10, 10); // Set text cursor position (optional)
  tft.println("Hello!"); // Print text on the screen (optional)
  delay(4000);
  // You can also draw shapes and graphics here
}

void loop() {
  reading1 = digitalRead(button1);
  reading2 = digitalRead(button2);

  switch (state){
    case CYCLE:
      if (isPressed(button1)){
        state = temp_state;
        if (state == STOPWATCH){
          time = 0;
        }
      }
      elif (isPressed(button2)){
        temp_state = (temp_state + 1) % NUM_STATES;
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

      break;

    case STOPWATCH:
      if (isPressed(button1)){
        if (time == 0){
          state = CYCLE;
          break;
        }
        else{
          time = 0;
          paused = true;
        }
      }
      else if (isPressed(button2)){
        paused = !paused;
      }
      if (!paused){
        increment_timer();
      }
      tft.fillScreen(bg_colour);
      ftf.println(time)


      break;
  }

  prev_reading1 = reading1;
  prev_reading2 = reading2;
  
}



bool isPressed(int button) {
  int reading1 = digitalRead(button); // Read the button state


  if (reading != lastButtonState) {
    lastDebounceTime = millis(); // Save the last time the button state changed
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // If the button state has been stable for a while, it's a valid press
    if (reading != buttonState) {
      buttonState = reading; // Save the current button state

      // Check if the button is pressed (assuming LOW when pressed)
      if (buttonState == LOW) {
        // Perform your action here when the button is pressed
        return true;
      }
    }
  return false;
}

void increment_timer(){

}