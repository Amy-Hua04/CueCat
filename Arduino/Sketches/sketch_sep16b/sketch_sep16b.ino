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

uint16_t version = MCUFRIEND_KBV_H_;


int x; 

void setup() { 
	Serial.begin(115200); 
	Serial.setTimeout(1); 

  uint16_t ID = tft.readID();
  tft.begin(ID);
  tft.fillScreen(BLACK);
  tft.setRotation(3);

} 
void loop() { 
  int width = tft.width();
  int height = tft.height();

  tft.drawCircle(20, 20, 20, GREEN);
  tft.drawCircle(70, 20, 20, BLUE);
  tft.drawCircle(120, 20, 20, WHITE);
  tft.drawCircle(170, 20, 20, RED);


  tft.fillCircle(20, 20, 20, GREEN);


	while (!Serial.available()); 
	x = Serial.readString().toInt(); 
	Serial.print(x + 1);
  tft.setTextSize(2);
  tft.setTextColor(WHITE); 
  tft.print(x);
} 
