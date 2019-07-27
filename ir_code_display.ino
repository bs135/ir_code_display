//for LED 7-seg
#include "TM1637.h"

//for IR
#include <IRremote.h>

//for OLED
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define ON 1
#define OFF 0

#define CLK 3
#define DIO 2
TM1637 led7(CLK,DIO);

#define RECV_PIN 5
IRrecv irrecv(RECV_PIN);
decode_results results;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
boolean OLED_EN = true;

union IRData {
  unsigned long l;
  uint8_t b[4];
} irdata;

void setup()
{
  Serial.begin(9600);
  Serial.println(F("Init Led 7-seg"));
  led7.set();
  led7.init();
  led7.clearDisplay();
  
  irrecv.enableIRIn();
  Serial.println(F("Init IR"));

  Serial.println("Init OLED");
  if(!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3D for 128x64
    Serial.println(F("OLED allocation failed"));
    OLED_EN=false;
  }

  if(OLED_EN){
    oled.clearDisplay();
    oled.setTextSize(1);     
    oled.setTextColor(WHITE); 
    oled.setCursor(0, 0);     
    oled.println(F("Welcom to NPNLab"));
    oled.display();
    delay(100);
  }
}

uint8_t reverse(uint8_t b) {
   b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
   b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
   b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
   return b;
}

void loop()
{
  if (irrecv.decode(&results)) {
    Serial.println(results.value, HEX);
    irdata.l = results.value;
    uint8_t ir = reverse(irdata.b[1]);
    char s[10];
    Serial.println(ir, HEX);
    if(ir != 0xff){
      led7.display(2, (ir>>4)&0x0f);
      led7.display(3, ir&0x0f);

      if(OLED_EN){
        oled.clearDisplay();
        
        oled.setCursor(0, 0);
        oled.println(F("IR DATA"));
        
        oled.setCursor(0, 10);
        oled.print(F("raw code: 0x"));
        sprintf(s, "%02X%02X%02X%02X", irdata.b[3], irdata.b[2], irdata.b[1], irdata.b[0]);
        oled.println(s);
        
        oled.setCursor(0, 20);
        oled.print(F("rev code: 0x"));
        sprintf(s, "%02X%02X%02X%02X", reverse(irdata.b[3]), reverse(irdata.b[2]), reverse(irdata.b[1]), reverse(irdata.b[0]));
        oled.println(s);
  
        oled.setCursor(0, 30);
        oled.print(F("IR code : 0x"));
        sprintf(s, "%02X (%d)", reverse(irdata.b[1]), reverse(irdata.b[1]));
        oled.println(s);
  
        oled.setCursor(0, 40);
        oled.print(F("bits    : "));
        oled.println(results.bits);
        
        oled.setCursor(0, 50);
        oled.print(F("raw len : "));
        oled.println(results.rawlen);
        
        oled.display();
      }
    }
    irrecv.resume(); // Receive the next value
  }
  delay(100);
}
