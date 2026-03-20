#include "LCD_cm1441.h"

const uint8_t LCD_cm1441::kNumTable[10] = { //7セグの0-9 (7ビット目0x80は2-12桁ドット、1,13桁は左右12桁目コンマ)
  0x77, 0x12, 0x6b, 0x5b, 0x1e, 0x5d, 0x7d, 0x13, 0x7f, 0x5f
};

const uint16_t LCD_cm1441::kNum16Table[10] = { //16セグの0-9(未使用)
  0xe997, 0x0014, 0xd18d, 0x919d, 0x181c, 0x9999, 0xd999, 0x0115, 0xd99d, 0x999d
};

const uint8_t LCD_cm1441::kSegmentAddresses[26] = { //7セグの各先頭アドレス(13桁x2行)
  0x50, 0x52, 0x54, 0x56, 0x58, 0x5a, 0x5c, 0x5e, 0x60, 0x62, 0x64, 0x66, 0x68,
  0x44, 0x46, 0x48, 0x4a, 0x4c, 0x4e, 0x6c, 0x6e, 0x70, 0x72, 0x74, 0x76, 0x78
};

const uint8_t LCD_cm1441::kSegment16Addresses[16] = { //16セグの各先頭アドレス(16桁)
  0x3c, 0x38, 0x34, 0x30, 0x2c, 0x28, 0x24, 0x20, 0x1c, 0x18, 0x14, 0x10, 0x0c, 0x08, 0x04, 0x00
};

const uint16_t LCD_cm1441::kNum16ASCIITable[128] = { //16セグ用のASCIIテキスト表示テーブル
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
0x0000, 0x0280, 0x0804, 0xA5A3, 0x9BD9, 0x3BDA, 0xDBF0, 0x0200, 0x1022, 0x2408, 0x342A, 0x1248, 0x0090, 0x1008, 0x0080, 0x2002, 
0xE997, 0x0014, 0xD18D, 0x919D, 0x181C, 0x9999, 0xD999, 0x0115, 0xD99D, 0x999D, 0x0081, 0x0091, 0x0022, 0x9088, 0x2400, 0x018D, 
0xC9DD, 0x591D, 0x83DD, 0xC981, 0x83D5, 0xD989, 0x5909, 0xC999, 0x581C, 0x83C1, 0xC094, 0x5822, 0xC880, 0x4C56, 0x4C34, 0xC995, 
0x590D, 0xC9B5, 0x592D, 0x99A1, 0x0341, 0xC894, 0x6802, 0x6A34, 0x2422, 0x184C, 0xA183, 0x02C1, 0x0420, 0x8340, 0x0905, 0x8080, 
0x0400, 0x591D, 0x83DD, 0xC981, 0x83D5, 0xD989, 0x5909, 0xC999, 0x581C, 0x83C1, 0xC094, 0x5822, 0xC880, 0x4C56, 0x4C34, 0xC995, 
0x590D, 0xC9B5, 0x592D, 0x99A1, 0x0341, 0xC894, 0x6802, 0x6A34, 0x2422, 0x184C, 0xA183, 0x12C1, 0x0240, 0x8348, 0x0402, 0xFFFF
};

const uint16_t LCD_cm1441::kSegmentCommaAddresses[22] { //コンマの位置(2桁目-12桁目までの11桁 各2行分)
  0x5108, 0x7f08, 0x6a01, 0x6a02, 0x6a04, 0x6a08, 0x6b01, 0x6b02, 0x6b04, 0x6b08, 0x6908,
  0x4508, 0x7d08, 0x7a01, 0x7a02, 0x7a04, 0x7a08, 0x7b01, 0x7b02, 0x7b04, 0x7b08, 0x7908
};


LCD_cm1441::LCD_cm1441(uint8_t csPin, uint8_t wrPin, uint8_t dataPin)
  : csPin_(csPin), wrPin_(wrPin), dataPin_(dataPin) {
  for (int i = 0; i < 128; i++) {
    displayRAM_[i] = 0x00;
    currentHardwareRAM_[i] = 0xFF;
  }
}

void LCD_cm1441::begin() {
  pinMode(csPin_, OUTPUT);
  pinMode(wrPin_, OUTPUT);
  pinMode(dataPin_, OUTPUT);
  digitalWrite(csPin_, HIGH);
  digitalWrite(wrPin_, HIGH);

  sendCommand(0x01);  // SYS EN
  sendCommand(0x03);  // LCD ON

  syncAll();
}

void LCD_cm1441::setBuffer(uint8_t address, uint8_t data4bit, uint8_t mask) {
  if (address > 0x7F) return;
  uint8_t safeMask = mask & 0x0F;
  uint8_t data = data4bit & 0x0F;
  displayRAM_[address] = (displayRAM_[address] & ~safeMask) | (data & safeMask);
}

void LCD_cm1441::flushDisplay() {
  for (uint8_t i = 0; i <= 0x7F; i++) {
    if (displayRAM_[i] != currentHardwareRAM_[i]) {
      writeSingle(i, displayRAM_[i]);
      currentHardwareRAM_[i] = displayRAM_[i];
    }
  }
}

void LCD_cm1441::syncAll() {
  for (int i = 0; i < 128; i++) {
    writeSingle(i, displayRAM_[i]);
    currentHardwareRAM_[i] = displayRAM_[i];
  }
}

void LCD_cm1441::setNumber8bit(uint8_t startAddress, uint8_t pattern) {
  setBuffer(startAddress, pattern & 0x0F);
  setBuffer(startAddress + 1, (pattern >> 4) & 0x07, 0x07);
}

void LCD_cm1441::setNumber16bit(uint8_t startAddress, uint16_t pattern) {
  setBuffer(startAddress, pattern & 0x0F);
  setBuffer(startAddress + 1, (pattern >> 4) & 0x0F);
  setBuffer(startAddress + 2, (pattern >> 8) & 0x0F);
  setBuffer(startAddress + 3, (pattern >> 12) & 0x0F);
}

void LCD_cm1441::writeSingle(uint8_t address, uint8_t data) {
  digitalWrite(csPin_, LOW);
  sendBits(0b101, 3);
  sendBits(address, 7);
  for (int i = 0; i < 4; i++) {
    sendBit((data >> i) & 0x01);
  }
  digitalWrite(csPin_, HIGH);
}

void LCD_cm1441::sendCommand(uint8_t cmd) {
  digitalWrite(csPin_, LOW);
  sendBits(0b100, 3);
  sendBits(cmd, 8);
  sendBit(0);
  digitalWrite(csPin_, HIGH);
}

void LCD_cm1441::sendBit(bool bit) {
  digitalWrite(wrPin_, LOW);
  digitalWrite(dataPin_, bit ? HIGH : LOW);
  digitalWrite(wrPin_, HIGH);
}

void LCD_cm1441::sendBits(uint32_t data, int count) {
  for (int i = count - 1; i >= 0; i--) {
    sendBit((data >> i) & 0x01);
  }
}


void LCD_cm1441::set7segValue(int row, int64_t value, bool allowOverflow) {
  if (row<0||row>1) return;
  if (!allowOverflow){
    if (value > 9999999999999) value = 9999999999999;
    if (value < -999999999999) value = -999999999999;
  }
  uint8_t status = (value < 0) << 4; // {0}{0}{IsMinused}{Minus}{4:Column}
  if (status & (1 << 4)) value *= -1;
  for (; (status & 0x0f) < 13; status++) {
    if (value != 0)
      setNumber8bit(LCD_cm1441::kSegmentAddresses[(12+row*13) - (status & 0x0f)], kNumTable[value % 10]);
    else {
      if ((status & 0x30) == 0x10){
        status |= 1<<5;
        setNumber8bit(LCD_cm1441::kSegmentAddresses[(12+row*13) - (status & 0x0f)], 0x08);
      } else {
        setNumber8bit(LCD_cm1441::kSegmentAddresses[(12+row*13) - (status & 0x0f)], 0);
      }
    }
    value /= 10;
  }

}

void LCD_cm1441::setVNLabels(uint16_t visible) {
  setBuffer(0x7c, visible & 0x00F);
  setBuffer(0x7d, (visible & 0x010) >> 4,0x01);
  setBuffer(0x7f, (visible & 0x020) >> 5,0x01);
  uint8_t high = (visible >> 6) & 0x0F; // V7-V0
  uint8_t highReversed = ((high & 0x01) << 3)
                       | ((high & 0x02) << 1)
                       | ((high & 0x04) >> 1)
                       | ((high & 0x08) >> 3);
  setBuffer(0x7e, highReversed);
}

void LCD_cm1441::set16segString(String str){
  uint8_t status = 0x00;
  for(; (status & 0x1f) < 16; status++){
    if ((status & 0x80)== 0x80) {
      setNumber16bit(kSegment16Addresses[(status&0x1f)],0);
      continue;
    }
    if(str.length()>=(status & 0x1f)){
      setNumber16bit(kSegment16Addresses[(status&0x1f)],kNum16ASCIITable[str.charAt(status & 0x1f)]);
    }else{
      status |= 0x80;
      setNumber16bit(kSegment16Addresses[(status&0x1f)],0);
    }
  }
  
  str.length();
}

/// @brief Battery UI
/// @param frame frame of battery UI
/// @param value cells of battery UI (0-3)
void LCD_cm1441::setBatteryUI(bool frame, uint8_t value){
  uint8_t visible= (frame<<0);
  if(value>=1) visible |= 1<<3;
  if(value>=2) visible |= 1<<2;
  if(value>=3) visible |= 1<<1;
  setBuffer(0x40, (visible & 0x7) << 1 , 0x0E);
  setBuffer(0x41, visible >> 3, 0x01);
  
}

	void LCD_cm1441::setSymbolAdjust0(bool value){
    setBuffer(0x42, value << 0, 1 << 0);
  }
	void LCD_cm1441::setSymbolNET(bool value){
    setBuffer(0x42, value << 1, 1 << 1);
  }
	void LCD_cm1441::setSymbolDoubleTriangle(bool value){
    setBuffer(0x42, value << 2, 1 << 2);
  }
	void LCD_cm1441::setSymbolCoinPer100g(bool value){
    setBuffer(0x42, value << 3, 1 << 3);
  }
	void LCD_cm1441::setSymbolMIN(bool value){
    setBuffer(0x43, value << 0, 1 << 0);
  }
	void LCD_cm1441::setSymbolLabel(bool value){
    setBuffer(0x43, value << 1, 1 << 1);
  }
	void LCD_cm1441::setSymbolE1(bool value){
    setBuffer(0x43, value << 2, 1 << 2);
  }
	void LCD_cm1441::setSymbolE2(bool value){
    setBuffer(0x43, value << 3, 1 << 3);
  }
	void LCD_cm1441::setSymbolWeight(bool E1, bool E2){
    setSymbolE1(E1);
    setSymbolE2(E2);
  }
	void LCD_cm1441::setSymbolFIX(bool value){
    setBuffer(0x41, value << 1, 1 << 1);
  }


	void LCD_cm1441::setComma(int row, uint16_t comma, uint16_t mask){
    if(row<0||row>1)return;
    for(int i=1;i<12;i++){
      if(((mask >> i) & 0x01) == 0) continue;
      uint16_t address = kSegmentCommaAddresses[row*11+10-(i-1)];
      setBuffer(address>>8, ((comma>>i)&0x01)?address&0xf:0,address&0xf);
    }
  }
	void LCD_cm1441::setDot(int row, uint16_t dot, uint16_t mask){
    if(row<0||row>1)return;
    for(int i=1;i<12;i++){
      if(((mask >> i) & 0x01) == 0) continue;
      uint8_t address = kSegmentAddresses[row*13+12-i];
      setBuffer(address+1, ((dot>>i)&0x01)?0x08:0,0x08);
    }
  }