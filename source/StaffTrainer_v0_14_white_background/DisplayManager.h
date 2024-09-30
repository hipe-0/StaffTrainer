// DisplayManager.h
// by Hippolyte Mounier
// 2024-05-20
//
// Goal: Manages the display of the Trainer device
// Platform: Teensy 4.1
// Display: circular 240*240 pixel 4-wire SPI TFT CG9A010 driver
//
// SPI protocol pin wiring is as follows
//     GC9A010 -------- Teensy 4.1
//       RST ------------ Not connected
//       CS ------------- 10 // defined below, chip select pin
//       DC ------------- 9  // defined below, Data or Command is sent pin
//       SDA ------------ 11 // predefined SPI Data pin on Teensy
//       SCL ------------ 13 // predefined SPI Clock pin on Teensy
//       GND ------------ GND
//       VCC -------------3V3
//
// SPI pins for the TFT screen
#define TFT_DC 9
#define TFT_CS 10

#include "SPI.h"
#include "Adafruit_GFX.h"
#include "Adafruit_GC9A01A.h"
#include "Bitmaps.h"
#include "iferror.h"

#define COLOR_BACKGROUND GC9A01A_WHITE
#define COLOR_FOREGROUND GC9A01A_BLACK

class DisplayManager {
private:
  Adafruit_GC9A01A tft_;
  int lastNoteOnStaff_;


public:
  DisplayManager()
    : tft_(TFT_CS, TFT_DC) {
    lastNoteOnStaff_ = 0;
  }

  void init() {
    tft_.begin();
    tft_.setRotation(1);
    clean();
  }

  void clean() {
    tft_.fillScreen(COLOR_BACKGROUND);
  }

  void writeText(char* t) {
    tft_.setCursor(100, 100);
    tft_.setTextColor(GC9A01A_CYAN);
    tft_.setTextSize(3);
    tft_.println(t);
  }

  void writeTextLine2(char* t) {
    tft_.setCursor(100, 130);
    tft_.setTextColor(COLOR_FOREGROUND);
    tft_.setTextSize(2);
    tft_.println(t);
  }


#define BAR_X 28
#define BAR_Y 160
#define BAR_W 170
#define BAR_H 10

  void displayProgressbar(int x, int max) {
    tft_.drawRect(28, BAR_Y, BAR_W + 4, BAR_H + 5, COLOR_FOREGROUND);

    for (int i = 0; i < x; i++)
      tft_.fillRect(30 + (i * BAR_W) / max, BAR_Y + 2, BAR_W / max - 2, BAR_H, GC9A01A_GREEN);
  }

  void displayProgressbarVertical(int x, int max) {
    tft_.drawRect(BAR_Y, 28, BAR_H + 5, BAR_W + 4, COLOR_FOREGROUND);

    for (int i = 0; i < x; i++)
      tft_.fillRect(BAR_Y + 2, 30 + (i * BAR_W) / max, BAR_H, BAR_W / max - 2, GC9A01A_GREEN);
  }

  void displayMenu() {
    clean();
    tft_.setCursor(40, 30);
    tft_.setTextColor(COLOR_FOREGROUND);
    tft_.setTextSize(2);
    tft_.setTextColor(GC9A01A_DARKGREEN);
    tft_.print(APP_NAME);

    tft_.setTextSize(1);
    tft_.setCursor(40, 50);
    tft_.setTextColor(COLOR_FOREGROUND);
    tft_.print("by Hippo");
    tft_.setCursor(160, 50);
    tft_.print(APP_VERSION);

    // Menu display
    tft_.setTextSize(2);
    tft_.setCursor(30, 70);
    tft_.setTextColor(COLOR_FOREGROUND);
    tft_.println("Training Menu: ");
    tft_.setTextColor(GC9A01A_BLUE);
    tft_.println("  C: French notation");
    tft_.setTextColor(GC9A01A_RED);
    tft_.println("  D: English notat.");
    tft_.setTextColor(GC9A01A_DARKGREEN);
    tft_.println("  E: Staff notation");
    tft_.setTextColor(GC9A01A_MAGENTA);
    tft_.println("  F: Staff +1 oct");
    tft_.setTextColor(GC9A01A_DARKGREY);
    tft_.println("  G: Staff -1 oct");
    tft_.setTextColor(GC9A01A_ORANGE);
    tft_.println("  A: Staff -2 oct");
  }

  void displayCongratulation(float lastTimeS, char* exerciseTxt) {
    clean();
    tft_.setCursor(30, 60);
    tft_.setTextColor(GC9A01A_DARKGREEN);
    tft_.setTextSize(2);
    tft_.println("Congratulations");

    int nbStars = 0;
    if (lastTimeS < 7.6) nbStars = 1;
    if (lastTimeS < 5.1) nbStars = 2;
    if (lastTimeS < 2.3) nbStars = 3;
    if (lastTimeS < 1.5) nbStars = 4;
    if (lastTimeS < 1.0) nbStars = 5;
    if (lastTimeS < 0.7) nbStars = 6;
    if (lastTimeS < 0.5) nbStars = 7;  // follows 1.5 ^ i sequence

    if (nbStars > 0)
      for (int i = 0; i < nbStars; i++)
        tft_.drawBitmap(115 + i * 35 - nbStars * 15, 105, BitmapStar, 28, 28, GC9A01A_ORANGE);

    tft_.setTextColor(COLOR_FOREGROUND);
    tft_.setCursor(30, 150);
    tft_.println(exerciseTxt);

    char phrase3[30] = "";
    if (lastTimeS >= 0.0) {
      sprintf(phrase3, "Avg. %.1f s/key", lastTimeS);
      tft_.setCursor(30, 170);
      tft_.println(phrase3);
    }
  }

// in pixels
#define STAFF_X 30
#define STAFF_Y 30
#define STAFF_W 120
#define STAFF_H 80  // distance between 5 bars
#define NOTE_RADIUS 7
#define NB_LINES 12
#define CLEF_X 30

  void displayStaff() {
    tft_.drawBitmap(CLEF_X + 10, STAFF_Y - 20, BitmapTrebbleClef, 40, 114, COLOR_FOREGROUND);
    tft_.drawBitmap(CLEF_X, STAFF_Y + 95, BitmapBassClef, 52, 57, COLOR_FOREGROUND);

    for (int i = -1; i < NB_LINES; i++)
      if (i != 5 && i != -1 && i != 11)
        tft_.drawLine(STAFF_X, STAFF_Y + (i * STAFF_H) / 5, STAFF_X + STAFF_W, 30 + (i * STAFF_H) / 5, COLOR_FOREGROUND);
      else
        tft_.drawLine(90, STAFF_Y + (i * STAFF_H) / 5, 120, 30 + (i * STAFF_H) / 5, COLOR_FOREGROUND);
  }

  void displayNoteOnStaff(int notePos, char* symbol) {
    int x = 100;
    int y = STAFF_Y + STAFF_H - (notePos * STAFF_H) / 10;

    tft_.drawBitmap(x - 5, y - 6, BitmapWholeNote, 19, 14, GC9A01A_BLUE);
    tft_.setTextColor(GC9A01A_BLUE);
    tft_.setTextSize(2);
    tft_.setCursor(x + 20, y - 8);
    tft_.print(symbol);
    lastNoteOnStaff_ = notePos;
  }

  void eraseNoteOnStaff() {
    int x = 100;
    int y = STAFF_Y + STAFF_H - (lastNoteOnStaff_ * STAFF_H) / 10;

    tft_.fillRect(x - 5, y - 6, 40, 14, COLOR_BACKGROUND);
    tft_.fillRect(100, 130, 110, 14, COLOR_BACKGROUND);
  }
};
