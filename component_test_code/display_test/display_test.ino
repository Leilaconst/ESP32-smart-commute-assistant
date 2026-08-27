#include <SPI.h>
#include <TFT_eSPI.h>
#include <string>

TFT_eSPI tft = TFT_eSPI();

int text_start_y = 20;
int text_start_x = 20;

int base_txt_mul = 8;

void setup() {
  Serial.begin(115200);
  tft.init();
  tft.setRotation(3);
  tft.fillScreen(TFT_WHITE);
  tft.setTextColor(0xFC9F);
  tft.setTextSize(2);
  tft.drawString("Screen Display Test", text_start_y, text_start_x);
  
  drawTest();

}

void drawTest() {
  
  int padding = 7;
  int lines = 7;
  int start_txt_size = 2;
  int prev_txt_size=2;

  for(int size = 1; size <= 7; size++){
    //set txt size
    tft.setTextSize(size);

    //Move down by padding and prev txt size
    text_start_x = text_start_x;
    text_start_y = text_start_y + padding + (prev_txt_size * base_txt_mul);

    //draw
    std::string txt = "Text Size " + std::to_string(size);
    
    tft.drawString(txt.c_str(), text_start_x, text_start_y);

    prev_txt_size = size;
  }
}

void loop() {
  
  
}
