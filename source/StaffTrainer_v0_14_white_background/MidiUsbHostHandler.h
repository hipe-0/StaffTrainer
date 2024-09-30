#include "USBHost_t36.h"
#include "iferror.h"

bool isKeyDown_[128];

void NoteOnHandler(uint8_t channel, uint8_t pitch, uint8_t velo) {
  DEBUGVAL("NoteOnHandler pitch", pitch)
  isKeyDown_[pitch] = true;
}

void NoteOffHandler(uint8_t channel, uint8_t pitch, uint8_t velo) {
  DEBUGTXT("NoteOffHandler")
  isKeyDown_[pitch] = false;
}


class MidiUsbHostHandler {
private:
  USBHost usbHost_;
  MIDIDevice midiUSBHost_;
  bool previousKeyDown_[128];
  bool isKeyPressed_[128];

public:
  MidiUsbHostHandler()
    : midiUSBHost_(usbHost_) {
  }

  void init() {
    usbHost_.begin();

    midiUSBHost_.setHandleNoteOn(NoteOnHandler);
    midiUSBHost_.setHandleNoteOff(NoteOffHandler);

    for (int i = 0; i < 128; i++) {
      isKeyDown_[i] = false;
      previousKeyDown_[i] = false;
      isKeyPressed_[i] = false;
    }
  }

  void loop() {
    do {
    } while (midiUSBHost_.read());  // handlers will be called if data is available

    for (int i = 0; i < 128; i++)
      isKeyPressed_[i] = (isKeyDown_[i] && !previousKeyDown_[i]);

    for (int i = 0; i < 128; i++)
      previousKeyDown_[i] = isKeyDown_[i];
  }

#define MIDI_MIDDLE_C 48

  bool isKeyPressed(int pitch) {
    int midiPitch = MIDI_MIDDLE_C + pitch;
    IFERROR(midiPitch < 0 || midiPitch >= 128, "isKeyPressed error, midi pitch out of range 0-127")
    return isKeyPressed_[midiPitch];
  }
};
