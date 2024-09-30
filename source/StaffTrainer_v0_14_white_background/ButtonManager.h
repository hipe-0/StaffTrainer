// ButtonManager.h
// by Hippolyte Mounier
// 2024-05-20
//
// Goal: manages a piano keyboard with 12 keys


#include <ezButton.h>

#define BUTTON_DEBOUNCE_MILISECONDS 15 // 15 for snappy button reaction time, 50 is regarded as sluggish

const int BUTTON_NUM = 12;

const int BUTTON_1_PIN = 38;
const int BUTTON_2_PIN = 37;
const int BUTTON_3_PIN = 36;
const int BUTTON_4_PIN = 35;
const int BUTTON_5_PIN = 34;
const int BUTTON_6_PIN = 33;
const int BUTTON_7_PIN = 32;
const int BUTTON_8_PIN = 31;
const int BUTTON_9_PIN = 30;
const int BUTTON_10_PIN = 29;
const int BUTTON_11_PIN = 28;
const int BUTTON_12_PIN = 27;

ezButton buttonArray[] = {
  ezButton(BUTTON_1_PIN),
  ezButton(BUTTON_2_PIN),
  ezButton(BUTTON_3_PIN),
  ezButton(BUTTON_4_PIN),
  ezButton(BUTTON_5_PIN),
  ezButton(BUTTON_6_PIN),
  ezButton(BUTTON_7_PIN),
  ezButton(BUTTON_8_PIN),
  ezButton(BUTTON_9_PIN),
  ezButton(BUTTON_10_PIN),
  ezButton(BUTTON_11_PIN),
  ezButton(BUTTON_12_PIN)
};

class ButtonManager {
private:

public:

  ButtonManager() {
    for (byte i = 0; i < BUTTON_NUM; i++)
      buttonArray[i].setDebounceTime(BUTTON_DEBOUNCE_MILISECONDS); 
  }


  bool isPressed(int b) {
    return buttonArray[b].isPressed();
  }

  void loop() {
    for (byte i = 0; i < BUTTON_NUM; i++)
      buttonArray[i].loop();
  }
};