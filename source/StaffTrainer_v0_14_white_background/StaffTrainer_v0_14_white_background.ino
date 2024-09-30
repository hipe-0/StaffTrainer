// StaffTrainer
// by Hippolyte Mounier
// 2024-05-20

#define APP_NAME "Staff Trainer"
#define APP_VERSION "v0.14"

#include "ButtonManager.h"
ButtonManager myButtons;

#include "DisplayManager.h"
DisplayManager myScreen;

#include "Trainer.h"
Trainer myTrainer;

#include "MidiUsbHostHandler.h"
MidiUsbHostHandler myMidi;

#include "AudioBeepMic.h"
AudioBeepMic myBeep;

void setup() {
  Serial.begin(115200);
  Serial.println(APP_NAME);

  myScreen.init();
  myBeep.init();

  myBeep.playBeep(0, 100);
  delay(200);
  myBeep.playBeep(12, 100);
  myBeep.playBeep(0, 100);

  myMidi.init();

  Serial.println("--------End Setup---------");
}

int prevState = -1;  // forces initial display update

void loop() {
  myButtons.loop();
  myMidi.loop();

  int state = myTrainer.getCurrentState();
  if (state != prevState) 
    manageDisplay();

  prevState = state;

  manageState(state);
}

void manageState(int state) {
  switch (state) {
    case MENU:
      if (myButtons.isPressed(0) || myMidi.isKeyPressed(0)) {
        myTrainer.newExercise(FRENCH_NOTATION, 20, 0, 11);
        myBeep.playBeep(0, 100);
      }

      if (myButtons.isPressed(2) || myMidi.isKeyPressed(2)) {
        myTrainer.newExercise(ENGLISH_NOTATION, 20, 0, 11);
        myBeep.playBeep(0, 100);
      }

      if (myButtons.isPressed(4) || myMidi.isKeyPressed(4)) {
        myTrainer.newExercise(STAFF_NOTATION, 20, 0, 11);
        myBeep.playBeep(0, 100);
        myScreen.clean();
      }

      if (myButtons.isPressed(5) || myMidi.isKeyPressed(5)) {
        myTrainer.newExercise(STAFF_NOTATION, 20, 12, 23);
        myBeep.playBeep(0, 100);
        myScreen.clean();
      }

      if (myButtons.isPressed(7) || myMidi.isKeyPressed(7)) {
        myTrainer.newExercise(STAFF_NOTATION, 20, -12, -1);
        myBeep.playBeep(0, 100);
        myScreen.clean();
      }

      if (myButtons.isPressed(9) || myMidi.isKeyPressed(9)) {
        myTrainer.newExercise(STAFF_NOTATION, 20, -24, -13);
        myBeep.playBeep(0, 100);
        myScreen.clean();
      }

      break;

    case EXERCISE_STOP:
      myBeep.playBeep(0, 100);
      myBeep.playBeep(4, 100);
      myBeep.playBeep(8, 100);
      myBeep.playBeep(12, 200);
      myTrainer.setState(CONGRAT);

      break;

    case CONGRAT:
      for (byte i = 0; i < BUTTON_NUM; i++)
        if (myButtons.isPressed(i) || myMidi.isKeyPressed(i))
          myTrainer.setState(MENU);

      break;

    case CORRECT_ANSWER:
      delay(100);
      myBeep.playBeep(myTrainer.getCurrentNoteValue(), 100);
      myTrainer.setState(ROLLING);  // rettriger a new note
      break;

    case WRONG_ANSWER:
      delay(100);
      myTrainer.setState(ASKING);  // no not rettriger a new note
      break;

    case ROLLING:
      delay(100 * rand() % 5);
      myTrainer.randomizeNote();
      myTrainer.setState(ASKING);  // forces back to "asking" state so that the display is triggered

      break;

    case ASKING:

      askingQuestion();
      break;
  }
}

void manageDisplay() {
  DEBUGVAL("myTrainer.getCurrentState()", myTrainer.getCurrentState())
  switch (myTrainer.getCurrentState()) {
    case MENU:
      myScreen.displayMenu();
      break;

    case CONGRAT:
      myScreen.displayCongratulation(myTrainer.getLastAverageTimeSeconds(), myTrainer.getCurrentExerciseTxt());
      break;

    case ASKING:
      displayNoteNormally();
      break;

    case CORRECT_ANSWER:
      displayNoteNormally();
      myScreen.writeTextLine2("Good!");
      break;

    case WRONG_ANSWER:
      displayNoteNormally();
      // display wrong answer message
      char phrase2[20] = "That's ";
      char* pressedNoteName = myTrainer.getNoteName(myTrainer.getLastNotePressed());
      strcat(phrase2, pressedNoteName);
      myScreen.writeTextLine2(phrase2);
      break;
  }
}

void displayNoteNormally() {

  switch (myTrainer.getCurrentExercise()) {
    case FRENCH_NOTATION:
    case ENGLISH_NOTATION:
      myScreen.clean();
      myScreen.writeText(myTrainer.getCurrentNoteName());
      myScreen.displayProgressbar(myTrainer.getNbResponses(), myTrainer.getMaxResponses());
      break;

    case STAFF_NOTATION:
      
      myScreen.eraseNoteOnStaff();
      myScreen.displayStaff();

      NoteHeight n = myTrainer.getNoteOnStaff(myTrainer.getCurrentNoteValue());
      myScreen.displayNoteOnStaff(n.noteOnStaff, n.extension);
      myScreen.displayProgressbarVertical(myTrainer.getNbResponses(), myTrainer.getMaxResponses());
      break;
  }
}

void askingQuestion() {
  char* currentNoteName = myTrainer.getCurrentNoteName();
  int noteHeard = myBeep.getMicHearingNote();
  if (noteHeard > -100)
    myTrainer.checkAnswer(noteHeard);


  for (byte i = 0; i < BUTTON_NUM; i++)
    if (myButtons.isPressed(i) || myMidi.isKeyPressed(i)) {
      Serial.print("The piano key ");
      Serial.print(i + 1);
      Serial.println(" is pressed");

      myTrainer.checkAnswer(i);
    }
}
