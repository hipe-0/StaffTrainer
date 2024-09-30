// Trainer.h
// by Hippolyte Mounier
// 2024-05-20
//
// Goal: manages the musical logic of the training, no display, no button management

#include "iferror.h"
enum TrainerStates { MENU,
                     ROLLING,
                     ASKING,
                     CORRECT_ANSWER,
                     WRONG_ANSWER,
                     EXERCISE_STOP,
                     CONGRAT };


enum Exercises { FRENCH_NOTATION,
                 ENGLISH_NOTATION,
                 STAFF_NOTATION };

struct NoteHeight {
  int noteOnStaff;
  char* extension;
};


#define MAX_RESPONSES 100

class Trainer {
private:
  int currentNote_ = 0;
  enum TrainerStates currentState_;
  enum Exercises currentExercise_;
  int minNote_ = 0;
  int maxNote_ = 11;
  unsigned long startMillis_;
  long lastAverageTime_ = 0;
  int lastPianoNotePressed_ = 0;
  bool isCurrentlyInFlatMode_ = true;

  char* noteNames_[12] = { "do", "do#", "re", "re#", "mi", "fa", "fa#", "sol", "sol#", "la", "la#", "si" };
  char* noteNamesFlats_[12] = { "do", "reb", "re", "mib", "mi", "fa", "solb", "sol", "lab", "la", "sib", "si" };

  char* noteNamesEN_[12] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
  char* noteNamesENFlats_[12] = { "C", "Db", "D", "Eb", "E", "F", "Gb", "G", "Ab", "A", "Bb", "B" };

  int noteOnStaff_[12] = { 0, 0, 1, 1, 2, 3, 3, 4, 4, 5, 5, 6 };
  int noteOnStaffFlats_[12] = { 0, 1, 1, 2, 2, 3, 4, 4, 5, 5, 6, 6 };

  char* exercisesTxt_[6] = {
    "French notation",
    "English notation",
    "Staff notation",
    "Staff +1 octave",
    "Staff -1 octave",
    "Staff -2 octave"
  };

  int nbResponses_;
  int maxResponses_;
  long lastResponseTimes_[MAX_RESPONSES];

  long computeAverageTime_() {
    if (nbResponses_ == 0) return -1;  // error code if no score

    long averageTime = 0;
    for (int i = 0; i < nbResponses_; i++)
      averageTime += lastResponseTimes_[i];

    averageTime = averageTime / nbResponses_;

    return averageTime;
  }

  char lastChar_(char* string) {
    int len = strlen(string);
    return string[len - 1];
  }

  int moduloBy12_(int x) {  // modulo, remainer of the division by 12
    if (x > 0)
      return x % 12;
    else
      return (x + 96) % 12;  // avoid device dependent overflow that happens with negative operand value.
  }


public:

  Trainer() {
    DEBUGTXT("Trainer Init started")

    maxResponses_ = MAX_RESPONSES;
    currentState_ = MENU;
    currentExercise_ = FRENCH_NOTATION;
  }

  void setState(enum TrainerStates state) {
    currentState_ = state;
  }

  void newExercise(enum Exercises exercice, int maxResponses, int minNote, int maxNote) {
    IFERROR(maxResponses > MAX_RESPONSES, "Maximum number of responses reached")
    maxResponses_ = maxResponses;
    currentExercise_ = exercice;
    minNote_ = minNote;
    maxNote_ = maxNote;
    restartTraining();
  }

  void stopExercise() {
    if (currentState_ = ASKING)
      lastAverageTime_ = computeAverageTime_();

    currentState_ = EXERCISE_STOP;
  }

  void restartTraining() {
    if (currentState_ = ASKING)
      lastAverageTime_ = computeAverageTime_();

    nbResponses_ = 0;  // reseting the counter
    startMillis_ = millis();
    srand(millis());
    currentState_ = ROLLING;
  }


  int getCurrentState() {
    return currentState_;
  }

  void randomizeNote() {

    int old = currentNote_;
    //  find a new note that is different than the previous one
    do {
      currentNote_ = minNote_ + rand() % (maxNote_ - minNote_ + 1);
    } while (currentNote_ == old);

    isCurrentlyInFlatMode_ = rand() % 2;

    DEBUGVAL("New randomized note", currentNote_);
  }

  int getCurrentNoteValue() {
    return currentNote_;
  }

  char* getCurrentNoteName() {
    return getNoteName(currentNote_);
  }

  char* getNoteName(int i) {
    i = moduloBy12_(i);

    if (currentExercise_ == FRENCH_NOTATION)
      if (isCurrentlyInFlatMode_)
        return noteNamesFlats_[i];
      else
        return noteNames_[i];
    else if (isCurrentlyInFlatMode_)
      return noteNamesENFlats_[i];
    else
      return noteNamesEN_[i];
  }

  NoteHeight getNoteOnStaff(int i) {
    int i2 = moduloBy12_(i);
    int ioct = (i - i2) / 12;

    NoteHeight n = { 0, " " };

    if (isCurrentlyInFlatMode_) {
      char extension = lastChar_(noteNamesENFlats_[i2]);
      DEBUGVAL("extension", extension)
      if (extension == 'b')
        n.extension = "b";

      n.noteOnStaff = noteOnStaffFlats_[i2] + ioct * 7;
      DEBUGVAL2("n.noteOnStaff", n.noteOnStaff, "n.extention", n.extension)
      DEBUGVAL("getNoteName(i)", getNoteName(i))
      return n;
    }

    else {  // currently in Sharp mode
      char extension = lastChar_(noteNamesEN_[i2]);
      DEBUGVAL("extension", extension)
      if (extension == '#')
        n.extension = "#";

      n.noteOnStaff = noteOnStaff_[i2] + ioct * 7;
      DEBUGVAL2("n.noteOnStaff", n.noteOnStaff, "n.extention", n.extension)
      DEBUGVAL("getNoteName(i)", getNoteName(i))
      return n;
    }
  }


  bool checkAnswer(int buttonPressed) {
    lastPianoNotePressed_ = buttonPressed;

    DEBUGVAL2("currentNote_", currentNote_, "buttonPressed", buttonPressed)

    if (moduloBy12_(buttonPressed) == moduloBy12_(currentNote_))  // match is found
    {
      currentState_ = CORRECT_ANSWER;
      if (nbResponses_ < maxResponses_) {
        lastResponseTimes_[nbResponses_++] = millis() - startMillis_;
        startMillis_ = millis();  // restart counter
      } else
        stopExercise();

      return true;
    } else {
      currentState_ = WRONG_ANSWER;
      return false;
    }
  }

  float getLastAverageTimeSeconds() {
    return lastAverageTime_ * 0.001;
  }

  int getNbResponses() {
    return nbResponses_;
  }

  int getMaxResponses() {
    return maxResponses_;
  }

  int getLastNotePressed() {
    return lastPianoNotePressed_;
  }

  enum Exercises getCurrentExercise() {
    return currentExercise_;
  }

  char* getCurrentExerciseTxt()
  {
    return exercisesTxt_[currentExercise_];
  }
};