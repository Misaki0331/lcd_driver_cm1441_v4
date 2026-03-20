# LCD_cm1441 Arduino Library

CM1441_V4の7セグメント、16セグメント、および各種シンボルすべての表示に対応しています。

## 特徴
- 7セグメントディスプレイでの数値表示
- 16セグメントのテキスト表示
- ドット、コンマの制御
- バッテリー残量表示
- 特殊シンボル表示機能
- Vラベル（V0～V9）の表示

## 環境要件
- **Board**: megaatmega2560 (megaatmega1280搭載ボード)
- **Framework**: arduino

## ピン配置
デフォルトのサンプルプログラム (sketch_mar1a.ino) での接続例は以下の通りです：

| Arduino 側ピン | LCD (cm1441) ピン|
| - | - |
| D2 | CS_PIN (チップセレクト) |
| D3 | WR_PIN (ライト/クロック) |
| D7 | DATA_PIN (データ) |

裏面
| 8: LED(K) | 16: LED(K)
| 7: LED(A) | 15: LED(A)
| 6: GND | 14: GND
| 5: Unknown | 13: VCC
| 4: CS_PIN | 12: NC
| 3: WR_PIN | 11: NC
| 2: NC | 10: NC
| 1: DATA_PIN | 9: NC

## 使用方法

### 初期化
`cpp
#include "LCD_cm1441.h"

const int CS_PIN = 2;
const int WR_PIN = 3;
const int DATA_PIN = 7;

LCD_cm1441 lcd(CS_PIN, WR_PIN, DATA_PIN);

void setup() {
  lcd.begin();
}
`

### 描画の更新
表示内容を更新した後は、必ず lcd.flushDisplay() を呼び出してLCDにデータを転送してください。

`cpp
void loop() {
  // 上段下段の7セグ表示器に数値をセット
  lcd.set7segValue(0, 1234567890123); 
  lcd.set7segValue(1, -987654321098); 

  // 16セグメントへの文字列セット
  lcd.set16segString("HELLO CM1441_V4!");

  // ディスプレイへ反映
  lcd.flushDisplay();
}
`

## 主なAPI

### セグメントテキスト表示
- void set7segValue(int row, int64_t value, bool allowOverflow=false);
- void set16segString(String str);
- void setComma(int row, uint16_t comma, uint16_t mask);
- void setDot(int row, uint16_t dot, uint16_t mask);

### UIシンボル表示
- void setBatteryUI(bool frame, uint8_t value); - バッテリー残量 (0-3) と枠の表示
- void setVNLabels(uint16_t visible); - V0-V9までの印字をビット単位で有効化
- void setSymbolAdjust0(bool value); - 0
- void setSymbolNET(bool value); - NET
- void setSymbolMIN(bool value); - MIN
- void setSymbolLabel(bool value); - LABEL
- void setSymbolFIX(bool value); - FIX
- void setSymbolCoinPer100g(bool value); - ㊿/100g

## ライセンス

Mit Licenseの元で自由に改変、商用問わず利用可能です。