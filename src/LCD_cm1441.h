#pragma once

#include <Arduino.h>

class LCD_cm1441 {
 public:
	static const uint8_t kNumTable[10];
	static const uint16_t kNum16Table[10];
	static const uint8_t kSegmentAddresses[26];
	static const uint8_t kSegment16Addresses[16];
	static const uint16_t kNum16ASCIITable[128];
	static const uint16_t kSegmentCommaAddresses[22];
	
	LCD_cm1441(uint8_t csPin, uint8_t wrPin, uint8_t dataPin);

	void begin();
	void setBuffer(uint8_t address, uint8_t data4bit, uint8_t mask=0xFF);
	void flushDisplay();
	void syncAll();
	void setNumber8bit(uint8_t startAddress, uint8_t pattern);
	void setNumber16bit(uint8_t startAddress, uint16_t pattern);

	//User Friendly
	void set7segValue(int row, int64_t value, bool allowOverflow=false);
	void set16segString(String str);
	void setVNLabels(uint16_t visible); // V0-V9 Labels　
	void setBatteryUI(bool frame, uint8_t value);
	void setComma(int row, uint16_t comma, uint16_t mask=0xFFFF);
	void setDot(int row, uint16_t dot, uint16_t mask=0xFFFF);

	/* ToDo:
	void set7segChar(int row, uint8_t column, char txt, bool dot=false, bool comma=false);
	void set16segChar(uint8_t column, char txt);
	void set16segAnimation(uint8_t column, uint8_t type);
	*/

	void setSymbolAdjust0(bool value);
	void setSymbolNET(bool value);
	void setSymbolDoubleTriangle(bool value);
	void setSymbolCoinPer100g(bool value);
	void setSymbolMIN(bool value);
	void setSymbolLabel(bool value);
	void setSymbolE1(bool value);
	void setSymbolE2(bool value);
	void setSymbolWeight(bool E1, bool E2);
	void setSymbolFIX(bool value);

 private:
	void writeSingle(uint8_t address, uint8_t data);
	void sendCommand(uint8_t cmd);
	void sendBit(bool bit);
	void sendBits(uint32_t data, int count);

	uint8_t csPin_;
	uint8_t wrPin_;
	uint8_t dataPin_;
	uint8_t displayRAM_[128];
	uint8_t currentHardwareRAM_[128];
};