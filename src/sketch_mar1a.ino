#include <Arduino.h>
#include "LCD_cm1441.h"

// ピン設定
const int CS_PIN = 2;
const int WR_PIN = 3;
const int DATA_PIN = 7;

LCD_cm1441 lcd(CS_PIN, WR_PIN, DATA_PIN);
void setup() {
  lcd.begin();

  for (int i = 0; i < 128; i++)
    lcd.setBuffer(i, 0x0f);
  lcd.flushDisplay();
  delay(1000);
  for (int i = 0; i < 128; i++)
    lcd.setBuffer(i, 0x00);
  lcd.flushDisplay();
}

// --- 高速検証用ループ ---
unsigned long long counter = 0;
unsigned long lastUpdate = 0;
String sampleText = "HELLO Misaki-chan.net !\"#$%&'()*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_{|}~\x7f";
const uint8_t kDisplayWidth = 16;
const unsigned long kScrollIntervalMs = 500;
unsigned long lastScroll = 0;
uint16_t scrollIndex = 0;

String getScrollWindow(const String& text, uint8_t width, uint16_t index) {
  String padded = text + "                "; // 16個の空白
  if (padded.length() < width) {
    padded += String(' ', width - padded.length());
  }
  uint16_t len = padded.length();
  uint16_t start = index % len;
  String window = "";
  for (uint8_t i = 0; i < width; i++) {
    window += padded.charAt((start + i) % len);
  }
  return window;
}
void loop() {

  unsigned long long temp = counter;
  int symbol = millis()/1000%9;
  unsigned long now = millis();
  if (now - lastScroll >= kScrollIntervalMs) {
    scrollIndex++;
    lastScroll = now;
  }

  
  uint32_t sec = millis()/1000;
  uint32_t sec1 = sec-1;
  lcd.set7segValue(0, micros()); //上段
  lcd.set7segValue(1, millis()); //下段
  //前の桁をマスクしつつ消去
  lcd.setDot(sec1/13%2,0,1<<(sec%13));
  lcd.setComma(sec1/13%4,0,1<<(sec%13));
  //ドットとコンマを作成
  lcd.setDot(sec/13%2,1<<(sec%13),1<<(sec%13));
  lcd.setComma(sec/13%4,1<<(sec%13),1<<(sec%13)); //周期の奇数毎に表示

  lcd.setVNLabels(1<<(millis()/1000%10)); //V0-V9
  lcd.setBatteryUI(millis()/500%2,millis()/1000%4); //枠を0.5秒毎に点滅、0-3個セル点灯
  lcd.setSymbolAdjust0(symbol==0); //→0←
  lcd.setSymbolNET(symbol==1);  //NET
  lcd.setSymbolDoubleTriangle(symbol==2); //◣ ◢
  lcd.setSymbolCoinPer100g(symbol==3); //㊿⑩①/100g
  lcd.setSymbolMIN(symbol==4);  //MIN
  lcd.setSymbolLabel(symbol==5);  //LABEL
  lcd.setSymbolE1(symbol==6); //[e1]
  lcd.setSymbolE2(symbol==7); //[e2]
  lcd.setSymbolFIX(symbol==8);  //FIX
  lcd.set16segString(getScrollWindow(sampleText, 16, scrollIndex));//16桁16セグ



  long a = micros();
  if (a - lastUpdate >= 10000) {
    counter += a - lastUpdate*10;
    lastUpdate = a;
    lcd.flushDisplay();
  }
}